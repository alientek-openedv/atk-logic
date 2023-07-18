/**
 ****************************************************************************************************
 * @author      正点原子团队(ALIENTEK)
 * @date        2023-07-18
 * @license     Copyright (c) 2023-2035, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:zhengdianyuanzi.tmall.com
 *
 ****************************************************************************************************
 */

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
    QStringList markList;
    markList.append("tlc5620");
    markList.append("usb_power_delivery");
    atk_decoder_inst* prev_di = nullptr;
    {
        QVector<QString> nameList;
        nameList.append(firstName);
        for(auto k : json["main"].toObject()["second"].toArray())
            nameList.append(k.toString());
        for(auto &decodeName : nameList){
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
            atk_decoder* p_decoder= atk_decoder_get_by_id(row["main"].toObject()["id"].toString().toUtf8().constData());
            if( p_decoder == NULL ){
                LogHelp::write("srd_decoder_get failed");
                return false;
            }
            atk_GHashTable* options_tab  = atk_decoder_hashtable_create();
            for(auto i : row["options"].toArray()){
                atk_decoder_hashtable_set_option(options_tab, p_decoder,
                                                 i.toObject()["id"].toString().toUtf8().constData(),
                        i.toObject()["value"].toString().toUtf8().constData());
            }
            if(m_isFirst)
                m_decoder_inst = atk_decoder_inst_new(m_decodeSession, row["main"].toObject()["id"].toString().toUtf8().constData(), options_tab);
            else
                atk_decoder_inst_option_set(m_decoder_inst, options_tab);
            atk_decoder_hashtable_destroy(options_tab);
            if(m_decoder_inst == nullptr){
                LogHelp::write("atk_inst_new failed");
                delete p_decoder;
                return false;
            }
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
                if(temp=="-"&&markList.contains(row["main"].toObject()["id"].toString()))
                    temp="-1";
                if(temp!="-"){
                    m_channelList.append(temp.toInt());
                    m_sendChannelList.append(index);
                    atk_decoder_hashtable_set_channel(probes_tab, i.toObject()["id"].toString().toUtf8().constData(), index);
                    index++;
                }
            }
            atk_decoder_inst_channel_set_all(m_decoder_inst, probes_tab);
            atk_decoder_hashtable_destroy(probes_tab);
            
            if(m_isFirst){
                if(prev_di!=nullptr)
                    atk_decoder_inst_stack(m_decodeSession, prev_di, m_decoder_inst);
                if(prev_di==nullptr)
                    prev_di=m_decoder_inst;
            }
            m_decodeIndex.append(p_decoder->name);
        }
        atk_decoder_session_metadata_set_samplerate(m_decodeSession, samplingFrequency);
        atk_decoder_pd_output_callback_add(m_decodeSession, ATK_OUTPUT_ANN, _output_callback, this);
        if(atk_decoder_session_start(m_decodeSession) != ATK_OK){
            LogHelp::write("atk_session_start error");
            return false;
        }
    }
    m_isFirst=false;
    return true;
}

