#include "decode_controller.h"

#include <QFile>


DecodeController::DecodeController(const QString decodeID, QObject *parent)
    : QObject{parent},m_decodeID(decodeID)
{
    m_thread.start();
    this->moveToThread(&m_thread);
    m_decodeSession = atk_decoder_session_new();
    if(!m_decodeSession)
        LogHelp::write("atk_decoder_session_new error");
}

DecodeController::~DecodeController()
{
    deleteData();
    m_thread.quit();
    m_thread.wait();
    disconnect();
    if(m_decodeSession)
        atk_decoder_session_destroy(m_decodeSession);
}

bool DecodeController::analysisJSON(QJsonObject json, qint64 samplingFrequency)
{
    if(!m_decodeSession){
        m_decodeSession = atk_decoder_session_new();
        if(!m_decodeSession){
            LogHelp::write("atk_decoder_session_new error");
            return false;
        }
    }
    deleteData();

    atk_decoder_session_terminate_reset(m_decodeSession);

    m_isStop=false;
    m_recode.json=json;
    QString firstName=json["main"].toObject()["first"].toString();
    QList<qint32> ls;
    ls<<1<<2<<4<<8;
    if(ls.contains(json["main"].toObject()["height"].toInt()))
        m_height=json["main"].toObject()["height"].toInt();
    m_isLockRow=json["main"].toObject()["isLockRow"].toBool();

    atk_decoder_inst* prev_di = nullptr;
    {
        QStringList nameList;
        nameList.append(firstName);
        for(auto k : json["main"].toObject()["second"].toArray())
            nameList.append(k.toString());
        LogHelp::write(QString("        协议解析: %1，解码ID：%2").arg(nameList.join("-"),json["main"].toObject()["decodeID"].toString()));
        for(auto &decodeName : nameList){
            //添加显示列表
            QJsonObject row=json[decodeName].toObject();

            DecodeRowRoot* rowList=new DecodeRowRoot();
            rowList->isSingle=row["main"].toObject()["isSingle"].toBool();
            for(auto i : row["annotation_rows"].toArray()){
                QJsonObject obj=i.toObject();
                DecodeRow row;
                row.id=obj["id"].toString();
                row.desc=obj["desc"].toString();
                row.isShow=obj["isShow"].toBool();
                QJsonArray list;
                for (auto ii : obj["ann_classes"].toArray())
                    list.append(ii.toString().toInt());
                QJsonObject js;
                js["list"]=list;
                js["desc"]=row.desc;
                row.rowType[row.id]=js;
                rowList->row.append(row);
            }

            m_rowList[decodeName]=rowList;

            //获取解码器
            atk_decoder* p_decoder= atk_decoder_get_by_id(row["main"].toObject()["id"].toString().toUtf8().constData());
            if( p_decoder == NULL ){
                LogHelp::write("srd_decoder_get failed");
                return false;
            }

            //通道属性参数设置
            atk_GHashTable* options_tab  = atk_decoder_hashtable_create();
            for(auto i : row["options"].toArray()){
                atk_decoder_hashtable_set_option(options_tab, p_decoder,
                                                 i.toObject()["id"].toString().toUtf8().constData(),
                        i.toObject()["value"].toString().toUtf8().constData());
            }

            //新建实例
            if(m_isFirst)
                m_decoder_inst = atk_decoder_inst_new(m_decodeSession, row["main"].toObject()["id"].toString().toUtf8().constData(), options_tab);
            else
                atk_decoder_inst_option_set(m_decoder_inst, options_tab);
            atk_decoder_hashtable_destroy(options_tab);
            if(m_decoder_inst == nullptr){
                LogHelp::write("atk_inst_new failed");
                return false;
            }
            /* 通道设置 */
            atk_GHashTable* const probes_tab = atk_decoder_hashtable_create();
            qint32 index=0;
            for(auto i : row["channels"].toArray()){
                QString temp=i.toObject()["value"].toString();
                if(temp!="-"){
                    m_channelList.append(temp.toInt());
                    m_sendChannelList.append(index);
                    atk_decoder_hashtable_set_channel(probes_tab, i.toObject()["id"].toString().toUtf8().constData(), index);
                    index++;
                }
            }
            for(auto i : row["opt_channels"].toArray()){
                QString temp=i.toObject()["value"].toString();
                if(temp!="-"){
                    m_channelList.append(temp.toInt());
                    m_sendChannelList.append(index);
                    atk_decoder_hashtable_set_channel(probes_tab, i.toObject()["id"].toString().toUtf8().constData(), index);
                    index++;
                }
            }
            atk_decoder_inst_channel_set_all(m_decoder_inst, probes_tab);
            atk_decoder_hashtable_destroy(probes_tab);
            //堆叠协议
            if(m_isFirst){
                if(prev_di!=nullptr)
                    atk_decoder_inst_stack(m_decodeSession, prev_di, m_decoder_inst);
                if(prev_di==nullptr)
                    prev_di=m_decoder_inst;
            }
            m_decodeIndex.append(p_decoder->name);
        }

        //设置采样率
        atk_decoder_session_metadata_set_samplerate(m_decodeSession, samplingFrequency);
        /* 设置输出回调函数 */
        atk_decoder_pd_output_callback_add(m_decodeSession, ATK_OUTPUT_ANN, _output_callback, this);
//        atk_decoder_pd_output_callback_add(m_decodeSession, ATK_OUTPUT_BINARY, _output_binary_callback, this);
        if(atk_decoder_session_start(m_decodeSession) != ATK_OK){
            LogHelp::write("atk_session_start error");
            return false;
        }
    }
    m_isFirst=false;
    return true;
}