static void _output_callback(struct atk_proto_data *pData, void *cb_data)
{
    DecodeController* decode=(DecodeController*)cb_data;
    atk_proto_data_annotation*  pda = (atk_proto_data_annotation*) pData->data;
    QString name=QString(pData->pdo->di->decoder->name);

    DecodeRowData data;
    data.start=pData->start_sample+decode->m_decodeStart;
    data.end=pData->end_sample+decode->m_decodeStart;
    data.type=pda->ann_class;
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
    if(data.longText.size()>200)
        data.longText=data.longText.left(200);
    if(decode->m_isDelete || (data.text.isEmpty()&&data.longText.isEmpty()) || decode->m_isStop)
        return;
    if(decode->m_rowList.contains(name)){
        decode->m_rowListMutex.lock();
        decode->m_rowList[name]->mutex.lock();
        qint32 count=decode->m_rowList[name]->row.count();
        decode->m_rowList[name]->mutex.unlock();
        qint32 row=pda->ann_row;
        if(decode->m_rowList[name]->isSingle)
            row=0;
        if(row>=0&&row<count){
            decode->m_rowList[name]->mutex.lock();
            decode->m_rowList[name]->row[row].data.append(data);
            if(decode->m_rowList[name]->row[row].dataCount%100000==0){
                DecodeRowDataNode temp;
                temp.index=decode->m_rowList[name]->row[row].dataCount;
                temp.start=decode->m_rowList[name]->row[row].data[temp.index].start;
                decode->m_rowList[name]->row[row].node.append(temp);
            }
            decode->m_rowList[name]->row[row].dataCount++;
            decode->m_rowList[name]->mutex.unlock();
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
        decode->m_rowListMutex.unlock();
    }
}

QString DecodeController::getDecodeID()
{
    return m_decodeID;
}

QJsonObject DecodeController::getJsonObject()
{
    return m_recode.json;
}

void DecodeController::setState(qint8 state)
{
    m_lock.lock();
    m_state=state;
    m_lock.unlock();
}

void DecodeController::decodeAnalysis(Segment* segment, qint32 channelCount, qint64 ullDecodeStart, qint64 ullDecodeEnd, qint64 maxSample_)
{
    if(!m_decodeSession)
        return;
    setState(1);
    segment->appendCite();
    m_recode.channelCount=channelCount;
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
            ii.node.clear();
        }
    }
    m_orderListMutex.lock();
    m_orderList.clear();
    m_orderListMutex.unlock();
    m_multiply=segment->GetMultiply();
    quint64 maxSample=1000;
    for(qint32 i=0;i<segment->GetChannelNum();i++){
        qint64 temp=segment->GetMaxSample(i,false);
        if(temp>maxSample)
            maxSample=qMin(temp,maxSample_);
    }
    if(maxSample<=0||m_multiply<=0)
        goto end;
    
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
    if(ullDecodeStart>maxSample)
        ullDecodeStart=maxSample;
    if(ullDecodeEnd>maxSample)
        ullDecodeEnd=maxSample;

    m_decodeStart=ullDecodeStart-segment->GetStartOffsetSampling(m_channelList[0]);
    ullDecodeStart+=segment->GetStartOffsetSampling(m_channelList[0]);
    ullDecodeEnd+=segment->GetStartOffsetSampling(m_channelList[0]);
    {
        quint64 ullChunkStart = 0, ullDecodeEnd2 = ullDecodeEnd - ullDecodeStart;
        quint64 ullBlockStart = ullDecodeStart;
        qint32 schedule,scheduleIndex=qMax((double)ullDecodeEnd2/100/_MAX_CHUNK_SIZE-1,1.),scheduleCount=0;
        qint32 count=m_sendChannelList.count();

        while(ullChunkStart<ullDecodeEnd2){
            for(qint32 i=0;i<count;i++){
                memset(&inbuf[i], 0, sizeof(atk_input_data));
            }

            quint64 ullChunkEnd = 0;
            for(qint32 i=0;i<count;i++){
                if(m_channelList[i]>-1)
                {
                    inbuf[m_sendChannelList[i]].data=segment->GetSampleBlock(ullBlockStart, ullChunkEnd, m_channelList[i]);
                    if(!inbuf[m_sendChannelList[i]].data)
                        inbuf[m_sendChannelList[i]].constant=segment->GetSample(ullBlockStart, m_channelList[i],false);
                }
            }

            ullChunkEnd -= ullDecodeStart;
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
    segment->lessenCite();
    setState(0);
    emit sendDecodeSchedule(m_decodeID, 100);
}

void DecodeController::stopDedoceAnalysis()
{
    m_lock.lock();
    m_isStop=true;
    if(m_state==1 && m_decodeSession!=nullptr){
        m_state=2;
        atk_decoder_session_terminate_reset(m_decodeSession);
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

bool DecodeController::restart(Segment* segment, qint64 maxUnit)
{
    if(m_recode.isRecode){
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
        decodeAnalysis(segment,m_recode.channelCount,m_recode.ullDecodeStart,m_recode.ullDecodeEnd,maxUnit/multiply);
        segment->lessenCite();
        return true;
    }
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
            right = mid; 

        }
        else if(row->data[mid].end < start){
            left = mid+1;
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
    
    DecodeRow* row=&rows->row[index];
    qint32 len=row->node.length();
    qint32 left=0,right=len;
    while(left < right)
    {
        int mid=qFloor((left+right)/2.);
        if(mid+1>=len)
        {
            if(len<4){
                if(len==2){
                    if(row->node[1].start>=start){
                        left_=row->node[0].index;
                        right_=row->node[1].index;
                        break;
                    }else{
                        left_=row->node[1].index;
                        right_=row->data.length();
                        break;
                    }
                }else if(len==3){
                    if(row->node[1].start<=start && row->node[2].start>=start){
                        left_=row->node[1].index;
                        right_=row->node[2].index;
                        break;
                    }else{
                        left_=row->node[0].index;
                        right_=row->node[1].index;
                        break;
                    }
                }else{
                    left_=row->node[0].index;
                    right_=row->data.length();
                }
            }else{
                left_=row->node[mid].index;
                right_=row->data.length();
            }
            break;
        }
        if(row->node[mid].start<=start && row->node[mid+1].start>=start){
            left_=row->node[mid].index;
            right_=row->node[mid+1].index;
            break;
        }
        else if(row->node[mid].start > start){
            right = mid; 
        }
        else if(row->data[mid].end < start){
            left = mid+1;
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