static void Insert_sort(QList<DecodeRowData> &arr)
{
    int length=arr.length();
    if (length < 2)return;
    for (int i = 1;i < length;i++)
    {
        DecodeRowData tmp = arr[i];
        int j = i - 1;
        for (;j >= 0;j--)
        {
            if (arr[j].start <= tmp.start) break;
            arr[j + 1] = arr[j];
        }
        arr[j+1] = tmp;
    }
}

static void _output_binary_callback(struct atk_proto_data *pData, void *cb_data)
{
    DecodeController* decode=(DecodeController*)cb_data;
    atk_proto_data_binary*  pda = (atk_proto_data_binary*) pData->data;
    qDebug()<<pda->size<<pda->bin_class<<pda->data;
}

static void _output_callback(struct atk_proto_data *pData, void *cb_data)
{
    DecodeController* decode=(DecodeController*)cb_data;
    atk_proto_data_annotation*  pda = (atk_proto_data_annotation*) pData->data;
    QString name=QString(pData->pdo->di->decoder->name);

    DecodeRowData data;
    data.start=pData->start_sample+decode->m_decodeStart;
    data.end=pData->end_sample+decode->m_decodeStart;
    data.type=qAbs(pda->ann_class);
    data.decodeIndex=decode->m_decodeIndex.indexOf(name);
    char** annotations = (char**)pda->ann_text;
    int cnt = 0;
    QString str;
    while( *annotations )
    {
        str=QString(*annotations);
        if(cnt==0)
            data.longText=str;
        else if(!str.isEmpty())
            data.text=str;
        cnt++;
        annotations++;
    }

    if(decode->m_isDelete || (data.text.isEmpty()&&data.longText.isEmpty()) || decode->m_isStop)
        return;

    //添加数据列表
    if(decode->m_rowList.contains(name)){
        decode->m_rowListMutex.lock();
        decode->m_rowList[name]->mutex.lock();
        qint32 count=decode->m_rowList[name]->row.count();
        decode->m_rowList[name]->mutex.unlock();
        qint32 row=pda->ann_row;
        if(decode->m_rowList[name]->isSingle)
            row=data.type;
        if(row>=0&&row<count){
            decode->m_rowList[name]->mutex.lock();
            if(decode->m_rowList[name]->row[row].tmpData.isEmpty()){
                decode->m_rowList[name]->row[row].tmpData.append(data);
                for(qint32 j=0;j<decode->m_rowList[name]->row.length();j++){
                    if(j!=row&& decode->m_rowList[name]->row[j].tmpData.length()>0){
                        if(data.start>decode->m_rowList[name]->row[j].tmpData[0].start)
                            data_insert(decode, name, j);
                    }
                }
            }
            else{
                decode->m_rowList[name]->row[row].tmpData.append(data);
                if(data.end>decode->m_rowList[name]->row[row].tmpData[0].end){
                    data_insert(decode, name, row);
                }
            }
            decode->m_rowList[name]->mutex.unlock();
        }
        decode->m_rowListMutex.unlock();
    }
}

static void data_insert(DecodeController* decode, QString name, qint32 row){
    Insert_sort(decode->m_rowList[name]->row[row].tmpData);
    for(auto j:decode->m_rowList[name]->row[row].tmpData){
        decode->m_rowList[name]->row[row].data.append(j);
        if(decode->m_rowList[name]->row[row].dataCount%100000==0){
            DecodeRowDataNode temp;
            temp.index=decode->m_rowList[name]->row[row].dataCount;
            temp.start=decode->m_rowList[name]->row[row].data[temp.index].start;
            decode->m_rowList[name]->row[row].node.append(temp);
        }
        decode->m_rowList[name]->row[row].dataCount++;
        //添加排序分组
        decode->m_orderListMutex.lock();
        if(decode->m_orderList.isEmpty())
            decode->m_orderList.append(DataOrder(name,row,1));
        else
        {
            if(decode->m_orderList.last().rowIndex==row && decode->m_orderList.last().name==name)
                decode->m_orderList[decode->m_orderList.count()-1].len++;
            else
                decode->m_orderList.append(DataOrder(name,row,1));
        }
        decode->m_orderListMutex.unlock();
    }
    decode->m_rowList[name]->row[row].tmpData.clear();
}

QString DecodeController::getDecodeID()
{
    return m_decodeID;
}

QJsonObject DecodeController::getJsonObject()
{
    return m_recode.json;
}

void DecodeController::reloadDecoder()
{
    m_decodeSession = atk_decoder_session_new();
    if(!m_decodeSession)
        LogHelp::write("atk_decoder_session_new error");
}

void DecodeController::setState(qint8 state)
{
    m_lock.lock();
    m_state=state;
    m_lock.unlock();
}

void DecodeController::decodeAnalysis(Segment* segment, qint64 ullDecodeStart, qint64 ullDecodeEnd, qint64 maxSample_, bool isContinuous)
{
    if(!m_decodeSession)
        return;
    LogHelp::write(QString("        协议线程启动：%1").arg(m_recode.json["main"].toObject()["decodeID"].toString()));
    setState(1);
    segment->appendCite();
    m_multiply=segment->GetMultiply();
    m_recode.ullDecodeStart=ullDecodeStart;
    m_recode.ullDecodeEnd=ullDecodeEnd;
    m_recode.isRecode=true;
    m_isDelete=false;
    m_isReportedError=false;
    QVector<atk_input_data> inbuf(m_sendChannelList.count());
    emit sendDecodeReset(m_decodeID);
    emit sendDecodeSchedule(m_decodeID, 0);
    for(auto &i : m_rowList)
    {
        for(auto &ii:i->row)
        {
            ii.data.clear();
            ii.tmpData.clear();
            ii.node.clear();
        }
    }
    m_orderListMutex.lock();
    m_orderList.clear();
    m_orderListMutex.unlock();

    quint64 maxSample=1000;
    for(qint32 i=0;i<segment->GetChannelNum();i++){
        qint64 temp=segment->GetMaxSample(i,false);
        if(temp>maxSample){
            if(isContinuous)
                maxSample=temp;
            else
                maxSample=qMin(temp,maxSample_);
        }

    }
    if(maxSample<=0||m_multiply<=0)
        goto end;
    //计算起始位置和结束位置
    if(ullDecodeStart<0)
        ullDecodeStart=ullDecodeStart==-1?maxSample:0;
    else
        ullDecodeStart/=m_multiply;
    if(ullDecodeEnd<0)
        ullDecodeEnd=ullDecodeEnd==-1?maxSample:0;
    else
        ullDecodeEnd/=m_multiply;
    if(ullDecodeStart>ullDecodeEnd)
    {
        ullDecodeStart ^= ullDecodeEnd;
        ullDecodeEnd ^= ullDecodeStart;
        ullDecodeStart ^= ullDecodeEnd;
    }
    if(!isContinuous){
        if(ullDecodeStart>maxSample)
            ullDecodeStart=maxSample;
        if(ullDecodeEnd>maxSample)
            ullDecodeEnd=maxSample;
    }
    m_decodeStart=ullDecodeStart-segment->GetStartOffsetSampling(m_channelList[0]);
    ullDecodeStart+=segment->GetStartOffsetSampling(m_channelList[0]);
    ullDecodeEnd+=segment->GetStartOffsetSampling(m_channelList[0]);
    {
        quint64 ullChunkStart = 0, ullDecodeEnd2 = ullDecodeEnd - ullDecodeStart;
        quint64 ullBlockStart = ullDecodeStart;
        qint32 schedule,scheduleIndex=qMax((double)ullDecodeEnd2/100/_MAX_CHUNK_SIZE-1,1.),scheduleCount=0;
        qint32 count=m_sendChannelList.count();

        while(ullChunkStart<ullDecodeEnd2 || isContinuous){
            if(isContinuous){
                m_streamLock.lock();
                if(m_streamRecode.ullDecodeEnd>m_streamRecode.ullMaxSample)
                    m_streamRecode.ullDecodeEnd=m_streamRecode.ullMaxSample;
                if(m_streamRecode.ullDecodeEnd==m_streamRecode.ullMaxSample&&ullChunkStart+ullDecodeStart>=m_streamRecode.ullMaxSample){
                    m_streamLock.unlock();
                    break;
                }
                ullDecodeEnd2=m_streamRecode.ullDecodeEnd - ullDecodeStart;
                m_streamLock.unlock();
                if(ullChunkStart>=ullDecodeEnd2){
                    if(!isRun())
                        break;
                    QThread::msleep(50);
                    continue;
                }
            }
            for(qint32 i=0;i<count;i++)
                memset(&inbuf[i], 0, sizeof(atk_input_data));

            quint64 ullChunkEnd = 0;
            for(qint32 i=0;i<count;i++){
                if(m_channelList[i]>-1)
                {
                    inbuf[m_sendChannelList[i]].data=segment->GetSampleBlock(ullBlockStart, ullChunkEnd, m_channelList[i]);
                    if(!inbuf[m_sendChannelList[i]].data)
                        inbuf[m_sendChannelList[i]].constant=segment->GetSample(ullBlockStart, m_channelList[i],false);
                }
            }

            ullChunkEnd -= ullDecodeStart;//相对位置
            if (ullChunkEnd > ullDecodeEnd2)
                ullChunkEnd = ullDecodeEnd2;
            if (ullChunkEnd - ullChunkStart > _MAX_CHUNK_SIZE)
                ullChunkEnd = ullChunkStart + _MAX_CHUNK_SIZE;
            if(isRun()){
                qint32 state=atk_decoder_session_send(m_decodeSession, ullChunkStart, ullChunkEnd, &inbuf[0]);
                if(state!=ATK_OK && state!=ATK_ERR_TERM_REQ){
                    m_isReportedError=true;
                    LogHelp::write("atk_session_send error");
                    break;
                }
            }else
                break;
            ullBlockStart = ullChunkEnd+ullDecodeStart;
            ullChunkStart = ullChunkEnd;
            scheduleCount++;
            if(!(scheduleCount%scheduleIndex))
            {
                scheduleCount=0;
                schedule=qMin((qint32)((double)ullChunkStart/ullDecodeEnd2*100),99);
                emit sendDecodeSchedule(m_decodeID, schedule);
            }
        }
    }
end:
    atk_decoder_session_send_eof(m_decodeSession);
    //等待剩余数据传输
    QThread::msleep(200);
    //结尾还原数据
    QList<QString> keys=m_rowList.keys();
    for(qint32 i=0;i<keys.count();i++){
        for(qint32 ii=0;ii<m_rowList[keys[i]]->row.count();ii++){
            Insert_sort(m_rowList[keys[i]]->row[ii].tmpData);
            for(auto j:m_rowList[keys[i]]->row[ii].tmpData){
                m_rowList[keys[i]]->row[ii].data.append(j);
                if(m_rowList[keys[i]]->row[ii].dataCount%100000==0){
                    DecodeRowDataNode temp;
                    temp.index=m_rowList[keys[i]]->row[ii].dataCount;
                    temp.start=m_rowList[keys[i]]->row[ii].data[temp.index].start;
                    m_rowList[keys[i]]->row[ii].node.append(temp);
                }
                m_rowList[keys[i]]->row[ii].dataCount++;
                //添加排序分组
                m_orderListMutex.lock();
                if(m_orderList.isEmpty())
                    m_orderList.append(DataOrder(keys[i],ii,1));
                else
                {
                    if(m_orderList.last().rowIndex==ii && m_orderList.last().name==keys[i])
                        m_orderList[m_orderList.count()-1].len++;
                    else
                        m_orderList.append(DataOrder(keys[i],ii,1));
                }
                m_orderListMutex.unlock();
            }
            m_rowList[keys[i]]->row[ii].tmpData.clear();
        }
    }
    segment->lessenCite();
    setState(0);
    emit sendDecodeSchedule(m_decodeID, 100);
    LogHelp::write(QString("        协议线程退出：%1").arg(m_recode.json["main"].toObject()["decodeID"].toString()));
}

bool DecodeController::decodeToPosition(qint32 multiply, qint64 ullPosition, qint64 maxSample_)
{
    if(ullPosition<0||maxSample_<=0)
        return false;
    m_multiply=multiply;
    m_streamLock.lock();
    m_streamRecode.ullDecodeEnd=ullPosition;

    qint64 start=m_recode.ullDecodeStart;
    qint64 end=m_recode.ullDecodeEnd;

    if(start<0)
        start=start==-1?maxSample_:0;
    else
        start/=m_multiply;
    if(end<0)
        end=end==-1?maxSample_:0;
    else
        end/=m_multiply;
    if(start>end)
    {
        start ^= end;
        end ^= start;
        start ^= end;
    }
    if(start>maxSample_)
        start=maxSample_;
    if(end>maxSample_)
        end=maxSample_;
    maxSample_=end;
    m_streamRecode.ullMaxSample=maxSample_;
    m_streamLock.unlock();
    if(ullPosition>=start && ullPosition<m_streamRecode.ullMaxSample && !m_isRestart && !isRun()){
        m_isRestart=true;
        return true;
    }
    return false;
}

void DecodeController::stopDedoceAnalysis()
{
    m_lock.lock();
    m_isStop=true;
    if(m_state==1 && m_decodeSession!=nullptr){
        m_state=2;
        atk_decoder_session_terminate_reset(m_decodeSession);
        LogHelp::write(QString("        协议线程停止：%1").arg(m_recode.json["main"].toObject()["decodeID"].toString()));
    }
    m_lock.unlock();
}

void DecodeController::waitStopDedoceAnalysis()
{
    while(true){
        m_lock.lock();
        qint8 temp=m_state;
        m_lock.unlock();
        if(temp==0)
            break;
        QApplication::processEvents(QEventLoop::AllEvents, 50);
        QThread::msleep(50);
    }
}

void DecodeController::setRecodeJSON(QJsonObject json)
{
    m_recode.json=json;
    m_recode.ullDecodeStart=json["main"].toObject()["start"].toString().toLongLong();
    m_recode.ullDecodeEnd=json["main"].toObject()["end"].toString().toLongLong();
}

bool DecodeController::restart(Segment* segment, qint64 maxUnit, bool isContinuous)
{
    m_isRestart=true;
    if(m_recode.isRecode){
        LogHelp::write(QString("        协议线程重新解析：%1").arg(m_recode.json["main"].toObject()["decodeID"].toString()));
        segment->appendCite();
        stopDedoceAnalysis();
        waitStopDedoceAnalysis();
        bool isAdd=analysisJSON(m_recode.json, segment->GetSamplingFrequency()*1000);
        if(!isAdd)
        {
            segment->lessenCite();
            LogHelp::write(QObject::tr("创建编码器失败。"));
            return false;
        }
        quint32 multiply=qMax((qint32)segment->GetMultiply(),1);
        decodeAnalysis(segment,m_recode.ullDecodeStart,m_recode.ullDecodeEnd,maxUnit/multiply,isContinuous);
        segment->lessenCite();
        m_isRestart=false;
        return true;
    }
    m_isRestart=false;
    return false;
}

void DecodeController::deleteData()
{
    stopDedoceAnalysis();
    m_isDelete=true;
    m_rowListMutex.lock();
    QStringList keys=m_rowList.keys();
    for(qint32 i=0;i<keys.count();i++){
        for(qint32 ii=0;ii<m_rowList[keys[i]]->row.count();ii++){
            m_rowList[keys[i]]->row[ii].data.clear();
            m_rowList[keys[i]]->row[ii].node.clear();
            m_rowList[keys[i]]->row[ii].rowType.clear();
        }
        m_rowList[keys[i]]->row.clear();
        delete m_rowList[keys[i]];
    }
    m_rowList.clear();
    m_orderListMutex.lock();
    m_orderList.clear();
    m_orderListMutex.unlock();
    m_channelList.clear();
    m_sendChannelList.clear();
    m_rowListMutex.unlock();
    m_decodeIndex.clear();
}

qint32 DecodeController::findDecodeRowFirstData(DecodeRowRoot *rows, qint32 index, qint32 left, qint32 right, qint64 start)
{
    rows->mutex.lock();
    //二分法查找首个数据
    DecodeRow* row=&rows->row[index];
    qint32 selectStart=-1;
    while(left < right)
    {
        int mid=(left+right)/2;
        if(row->data[mid].start<=start && row->data[mid].end>=start){
            selectStart=mid;
            break;
        }
        else if(row->data[mid].start > start){
            right = mid; // 中间的值大于目标值，向左收缩区间

        }
        else if(row->data[mid].end < start){
            left = mid+1;// 中间的值小于目标值，向右收缩区间
        }
        if(right==left)
            selectStart=left-(left==0?0:1);
    }
    rows->mutex.unlock();
    return selectStart;
}

void DecodeController::findDecodeRowDataNode(DecodeRowRoot* rows, qint32 index, qint32 &left_, qint32 &right_, qint64 start)
{
    rows->mutex.lock();
    //二分法查找首个节点
    DecodeRow* row=&rows->row[index];
    qint32 len=row->node.length();
    qint32 left=0,right=len;
    while(left < right)
    {
        int mid=qFloor((left+right)/2.);
        if(mid+1>=len)
        {
            if(len<4){
                if(len>=2){
                    for(qint32 i=1;i<=len;i++){
                        if(i==len){
                            left_=row->node[i-1].index;
                            right_=row->data.length()-1;
                        }else{
                            if(start<row->node[i].start){
                                left_=row->node[i-1].index;
                                right_=row->node[i].index;
                                break;
                            }
                        }
                    }
                }else{
                    left_=row->node[0].index;
                    right_=row->data.length()-1;
                }
            }else{
                if(row->node[mid].start>start){
                    left_=row->node[mid-1].index;
                    right_=row->node[mid].index;
                }else{
                    left_=row->node[mid].index;
                    right_=row->data.length()-1;
                }
            }
            break;
        }
        if(row->node[mid].start<=start && row->node[mid+1].start>start){
            left_=row->node[mid].index;
            right_=row->node[mid+1].index;
            break;
        }
        else if(row->node[mid].start > start){
            right = mid; // 中间的值大于目标值，向左收缩区间
        }
        else if(row->data[mid].end < start){
            left = mid+1;// 中间的值小于目标值，向右收缩区间
        }
    }
    rows->mutex.unlock();
}

bool DecodeController::isRun()
{
    m_lock.lock();
    qint8 temp=m_state;
    bool stop=m_isStop;
    m_lock.unlock();
    return temp==1 && !stop;
}
