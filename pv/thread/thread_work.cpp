#include "thread_work.h"

ThreadWork::ThreadWork(QObject *parent)
    : QObject{parent}
{
    m_thread.start();
    this->moveToThread(&m_thread);
}

void ThreadWork::destroy()
{
    setRun(2);
    m_thread.requestInterruption();
    m_thread.quit();
    m_thread.wait();
}

void ThreadWork::stop(bool waitClose)
{
    if(isRun()){
        LogHelp::write(QString("    手动停止任务线程"));
        setRun(2);
        if(waitClose){
            while(true){
                if(m_lock.try_lock()){
                    qint8 temp=m_run;
                    m_lock.unlock();
                    if(temp==0)
                        break;
                    QThread::msleep(50);
                }
                QApplication::processEvents(QEventLoop::AllEvents, 50);
            }
        }
    }
}

void ThreadWork::onDataIn(Data_* data)
{
    m_dataMutex.lock();
    m_dataList.append(data);
    m_dataMutex.unlock();
}

void ThreadWork::setLiveFollowing(bool isEnable)
{
    m_liveFollowing=isEnable;
}

void ThreadWork::initError(SessionError *sessionError)
{
    m_sessionError=sessionError;
}

ThreadWork::~ThreadWork()
{
    if(m_dataBuffer!=nullptr){
        if(m_dataBuffer->buf)
            delete[] m_dataBuffer->buf;
        delete m_dataBuffer;
    }
    disconnect();
}

void ThreadWork::DeviceRecvThread(USBControl *usb, Segment* segment, SessionConfig* config,
                                  double second, void* channelIDList, bool isBuffer, bool isRLE)
{
    LogHelp::write(QString("    采集任务线程启动"));
    setRun(1);
    segment->appendCite();
    usb->m_busy=true;
    QVector<qint8>* IDList=(QVector<qint8>*)channelIDList;
    if(m_dataBuffer!=nullptr&&m_dataBuffer->len>0)
    {
        delete[] m_dataBuffer->buf;
        delete m_dataBuffer;
        m_dataBuffer=nullptr;
    }
    while(!m_dataList.empty())
    {
        Data_* temp=m_dataList.first();
        if(temp->buf!=nullptr)
            delete[] temp->buf;
        delete temp;
        m_dataList.pop_front();
    }
    //启动读线程
    ThreadRead m_usbReadThread;
    {
        qint32 maxTransferSize=16;
        double depthS=segment->m_SamplingDepth*IDList->count()/qCeil(second);
        if(depthS<250000000)
            maxTransferSize=qCeil(depthS/250000000*READ_TRANSFER_LENGTH);
        connect(this,&ThreadWork::SendReadThreadStart,&m_usbReadThread,&ThreadRead::start);
        emit SendReadThreadStart(usb,maxTransferSize,this);
    }
    emit SendDeviceRecvSchedule(0,0,1);
    qint32 channelID,multiply=segment->GetMultiply();
    QVector<quint64> channelDataPosition;//通道当前位置记录
    qint64 channelDataCount=0;
    qint32 missCount=0;
    qint32 scheduleRecode=-1,triggerScheduleRecode=-1;
    quint64 vernierTriggerPosition=0;
    bool isExceedCapacity=false,isExceedBandwidth=false,isOffsetOrder=false;
    const static QString errorMsg=QObject::tr("数据异常，请重连硬件再次尝试。");
    QVector<quint32> dataCount;
    const static qint32 rleSize=512*1024;
    quint8* rleBuffer=new quint8[rleSize];//rle解压数据
    quint8* offsetBuffer=new quint8[rleSize];//rle解压数据
    bool isSendClose=false, isSetTime=false;
    memset(rleBuffer,0,rleSize);
    memset(offsetBuffer,0,rleSize);
    //    uint64_t t1=GetTickCount_();
    try {
        if(IDList->count()<1)
            throw QObject::tr("开启通道数量错误");
        for (int i=0;i<usb->m_ChannelCount;i++)
        {
            channelDataPosition.append(0);
            dataCount.append(0);
        }
        if(usb->IsInit()){
            while(usb->m_ThreadState==1&&isRun()){
                Data_* data = nullptr;
                m_dataMutex.lock();
                if(!m_dataList.isEmpty())
                {
                    data=m_dataList.first();
                    m_dataList.pop_front();
                }
                m_dataMutex.unlock();
                if (data!=nullptr)
                {
                    if (data->len > 0) {
                        //数据拼接
                        bool isAddData=false;
                        Data_* data2=new Data_();
                        if(m_dataBuffer!=nullptr&&m_dataBuffer->len>0)
                        {
                            data2->len=data->len+m_dataBuffer->len;
                            quint8* buffer=new quint8[data2->len];
                            memset(buffer,0,data2->len);
                            memcpy(buffer,m_dataBuffer->buf,m_dataBuffer->len);
                            memcpy(buffer+m_dataBuffer->len,data->buf,data->len);
                            delete[] m_dataBuffer->buf;
                            delete m_dataBuffer;
                            m_dataBuffer=nullptr;
                            data2->buf=buffer;
                            isAddData=true;
                        }else{
                            data2->buf=data->buf;
                            data2->len=data->len;
                        }
                        Analysis analysis(data2->buf,data2->len);
                        AnalysisData analysisData=analysis.getNextData();

                        while(analysisData.order!=-1&&isRun()&&usb->m_ThreadState==1){
                            //获取通道ID，占2字节
                            channelID = *analysisData.pData;
                            missCount=0;
                            switch(analysisData.order){
                            case 1:
                            {
                                if(!isBuffer && !isSendClose){
                                    isSendClose=true;
                                    emit SendDeviceRecvSchedule(1,7,1);
                                }

                                //循环当前取出的所有数据
                                quint8* data_=analysisData.pData+2;
                                quint32 datalen=analysisData.ullLen-2;
                                if(isRLE){
                                    memset(rleBuffer,0,rleSize);
                                    qint32 current=0;
                                    for (qint32 i = 2; i < analysisData.ullLen; i+=2)
                                    {
                                        memset(rleBuffer+current,analysisData.pData[i+1],(quint8)analysisData.pData[i]);
                                        current+=(quint8)analysisData.pData[i];
                                    }
                                    data_=rleBuffer;
                                    datalen=current;
                                }

                                //过滤掉所有字节，剩余不够1字节删的再记录
                                if(segment->GetStartOffsetSampling(channelID)>7){
                                    qint64 offset=segment->GetStartOffsetSampling(channelID);
                                    qint32 removeByte=offset/8;
                                    if(datalen>removeByte){
                                        memset(offsetBuffer,0,rleSize);
                                        memcpy(offsetBuffer,data_+removeByte,datalen-removeByte);
                                        segment->SetStartOffsetSampling(channelID,offset&7);
                                        datalen-=removeByte;
                                        data_=offsetBuffer;
                                    }else{
                                        segment->SetStartOffsetSampling(channelID,offset-datalen*8);
                                        datalen=0;
                                    }
                                }
                                if(datalen>0){
                                    SEGMENT_MSG msg=segment->SetSampleBlock(channelID,channelDataPosition[channelID],data_,datalen,!isRLE);
                                    if(msg!=ATK_CORRECT){
                                        switch(msg){
                                        case ATK_MEMORY_ERROR:
                                            throw QObject::tr("内存溢出，请使用64位软件或更高配置电脑！");
                                            break;
                                        case ATK_CHANNEL_ERROR:
                                            throw QObject::tr("数据异常，采集中止！");
                                            break;
                                        case ATK_POSITION_ERROR:
                                            throw QObject::tr("数据超出限定值，无法继续采集！");
                                            break;
                                        }
                                    }
                                }
                                channelDataPosition[channelID]+=datalen;

                                dataCount[channelID]+=datalen;
                                if(IDList->at(0)==channelID)
                                {
                                    channelDataCount+=datalen;
                                    qint32 schedule=qMin((quint64)(channelDataCount*8/(qreal)segment->m_SamplingDepth*100),99ull);
                                    if(scheduleRecode!=schedule)
                                    {
                                        emit SendDeviceRecvSchedule(schedule,3,1);
                                        scheduleRecode=schedule;
                                    }
                                }
                                break;
                            }
                            case 3:
                            {
                                if(isOffsetOrder)
                                    throw QObject::tr("指令异常，请重连硬件再次尝试。");
                                else
                                    isOffsetOrder=true;
                                quint8* pBuffer=new quint8[8];
                                quint8* pTemp=analysisData.pData+2;
                                memset(pBuffer,0,8);
                                qint32 len=analysisData.ullLen-2;
                                //触发偏移
                                if(len>=5){
                                    pBuffer[0]=*(pTemp);
                                    pBuffer[1]=*(pTemp+1);
                                    pBuffer[2]=*(pTemp+2);
                                    pBuffer[3]=*(pTemp+3);
                                    pBuffer[4]=*(pTemp+4);
                                    vernierTriggerPosition=*((quint64*)pBuffer);
                                }else throw errorMsg;
                                len-=5;
                                LogHelp::write(QString("    收到偏移指令:%1，数据长度:%2").arg(QString::number(vernierTriggerPosition),QString::number(analysisData.ullLen)));
                                //数据裁剪
                                if(isBuffer){
                                    QVector<quint64> countArr;
                                    LogHelp::write(QString("    数据裁剪:"));
                                    if(len>=usb->m_ChannelCount*5){
                                        pTemp+=5;
                                        for(qint32 i=0;i<usb->m_ChannelCount;i++){
                                            pBuffer[0]=*(pTemp);
                                            pBuffer[1]=*(pTemp+1);
                                            pBuffer[2]=*(pTemp+2);
                                            pBuffer[3]=*(pTemp+3);
                                            pBuffer[4]=*(pTemp+4);
                                            quint64 temp=(*((quint64*)pBuffer));
                                            countArr.append(temp);
                                            if(temp && temp*8>segment->m_SamplingDepth)
                                                segment->SetStartOffsetSampling(i,temp*8-segment->m_TriggerSamplingDepth-vernierTriggerPosition);
                                            else
                                                segment->SetStartOffsetSampling(i,0);
                                            pTemp+=5;
                                            LogHelp::write("        通道:"+QString::number(i)+" 下数据总量:"+QString::number(temp)+"Byte 计算裁剪点:"+
                                                           QString::number(segment->GetStartOffsetSampling(i))+" 下触发线点:"+QString::number(vernierTriggerPosition)+
                                                           " 上采样深度:"+QString::number(segment->m_SamplingDepth)+" 上触发点:"+
                                                           QString::number(segment->m_TriggerSamplingDepth));
                                        }
                                    }else {
                                        delete[] pBuffer;
                                        throw errorMsg;
                                    }
                                    len-=usb->m_ChannelCount*5;
                                    if(isRLE){
                                        if(len>=1){
                                            isExceedCapacity=*(pTemp)&1;
                                            isExceedBandwidth=*(pTemp)&2;
                                            if(isExceedCapacity){
                                                LogHelp::write(QString("    超容处理"));
                                                quint64 min=~0ull;
                                                for(qint32 i=0;i<countArr.count();i++){
                                                    if(countArr[i]<min&&countArr[i]!=0&&IDList->contains(i))
                                                        min=countArr[i];
                                                }
                                                LogHelp::write(QString("    最小值：")+QString::number(min)+"Byte");
                                                for(qint32 i=0;i<countArr.count();i++){
                                                    if(countArr[i]!=0&&IDList->contains(i))
                                                        segment->SetStartOffsetSampling(i,(countArr[i]-min)*8);
                                                    else
                                                        segment->SetStartOffsetSampling(i,0);
                                                    LogHelp::write("        通道:"+QString::number(i)+" 计算裁剪点:"+QString::number(segment->GetStartOffsetSampling(i)));
                                                }
                                                setRun(2);
                                            }
                                            if(isExceedBandwidth){
                                                LogHelp::write(QString("    超带宽处理"));
                                                usb->m_ThreadState=0;
                                                setRun(2);
                                            }
                                            pTemp++;
                                        }else {
                                            delete[] pBuffer;
                                            throw errorMsg;
                                        }
                                    }
                                }
                                delete[] pBuffer;
                                break;
                            }
                            case 4:
                            {
                                //答复指令
                                if(analysisData.ullLen-2>=1){
                                    quint8 order=*(analysisData.pData+2);
                                    LogHelp::write(QString("    收到应答指令:%1").arg(QString::number(order)));
                                    if(order==0x15)
                                        usb->m_ThreadState=0;
                                    else if(order==0x12){
                                        if(*(analysisData.pData+3)!=3)
                                            throw QObject::tr("参数异常，请选择正确的参数。");
                                    }
                                }
                                break;
                            }
                            case 6:
                            {
                                //完成传输指令
                                if(usb->m_ThreadState!=0){
                                    LogHelp::write("    收到结束指令");
                                    usb->m_ThreadState=0;
                                }
                                if(!isBuffer&&analysisData.ullLen-2>=1){
                                    quint8 order=*(analysisData.pData+2);
                                    if(order==1){
                                        LogHelp::write(QString("    收到Stream超容指令"));
                                        isExceedCapacity=true;
                                    }
                                }
                                break;
                            }
                            case 5:
                            {
                                //获取采集/触发进度数据
                                if(analysisData.ullLen-2>=5){
                                    quint8* pBuffer=new quint8[8];
                                    memset(pBuffer,0,8);
                                    pBuffer[0]=*(analysisData.pData+2);
                                    pBuffer[1]=*(analysisData.pData+3);
                                    pBuffer[2]=*(analysisData.pData+4);
                                    pBuffer[3]=*(analysisData.pData+5);
                                    pBuffer[4]=*(analysisData.pData+6);
                                    quint64 temp=*((quint64*)pBuffer);
                                    delete[] pBuffer;
                                    //获取采集进度数据
                                    if(isBuffer)
                                    {
                                        qint32 schedule=qMin((quint64)((temp/(qreal)segment->m_SamplingDepth)*100),100ull);
                                        if(triggerScheduleRecode!=schedule)
                                        {
                                            if(!isSetTime && temp>segment->m_TriggerSamplingDepth){
                                                isSetTime=true;
                                                segment->m_triggerDate=QDateTime::currentDateTime();
                                                segment->m_triggerDate=segment->m_triggerDate.addMSecs(-(double)segment->m_TriggerSamplingDepth/segment->GetSamplingFrequency());
                                            }
                                            emit SendDeviceRecvSchedule(schedule,temp>segment->m_TriggerSamplingDepth?2:segment->m_TriggerSamplingDepth-temp<16?1:0,1);//获取采集前触发进度数据
                                            triggerScheduleRecode=schedule;
                                        }
                                    }
                                    else
                                        emit SendDeviceRecvSchedule(1,temp==0?1:7,1);
                                }else throw errorMsg;

                                break;
                            }
                            }
                            analysisData=analysis.getNextData();
                        }
                        m_dataBuffer=analysis.getLastData();
                        if(isAddData)
                            delete[] data2->buf;
                        delete data2;
                    }
                    if (data->buf != nullptr)
                        delete[] data->buf;
                    delete data;
                    //live模式
                    if(!isBuffer)
                    {
                        quint64 minDataPosition=~0ull;
                        for(qint32 i=0;i<IDList->count();i++){
                            if(channelDataPosition[IDList->at(i)]<minDataPosition)
                                minDataPosition=channelDataPosition[IDList->at(i)];
                        }
                        if(minDataPosition>0)
                            minDataPosition--;
                        qint64 showStart=minDataPosition*8;
                        qint64 tmp=showStart>>23;
                        tmp<<=23;
                        emit decodeToPosition(tmp, segment->m_SamplingDepth);
                        showStart*=multiply;
                        showStart-=config->m_width*config->m_showConfig.m_pixelUnit;
                        if(showStart<0)
                            showStart=0;
                        emit timerDrawUpdate(m_liveFollowing?showStart:-1);
                    }
                }else
                    QThread::msleep(100);
                missCount++;
                if(missCount>20)
                {
                    LogHelp::write(QString("    应答超时"));
                    usb->m_ThreadState=2;
                }
                if(usb->m_NoDevice){
                    LogHelp::write(QString("    设备异常断开"));
                    usb->m_ThreadState=3;
                }
            }
            LogHelp::write(QString("    数据处理完成"));
        }
        if(usb->m_ThreadState>=2&&missCount>20)
        {
            setRun(2);
            m_sessionError->setError_msg(QObject::tr("设备通讯异常，请尝试重新插拔设备，或者更新固件"));
        }
    } catch (QString str) {
        usb->m_ThreadState=2;
        setRun(2);
        m_sessionError->setError_msg(str);
    }catch (exception &e) {
        setRun(2);
        m_sessionError->setError_msg(QObject::tr("采集异常，错误信息:%1").arg(e.what()));
    } catch (...) {
        setRun(2);
        m_sessionError->setError_msg(QObject::tr("线程异常，视图数据可能有误。"));
    }
    try {
        if(usb->m_ThreadState>=2||isExceedCapacity)
        {
            quint64 maxSample=~0ull;
            for(qint32 i=0;i<IDList->count();i++){
                quint64 temp=segment->GetMaxSample(IDList->at(i));
                if(temp<maxSample)
                    maxSample=temp;
            }
            if(maxSample==~0ull || maxSample<1000*multiply)
                maxSample=1000*multiply;
            else
                maxSample--;
            config->m_maxUnit=maxSample*multiply;
            if(!isBuffer){
                if(segment->m_enableGlitchRemoval)
                    emit decodeToPosition(-1, -1);
                else
                    emit decodeToPosition(maxSample, maxSample);
            }

        }else if(!isBuffer){
            if(segment->m_enableGlitchRemoval)
                emit decodeToPosition(-1, -1);
            else
                emit decodeToPosition(segment->m_SamplingDepth, segment->m_SamplingDepth);
        }

        LogHelp::write(QString("    等待停止读取线程"));
        m_usbReadThread.stop();
        usb->Stop();
        while(m_usbReadThread.isRun())
            QApplication::processEvents(QEventLoop::AllEvents, 100);
        LogHelp::write(QString("    取得数据量(Byte):"));
        for(quint32 i=0;i<dataCount.count();i++){
            LogHelp::write(QString("        通道:%1 数据量:%2").arg(QString::number(i),QString::number(dataCount[i])));
        }

        //删除缓冲区
        while(!m_dataList.isEmpty()){
            Data_* data2=m_dataList.first();
            m_dataList.pop_front();
            if (data2->buf != nullptr)
                delete[] data2->buf;
            delete data2;
        }
        //清空MCU
        Data_* data = new Data_();
        qint32 count_=0;
        while(usb->ReadSynchronous(data)){//启动前清空mcu缓存
            delete[] data->buf;
            count_++;
            if(count_>1000)
                break;
        }
        delete data;
        for(qint32 i=0;i<segment->GetChannelNum();i++)
            segment->m_isFirst[i]=true;

        if(isRLE){
            for(qint32 i=0;i<IDList->count();i++){
                QVector<Segment::_Node>* dataList=segment->GetChannelDataList(IDList->at(i));
                qint8 offset=segment->GetStartOffsetSampling(IDList->at(i));
                qint32 blockCount=segment->m_BLOCK_MAX_SIZE/8-1;
                if(offset>0&&offset<64){
                    for(qint32 ii=0;ii<dataList->count();ii++){
                        for(quint32 j=0;j<segment->m_NODE_DATA_SIZE;j++){
                            if(dataList->at(ii).lbp[j]){
                                quint64* ptr=(quint64*)dataList->at(ii).lbp[j];
                                for(qint32 k=0;k<blockCount;k++){
                                    *ptr>>=offset;
                                    *ptr|=*(ptr+1)<<(64-offset);
                                    ptr++;
                                }
                                if(j+1<segment->m_NODE_DATA_SIZE&&dataList->at(ii).lbp[j+1]){
                                    quint64* ptr2=(quint64*)dataList->at(ii).lbp[j+1];
                                    *ptr>>=offset;
                                    *ptr|=*(ptr2+1)<<(64-offset);
                                }else if(j+1==segment->m_NODE_DATA_SIZE){
                                    if(ii+1<dataList->count()&&dataList->at(ii+1).lbp[0]){
                                        quint64* ptr2=(quint64*)dataList->at(ii+1).lbp[0];
                                        *ptr>>=offset;
                                        *ptr|=*(ptr2+1)<<(64-offset);
                                    }
                                }
                            }
                            segment->CheckBlockCompress(i,ii,j);
                        }
                    }
                    segment->SetStartOffsetSampling(i,0);
                }else{
                    for(qint32 ii=0;ii<dataList->count();ii++)
                        for(quint32 j=0;j<segment->m_NODE_DATA_SIZE;j++)
                            segment->CheckBlockCompress(i,ii,j);
                }
            }
        }
    } catch (...) {
        setRun(2);
        m_sessionError->setError_msg(QObject::tr("线程异常，视图数据可能有误。"));
    }
    if(isBuffer)
        emit sendVernierTriggerPosition(segment->m_TriggerSamplingDepth*multiply);
    else if(vernierTriggerPosition>0)
        emit sendVernierTriggerPosition(vernierTriggerPosition*multiply);
    if(m_dataBuffer!=nullptr&&m_dataBuffer->len>0)
    {
        delete[] m_dataBuffer->buf;
        delete m_dataBuffer;
        m_dataBuffer=nullptr;
    }
    delete[] rleBuffer;
    delete[] offsetBuffer;
    delete IDList;
    segment->lessenCite();
    emit SendDeviceRecvSchedule(100,3,usb->m_ThreadState==0&&isRun()?1:0);

    LogHelp::write(QString("    采集任务线程退出"));
    if(isExceedBandwidth)
        m_sessionError->setError_msg(QObject::tr("采集的数据超出硬件带宽上限，数据异常。"));
    else if(isExceedCapacity)
        m_sessionError->setError_msg(QObject::tr("采集的数据超出硬件容量上限，仅显示最后可用数据。如无可用数据，请调整参数再次采集。"));
    else if(usb->m_ThreadState==3)
        m_sessionError->setError_msg(QObject::tr("设备异常断开，无法继续采集。"));
    usb->m_busy=false;
    usb->m_ThreadState=0;
    setRun(0);
}

void ThreadWork::SaveFileThreadPara(QString filePath, Segment *segment, QVector<QString> channelsName)
{
    SharedMemoryHelper* shared=&DataService::getInstance()->m_shared;
    shared->increase(SAVE_FILE);
    LogHelp::write(QString("    保存文件线程启动"));
    setRun(1);
    segment->appendCite();
    bool isError=false;
    QString path=creanTimestampDir(DataService::getInstance()->m_tempDir+"/temp/save/",true);
    QFile* file=new QFile(path);
    try{
        emit SendDeviceRecvSchedule(0,4,1);
        {
            if(!file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
            {
                isError=true;
                m_sessionError->setError_msg(QObject::tr("创建文件失败。"));
                goto end;
            }
            QTextStream out(file);
#ifdef Q_OS_WIN
            out.setCodec(QTextCodec::codecForName("GBK"));
#else
            out.setCodec(QTextCodec::codecForName("UTF-8"));
#endif
            QVector<int> outChannels;
            QVector<quint64> outChannelsPosition;
            QVector<bool> outChannelsComplete;
            QVector<qint8> channelsData;
            qint32 channelsNum=0;
            QDateTime time=segment->m_triggerDate, time2;
            for(qint32 i=0;i<segment->isData.count();i++){
                if(segment->isData.at(i))
                {
                    outChannels.append(i);
                    DataEnd temp=segment->GetDataEnd(0,i,false);
                    outChannelsPosition.append(temp.position);
                    outChannelsComplete.append(temp.isEnd);
                    channelsNum|=1<<i;
                }
            }
            out<<"; CSV, generated by atk_libsigrok4 1.0.0 on "<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")<<"\n";
            out<<"; Channels ("<<outChannels.count()<<"/"<<segment->isData.count()<<")\n";
            out<<"; ChannelsType: "<<QString::number(channelsNum)<<"\n";
            out<<"; Sample rate: "<<hzToShowStr(segment->GetSamplingFrequency()*1000)<<"\n";
            out<<"; Sample count: "<<bitToShowStr(segment->m_SamplingDepth)<<"\n";
            out<<"; Sampling time: "<<segment->m_collectDate.toString("yyyy-MM-dd hh:mm:ss.zzz")<<"\n";
            out<<"SystemTime, Time(s)";
            for(auto i : outChannels){
                out<<", "<<channelsName[i];
            }
            out<<"\n";
            out<<"'"<<time.toString("yyyy-MM-dd hh:mm:ss.zzz,")<<"0";
            for(qint32 i=0;i<outChannels.count();i++){
                qint8 temp=segment->GetSample(0,outChannels[i]);
                out<<","<<temp;
                channelsData.append(temp);
            }
            out<<"\n";
            qint32 multiply=segment->GetMultiply();
            quint64 start=0,end=segment->m_SamplingDepth;
            quint64 currentMax,currentMaxRecode=0;
            qint32 schedule=0;
            QVector<int> currentMaxIndex;
            quint64 maxSample=1000,t;
            for(qint32 i=0;i<segment->GetChannelNum();i++){
                quint64 temp=segment->GetMaxSample(i);
                if(temp>maxSample)
                    maxSample=temp;
            }
            if(maxSample<end)
                end=maxSample;
            while(start<=end&&isRun()){
                currentMax=-1ull;
                currentMaxIndex.clear();
                for(qint32 i=0;i<outChannels.count();i++){
                    if(!outChannelsComplete[i]){
                        if(outChannelsPosition[i]>end){
                            outChannelsComplete[i]=true;
                            outChannelsPosition[i]=end;
                        }
                        if(outChannelsPosition[i]<currentMax)
                        {
                            currentMax=outChannelsPosition[i];
                            currentMaxIndex.clear();
                            currentMaxIndex.append(i);
                        }else if(outChannelsPosition[i]==currentMax)
                            currentMaxIndex.append(i);
                    }
                }
                if(currentMax==currentMaxRecode || currentMax==-1ULL)
                    break;
                currentMaxRecode=currentMax;
                for(qint32 i=0;i<currentMaxIndex.count();i++){
                    channelsData[currentMaxIndex[i]]=segment->GetSample(currentMax,outChannels[currentMaxIndex[i]]);
                    DataEnd temp=segment->GetDataEnd(outChannelsPosition[currentMaxIndex[i]],outChannels[currentMaxIndex[i]],false);
                    outChannelsPosition[currentMaxIndex[i]]=temp.position;
                    outChannelsComplete[currentMaxIndex[i]]=temp.isEnd;
                }
                t=currentMax*multiply;
                time2=time.addMSecs(t/1000000.);
                out<<"'"<<time2.toString("yyyy-MM-dd hh:mm:ss.zzz")<<QString("%1").arg(t%1000000, 6, 10, QLatin1Char('0'))<<",";
                out<<moveDecimal(t,9);
                for(qint32 i=0;i<outChannels.count();i++)
                    out<<","<<channelsData[i];
                out<<"\n";
                start=currentMax;
                qint32 scheduleTemp=qMin((qint32)(currentMax/(qreal)end*100),99);
                if(schedule!=scheduleTemp)
                {
                    schedule=scheduleTemp;
                    emit SendDeviceRecvSchedule(schedule,4,1);
                }
            }
            out.flush();
            if(!isRun())
            {
                m_sessionError->setError_msg(QObject::tr("导出线程已停止。"));
                isError=true;
            }
        }
    } catch (exception &e) {
        m_sessionError->setError_msg(QObject::tr("保存异常，错误信息:%1").arg(e.what()));
        isError=true;
    } catch (...) {
        m_sessionError->setError_msg(QObject::tr("保存失败，文件数据可能有误。"));
        isError=true;
    }
end:
    try{
        file->flush();
        file->close();
        if(isError)
            file->remove();
        else{
            {
                isError=false;
                QFile fileTarget(filePath);
                if(fileTarget.exists()){
                    if(!fileTarget.remove()){
                        isError=true;
                        m_sessionError->setError_msg(QObject::tr("文件被占用，无法删除文件。"));
                    }
                }
            }
            if(!isError){
                if(!file->rename(filePath))
                    m_sessionError->setError_msg(QObject::tr("文件被占用，无法覆盖文件。"));
            }
        }
        delete file;
    } catch(...){
    }
    LogHelp::write(QString("    保存文件线程退出"));
    segment->lessenCite();
    emit SendDeviceRecvSchedule(100,4,isRun()&&!isError?1:0);
    setRun(0);
    shared->decrement(SAVE_FILE);
}

void ThreadWork::SaveSourceFileThreadPara(QString filePath, QString sessionName, Segment *segment,
                                          QVector<QString> channelsName, QVector<MeasureData>* measureList, QVector<VernierData>* vernierList,
                                          QHash<QString,DecodeController*>* decodes)
{
    SharedMemoryHelper* shared=&DataService::getInstance()->m_shared;
    shared->increase(SAVE_FILE);
    LogHelp::write(QString("    保存工程文件线程启动"));
    ZipHelper zip;
    connect(&zip,&ZipHelper::sendZipDirSchedule,this,&ThreadWork::sendZipDirSchedule);
    setRun(1);
    segment->appendCite();
    QString path=creanTimestampDir(DataService::getInstance()->m_tempDir+"/temp/save/");
    try {
        emit SendDeviceRecvSchedule(0,4,1);
        emit saveSessionSettings(path);
        QFile setFile(path+"/channel.ini");
        if(!setFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
            throw QObject::tr("生成通道配置文件失败。");
        QTextStream setStream(&setFile);
        setStream.setCodec(QTextCodec::codecForName("UTF-8"));
        setStream<<"SessionName="<<sessionName<<"\n";
        //判断是否有毛刺过滤
        if(segment->m_enableGlitchRemoval){
            setStream<<"GlitchRemoval=";
            for(qint32 i=0;i<segment->GetChannelNum();i++){
                if(i!=0)
                    setStream<<",";
                setStream<<segment->GetChannelGlitchRemoval(i);
            }
            setStream<<"\n";
        }
        //保存常用参数
        setStream<<"SamplingFrequency="<<QString::number(segment->GetSamplingFrequency())<<"\n";
        setStream<<"SamplingDepth="<<QString::number(segment->m_SamplingDepth)<<"\n";
        setStream<<"TriggerSamplingDepth="<<QString::number(segment->m_TriggerSamplingDepth)<<"\n";
        //保存测量功能
        if(measureList->count()){
            QFile writeFile(path+"/measure.ini");
            if(!writeFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
                throw QObject::tr("生成测量配置文件失败。");
            QTextStream writeStream(&writeFile);
            writeStream.setCodec(QTextCodec::codecForName("UTF-8"));
            for(qint32 i=0;i<measureList->count();i++){
                auto tmp=measureList->at(i);
                writeStream<<tmp.name<<","<<QString::number(tmp.channelID)<<","<<
                             QString::number(tmp.start)<<","<<QString::number(tmp.end)<<","<<
                             QString::number(qRgb(tmp.dataColor.red(), tmp.dataColor.green(), tmp.dataColor.blue()),16)<<","<<
                             tmp.note<<"\n";
            }
            writeStream.flush();
            writeFile.flush();
            writeFile.close();
        }
        //保存游标功能
        if(vernierList->count()){
            QFile writeFile(path+"/vernier.ini");
            if(!writeFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
                throw QObject::tr("生成测量配置文件失败。");
            QTextStream writeStream(&writeFile);
            writeStream.setCodec(QTextCodec::codecForName("UTF-8"));
            for(qint32 i=0;i<vernierList->count();i++){
                auto tmp=vernierList->at(i);
                writeStream<<tmp.name<<","<<QString::number(qRgb(tmp.dataColor.red(), tmp.dataColor.green(), tmp.dataColor.blue()),16)<<","<<
                             QString::number(tmp.position)<<","<<tmp.note<<"\n";
            }
            writeStream.flush();
            writeFile.flush();
            writeFile.close();
        }
        //        //保存协议信息
        //        {
        //            QHash<QString,DecodeController*>::const_iterator it = decodes->constBegin();
        //            setStream<<"Decodes=";
        //            bool isFirstDecode=true;
        //            while (it != decodes->constEnd()) {
        //                QJsonDocument doc(it.value()->getJsonObject());
        //                if(isFirstDecode)
        //                    isFirstDecode=false;
        //                else
        //                    setStream<<",";
        //                setStream<<it.key();
        //                QFile writeFile(path+"/"+it.key());
        //                if(!writeFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        //                    throw QObject::tr("生成协议数据文件失败。");
        //                QTextStream writeStream(&writeFile);
        //                writeStream.setCodec(QTextCodec::codecForName("UTF-8"));
        //                writeStream<<QString(doc.toJson(QJsonDocument::Indented));
        //                writeStream.flush();
        //                writeFile.flush();
        //                writeFile.close();
        //                ++it;
        //            }
        //            setStream<<"\n";
        //        }
        //保存通道数据
        qreal channelSchedule=99/segment->GetChannelNum();
        for(qint32 i=0;i<segment->GetChannelNum();i++){
            QDir savePath(path+"/"+QString::number(i));
            if(!savePath.exists())
                savePath.mkpath(path+"/"+QString::number(i));
            QVector<Segment::_Node>* dataList=segment->GetChannelDataList(i);
            QFile channelSetFile(path+"/"+QString::number(i)+"/"+"channel.ini");
            if(!channelSetFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
                throw QObject::tr("生成通道%1配置文件失败。").arg(QString::number(i));
            QTextStream channelSetStream(&channelSetFile);
            channelSetStream.setCodec(QTextCodec::codecForName("UTF-8"));
            qreal channelSchedule2=channelSchedule/dataList->count();
            //写出通道名
            channelSetStream<<channelsName[i]<<"\n";
            //写出数据偏移
            channelSetStream<<QString::number(segment->GetStartOffsetSampling(i))<<"\n";
            //写出最大长度
            channelSetStream<<QString::number(segment->GetMaxSample(i,false))<<"\n";

            //写出tog和value
            for(qint32 ii=0;ii<dataList->count();ii++){
                channelSetStream<<dataList->at(ii).tog<<","<<dataList->at(ii).value<<"\n";
                for(quint32 j=0;j<segment->m_NODE_DATA_SIZE;j++){
                    if(dataList->at(ii).lbp[j]!=nullptr){
                        QFile file(path+"/"+QString::number(i)+"/"+QString::number(ii)+"-"+QString::number(j)+".bin");
                        if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
                            throw QObject::tr("生成原始文件失败。");
                        QDataStream out(&file);
                        out.writeRawData((const char*)dataList->at(ii).lbp[j], segment->m_BLOCK_MAX_SIZE);
                        file.flush();
                        file.close();
                    }
                }
                if(!isRun())
                    break;
                emit SendDeviceRecvSchedule(qMin(i*channelSchedule+ii*channelSchedule2,99.),4,1);
            }
            channelSetStream.flush();
            channelSetFile.flush();
            channelSetFile.close();
            if(!isRun())
                break;
        }
        setStream.flush();
        setFile.flush();
        setFile.close();
        emit SendDeviceRecvSchedule(99,4,1);
        QFile set(path+"/set.ini");
        while(!set.exists()&&isRun())
            QApplication::processEvents(QEventLoop::AllEvents, 100);
        if(isRun())
        {
            if(!zip.zipDir(path,path+".atkdl"))
                throw QObject::tr("生成压缩文件失败。");
            else{
                bool isError=false;
                QFile fileTarget(filePath);
                if(fileTarget.exists()){
                    if(!fileTarget.remove()){
                        isError=true;
                        m_sessionError->setError_msg(QObject::tr("文件被占用，无法删除文件。"));
                    }
                }
                if(!isError){
                    if(!QFile::rename(path+".atkdl",filePath))
                        m_sessionError->setError_msg(QObject::tr("文件被占用，无法覆盖文件。"));
                }
            }
            segment->m_isSave=true;
        }else
            m_sessionError->setError_msg(QObject::tr("保存进程已停止。"));
    } catch (QString str) {
        m_sessionError->setError_msg(QObject::tr("保存工程异常，错误信息:%1").arg(str));
    } catch (exception &e) {
        m_sessionError->setError_msg(QObject::tr("保存工程异常，错误信息:%1").arg(e.what()));
    } catch (...) {
        m_sessionError->setError_msg(QObject::tr("保存工程失败。"));
    }
    segment->lessenCite();
    emit SendDeviceRecvSchedule(100,4,isRun()?1:0);
    LogHelp::write(QString("    保存工程文件线程结束"));
    setRun(0);
    try {
        QDir p(path);
        if(p.exists()){
            p.removeRecursively();
            p.remove(path);
        }
    } catch (...) {
    }
    shared->decrement(SAVE_FILE);
}

void ThreadWork::LoadSourceFileThreadPara(QString filePath, Segment *segment, SessionConfig *config,
                                          QVector<MeasureData>* measureList, QVector<VernierData>* vernierList,
                                          QHash<QString,DecodeController*>* decodes, QVector<QString>* channelsNames, bool isSendDecode)
{
    SharedMemoryHelper* shared=&DataService::getInstance()->m_shared;
    shared->increase(LOAD_FILE);
    LogHelp::write(QString("    加载工程文件线程启动"));
    QString glitchString="";
    ZipHelper zip;
    connect(&zip,&ZipHelper::sendUnZipDirSchedule,this,&ThreadWork::sendUnZipDirSchedule);
    setRun(1);
    segment->appendCite();
    try {
        emit SendDeviceRecvSchedule(0,5,1);
        QString path=creanTimestampDir(DataService::getInstance()->m_tempDir+"/temp/load/");
        if(!zip.unZipDir(filePath,path))
            throw QObject::tr("解压缩文件失败。");
        QFile setFile(path+"/channel.ini");
        if(setFile.exists())
        {
            if(!setFile.open(QIODevice::ReadOnly | QIODevice::Text))
                throw QObject::tr("读取通道配置文件失败。");
            QTextStream setStream(&setFile);
            setStream.setCodec(QTextCodec::codecForName("UTF-8"));
            do{
                QStringList strArr=setStream.readLine().split("=");
                if(strArr.count()==2){
                    if(strArr[0]=="SessionName")
                        emit loadSessionSettings(strArr[1], path);
                    else if(strArr[0]=="GlitchRemoval" && strArr[1].contains(","))
                        glitchString=strArr[1];
                    else if(strArr[0]=="Decodes"){
                        //读取协议解码
                        QStringList decodes=strArr[1].split(",");
                        for(qint32 i=0;i<decodes.count();i++){
                            if(!decodes[i].isEmpty()){
                                QFile readFile(path+"/"+decodes[i]);
                                if(readFile.exists()){
                                    if(!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
                                        throw QObject::tr("读取协议文件失败。");
                                    QTextStream readStream(&readFile);
                                    readStream.setCodec(QTextCodec::codecForName("UTF-8"));
                                    emit appendDecode(readStream.readAll(),false);
                                    readFile.close();
                                }
                            }
                        }
                    }
                    else if(strArr[0]=="SamplingFrequency")
                        segment->SetSamplingFrequency(strArr[1].toULongLong());
                    else if(strArr[0]=="SamplingDepth")
                        segment->m_SamplingDepth=strArr[1].toULongLong();
                    else if(strArr[0]=="TriggerSamplingDepth")
                        segment->m_TriggerSamplingDepth=strArr[1].toULongLong();
                }
            }while(!setStream.atEnd());
            //读取测量功能
            {
                QFile readFile(path+"/measure.ini");
                if(readFile.exists()){
                    if(!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
                        throw QObject::tr("读取测量配置文件失败。");
                    QTextStream readStream(&readFile);
                    readStream.setCodec(QTextCodec::codecForName("UTF-8"));
                    do{
                        QStringList strArr=readStream.readLine().split(",");
                        if(strArr.length()==6){
                            MeasureData data;
                            data.name=strArr[0];
                            data.channelID=strArr[1].toInt();
                            data.start=strArr[2].toLongLong();
                            data.end=strArr[3].toLongLong();
                            data.dataColor=QColor("#"+strArr[4]);
                            if(!strArr[5].isEmpty())
                                data.note=strArr[5];
                            measureList->append(data);
                            emit appendMeasure(measureList->count()-1);
                        }
                    }while(!readStream.atEnd());
                    readFile.close();
                }
            }
            //读取游标功能
            {
                QFile readFile(path+"/vernier.ini");
                if(readFile.exists()){
                    if(!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
                        throw QObject::tr("读取测量配置文件失败。");
                    QTextStream readStream(&readFile);
                    readStream.setCodec(QTextCodec::codecForName("UTF-8"));
                    bool isFirst=true;
                    do{
                        QStringList strArr=readStream.readLine().split(",");
                        if(strArr.length()==4){
                            VernierData data;
                            if(isFirst){
                                isFirst=false;
                                emit appendVernier(-1);
                                emit sendVernierTriggerPosition(strArr[2].toLongLong());
                            }else{
                                data.name=strArr[0];
                                data.dataColor=QColor("#"+strArr[1]);
                                data.position=strArr[2].toLongLong();
                                if(!strArr[3].isEmpty())
                                    data.note=strArr[3];
                                vernierList->append(data);
                                emit appendVernier(vernierList->count()-1);
                            }
                        }
                    }while(!readStream.atEnd());
                    readFile.close();
                }
            }
            //读取通道数据
            qreal channelSchedule=99/segment->GetChannelNum();
            for(qint32 i=0;i<segment->GetChannelNum();i++){
                QDir savePath(path+"/"+QString::number(i));
                if(!savePath.exists())
                {
                    throw QObject::tr("缺失通道%1数据目录，已经跳过。\n").arg(QString::number(i));
                    continue;
                }
                QVector<Segment::_Node>* dataList=segment->GetChannelDataList(i);
                QFile channelSetFile(path+"/"+QString::number(i)+"/"+"channel.ini");
                if(!channelSetFile.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    throw QObject::tr("读取通道%1配置文件失败，已经跳过。\n").arg(QString::number(i));
                    continue;
                }
                QTextStream channelSetStream(&channelSetFile);
                channelSetStream.setCodec(QTextCodec::codecForName("UTF-8"));
                {
                    //读取通道名
                    (*channelsNames)[i]=channelSetStream.readLine();
                    //读取数据偏移
                    segment->SetStartOffsetSampling(i, channelSetStream.readLine().toULongLong());
                    //读取最大长度
                    segment->SetMaxSample(channelSetStream.readLine().toULongLong(), i, false);
                }
                {
                    //遍历数据文件
                    QDir tmp(path+"/"+QString::number(i)+"/");
                    QStringList filters;
                    filters << "*.bin";
                    tmp.setNameFilters(filters);
                    tmp.setFilter(QDir::Files); //设置过滤器
                    QFileInfoList fileInfoList=tmp.entryInfoList();
                    qreal channelSchedule2=channelSchedule/fileInfoList.count();
                    for(int ii=0;ii<fileInfoList.count();ii++){
                        QFile file(fileInfoList[ii].filePath());
                        if(!isRun())
                            break;
                        if(!file.open(QIODevice::ReadOnly))
                            throw QObject::tr("读取原始文件失败。");
                        QDataStream in(&file);
                        //判断是否没有申请内存
                        QStringList list= fileInfoList[ii].fileName().replace(".bin","").split("-");
                        if(list.size()!=2)
                            continue;
                        int iii=list[0].toInt(), j=list[1].toInt();
                        if ((*dataList)[iii].lbp[j] == NULL)
                        {
                            (*dataList)[iii].lbp[j] = malloc_(segment->m_BLOCK_MAX_SIZE);
                            if ((*dataList)[iii].lbp[j] == NULL)
                                throw QObject::tr("申请内存失败，无法写入数据。");
                            memset((*dataList)[iii].lbp[j], 0, segment->m_BLOCK_MAX_SIZE);
                            in.readRawData((char *)(*dataList)[iii].lbp[j], qMin(file.size(), (qint64)segment->m_BLOCK_MAX_SIZE));
                            segment->isData[i]=true;
                        }
                        file.close();
                        emit SendDeviceRecvSchedule(qMin(i*channelSchedule+ii*channelSchedule2,99.),5,1);
                    }

                    //读取tog和value
                    qint32 ii=0;
                    bool isCheckBlockCompress;
                    do{
                        QString str=channelSetStream.readLine();
                        if(str.contains(","))
                        {
                            isCheckBlockCompress=false;
                            QStringList ls=str.split(",");
                            if(ls.count()==2)
                            {
                                (*dataList)[ii].tog=ls[0].toULongLong();
                                (*dataList)[ii].value=ls[1].toULongLong();
                                if(dataList->at(ii).tog)
                                    segment->isData[i]=true;
                                if((*dataList)[ii].tog!=0)
                                    isCheckBlockCompress=true;
                            }
                            for(quint32 j=0;j<segment->m_NODE_DATA_SIZE;j++)
                                if(isCheckBlockCompress || (*dataList)[ii].lbp[j] != NULL)
                                    segment->CheckBlockCompress(i,ii,j);
                            ii++;
                        }
                    }while(!channelSetStream.atEnd() && isRun());
                }
                channelSetFile.close();
                if(!isRun())
                    break;
            }
            //判断最大显示位置
            quint64 maxSample=~0ull;
            for(qint32 i=0;i<segment->GetChannelNum();i++){
                quint64 temp=segment->GetMaxSample(i);
                if(temp<maxSample&&temp!=0)
                    maxSample=temp;
            }
            qint32 multiply=segment->GetMultiply();
            if(maxSample<segment->m_SamplingDepth){
                if(maxSample==~0ull || maxSample<1000*multiply)
                    maxSample=1000*multiply;
                else
                    maxSample--;
                config->m_maxUnit=maxSample*multiply;
            }
            else
                config->m_maxUnit=segment->m_SamplingDepth*multiply;
            for(qint32 i=0;i<segment->m_isFirst.count();i++)
                segment->m_isFirst[i]=true;
            emit SendDeviceRecvSchedule(99,5,1);

        }else
            throw QObject::tr("通道配置文件缺失。");

    } catch (QString str) {
        setRun(2);
        m_sessionError->setError_msg(QObject::tr("加载工程异常，错误信息:%1").arg(str));
    } catch (exception &e) {
        setRun(2);
        m_sessionError->setError_msg(QObject::tr("加载工程异常，错误信息:%1").arg(e.what()));
    } catch (...) {
        setRun(2);
        m_sessionError->setError_msg(QObject::tr("加载工程失败。"));
    }
    segment->lessenCite();
    emit SendDeviceRecvSchedule(100,5,isRun()?1:0);
    LogHelp::write(QString("    加载工程文件线程结束"));
    setRun(0);
    if(!glitchString.isEmpty())
        emit sendGlitchString(glitchString);
    else{
        if(isSendDecode)
            emit appendDecode("",true);
        emit sendGlitchString("");
    }
    shared->decrement(LOAD_FILE);
}

void ThreadWork::SaveBinFileThreadPara(QString filePath, Segment *segment, qint32 type, QString channels)
{
    SharedMemoryHelper* shared=&DataService::getInstance()->m_shared;
    shared->increase(SAVE_FILE);
    LogHelp::write(QString("    导出原始文件线程启动"));
    setRun(1);
    segment->appendCite();
    bool isError=false;
    QString path=creanTimestampDir(DataService::getInstance()->m_tempDir+"/temp/save/",true);
    QFile* file=new QFile(path);
    QList<quint8*> buff;
    buff.append(new quint8[segment->m_BLOCK_MAX_SIZE]);
    buff.append(new quint8[segment->m_BLOCK_MAX_SIZE]);
    memset(buff[0], 0, segment->m_BLOCK_MAX_SIZE);
    memset(buff[1], 0xff, segment->m_BLOCK_MAX_SIZE);
    try{
        emit SendDeviceRecvSchedule(0,4,1);
        {
            if(!file->open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
                isError=true;
                m_sessionError->setError_msg(QObject::tr("创建文件失败。"));
                goto end;
            }
            QDataStream out(file);
            QList<quint8> channelList;
            QList<QString> strs=channels.split(",");
            for(auto &i : strs){
                bool isOK=false;
                channelList.append(i.toInt(&isOK));
                if(!isOK || channelList[channelList.count()-1]>15){
                    isError=true;
                    m_sessionError->setError_msg(QObject::tr("参数异常。"));
                    goto end;
                }
            }

            qreal channelSchedule=99/channelList.count();
            for(auto &i:channelList){
                QVector<Segment::_Node>* dataList=segment->GetChannelDataList(i);
                if(dataList!=nullptr){
                    qreal channelSchedule2=channelSchedule/dataList->count();
                    qint32 offset=segment->GetStartOffsetSampling(i)>>3;
                    qint64 countSample=0,maxSample=segment->GetMaxSample(i);
                    bool isEnd=false;
                    for(qint32 ii=0;ii<dataList->count();ii++){
                        for(quint32 j=0;j<segment->m_NODE_DATA_SIZE;j++){
                            quint8* data=nullptr;
                            qint32 len=0;
                            if(dataList->at(ii).lbp[j]){
                                data=(quint8*)dataList->at(ii).lbp[j];
                                len=segment->m_BLOCK_MAX_SIZE;
                            }else{
                                data=buff[(dataList->at(ii).value&1<<j)?1:0];
                                len=segment->m_BLOCK_MAX_SIZE;
                            }

                            if(offset>0){
                                if(offset>len){
                                    offset-=len;
                                    len=0;
                                }else{
                                    data+=offset;
                                    len-=offset;
                                    offset=0;
                                }
                            }

                            if(countSample+(len<<3)>maxSample){
                                len=(maxSample-countSample)>>3;
                                if(((maxSample-countSample)&7)>0)
                                    len++;
                                isEnd=true;
                            }

                            if(len>0){
                                out.writeRawData((const char*)data, len);
                                countSample+=len<<3;
                            }

                            if(isEnd || !isRun())
                                break;
                        }
                        if(isEnd || !isRun())
                            break;
                        emit SendDeviceRecvSchedule(qMin(i*channelSchedule+ii*channelSchedule2,99.),4,1);
                    }
                }
                if(!isRun())
                    break;
            }
            if(!isRun())
            {
                m_sessionError->setError_msg(QObject::tr("导出线程已停止。"));
                isError=true;
            }
        }
    } catch (exception &e) {
        m_sessionError->setError_msg(QObject::tr("保存异常，错误信息:%1").arg(e.what()));
        isError=true;
    } catch (...) {
        m_sessionError->setError_msg(QObject::tr("保存失败，文件数据可能有误。"));
        isError=true;
    }
end:
    try{
        file->flush();
        file->close();
        if(isError)
            file->remove();
        else{
            {
                isError=false;
                QFile fileTarget(filePath);
                if(fileTarget.exists()){
                    if(!fileTarget.remove()){
                        isError=true;
                        m_sessionError->setError_msg(QObject::tr("文件被占用，无法删除文件。"));
                    }
                }
            }
            if(!isError){
                if(!file->rename(filePath))
                    m_sessionError->setError_msg(QObject::tr("文件被占用，无法覆盖文件。"));
            }
        }
        delete file;
    } catch(...){
    }
    for(auto i : buff)
        delete[] i;
    LogHelp::write(QString("    导出原始文件线程退出"));
    segment->lessenCite();
    emit SendDeviceRecvSchedule(100,4,isRun()&&!isError?1:0);
    setRun(0);
    shared->decrement(SAVE_FILE);
}

void ThreadWork::GlitchRemovalThreadPara(Segment *segment)
{
    LogHelp::write(QString("    计算毛刺过滤线程启动"));
    setRun(1);
    segment->appendCite();
    try{
        emit SendDeviceRecvSchedule(0,6,1);
        qint8 channelCount=0;
        for(qint32 i=0;i<segment->GetChannelNum();i++){
            qint32 glitch=segment->GetChannelGlitchRemoval(i);
            if(glitch>0)
                channelCount++;
        }
        if(channelCount<=0){
            m_sessionError->setError_msg(QObject::tr("设置失败，设置通道数错误。"));
            goto end;
        }
        qreal channelSchedule=99/channelCount;
        qint32 scheduleRecode=0;
        for(qint32 i=0;i<segment->GetChannelNum();i++){
            emit SendDeviceRecvSchedule(qMin(i*channelSchedule,99.),6,1);
            qint32 glitch=segment->GetChannelGlitchRemoval(i);
            if(glitch>0){
                quint64 maxSample=segment->GetMaxSample(i),startSample=0,lastStartSample=0;
                while(startSample<maxSample){
                    DataEnd next=segment->GetDataEnd(startSample,i,false);
                    qint64 len=next.position-startSample;
                    if(len<=0)
                        break;
                    if(len<=glitch){
                        segment->SetSampleBlock(startSample,next.position,!segment->GetSample(startSample,i),i,true);
                        if(next.position-lastStartSample>glitch){
                            lastStartSample=next.position-glitch-1;
                            startSample=lastStartSample;
                        }
                    }
                    else{
                        lastStartSample=startSample;
                        startSample=next.position;
                    }
                    if(!isRun()||next.isEnd)
                        break;

                    qint32 schedule=qMin(i*channelSchedule+((qreal)startSample/maxSample*channelSchedule*0.9),99.);
                    if(scheduleRecode!=schedule)
                    {
                        scheduleRecode=schedule;
                        emit SendDeviceRecvSchedule(schedule,6,1);
                    }
                }
                if(!isRun()){
                    QVector<Segment::_Node>* dataList=segment->GetChannelDataList(i);
                    qint32 count=dataList->count();
                    double scheduleLast=channelSchedule*0.9;
                    for(qint32 ii=0;ii<count;ii++){
                        for(quint32 j=0;j<segment->m_NODE_DATA_SIZE;j++)
                            segment->CheckBlockCompress(i,ii,j);
                        if(!isRun())
                            break;
                        qint32 schedule=qMin(i*channelSchedule+((qreal)ii/count*channelSchedule*0.1+scheduleLast),99.);
                        if(scheduleRecode!=schedule)
                        {
                            scheduleRecode=schedule;
                            emit SendDeviceRecvSchedule(schedule,6,1);
                        }
                    }
                }
            }
            if(!isRun())
                break;
        }
        for(qint32 i=0;i<segment->m_isFirst.count();i++)
            segment->m_isFirst[i]=true;
    } catch (exception &e) {
        setRun(2);
        m_sessionError->setError_msg(QObject::tr("设置失败，错误信息:%1").arg(e.what()));
    } catch (...) {
        setRun(2);
        m_sessionError->setError_msg(QObject::tr("设置失败，视图数据可能有误。"));
    }
end:
    segment->lessenCite();
    emit SendDeviceRecvSchedule(100,6,isRun()?1:0);
    LogHelp::write(QString("    计算毛刺过滤线程退出"));
    setRun(0);
}

void ThreadWork::SaveDecodeTableThreadPara(QString filePath, qint32 multiply, QList<DecodeRowData> *data,QList<QMap<QString,QJsonObject>>* rowType, QVector<bool>* saveList)
{
    SharedMemoryHelper* shared=&DataService::getInstance()->m_shared;
    shared->increase(SAVE_FILE);
    LogHelp::write(QString("    保存协议表格线程启动"));
    setRun(1);
    bool isError=false;
    QString path=creanTimestampDir(DataService::getInstance()->m_tempDir+"/temp/save/",true);
    QFile* file=new QFile(path);
    try{
        emit SendDeviceRecvSchedule(0,4,1);
        {
            if(!file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
            {
                isError=true;
                m_sessionError->setError_msg(QObject::tr("创建文件失败。"));
                goto end;
            }
            QTextStream out(file);
#ifdef Q_OS_WIN
            out.setCodec(QTextCodec::codecForName("GBK"));
#else
            out.setCodec(QTextCodec::codecForName("UTF-8"));
#endif
            qint32 dataCount=data->count();
            qint32 columnCount=saveList->count();
            qint32 scheduleCount=qFloor(dataCount/100.);
            if(scheduleCount<1)
                scheduleCount=1;
            for(qint32 i=0;i<columnCount;i++){
                QString str="";
                if(!saveList->at(i))
                    continue;
                switch(i){
                case 0:
                    str=QObject::tr("索引");
                    break;
                case 1:
                    str=QObject::tr("开始");
                    break;
                case 2:
                    str=QObject::tr("时间");
                    break;
                case 3:
                    str=QObject::tr("类型");
                    break;
                case 4:
                    str=QObject::tr("数据");
                    break;
                }
                out<<str<<",";
            }
            out<<"\n";
            for(qint32 i=0;i<dataCount;i++){
                const DecodeRowData rowData=data->at(i);
                for(qint32 ii=0;ii<columnCount;ii++){
                    if(saveList->at(ii)){
                        switch(ii){
                        case 0:
                            out<<QString::number(i);
                            break;
                        case 1:
                            out<<nsToShowStr((double)rowData.start*multiply);
                            break;
                        case 2:
                            out<<nsToShowStr((double)((rowData.end-rowData.start)*multiply));
                            break;
                        case 3:
                            for (auto i : rowType->at(rowData.decodeIndex))
                            {
                                if(i["list"].toArray().contains((int)rowData.type)){
                                    out<<i["desc"].toString();
                                    break;
                                }
                            }
                            break;
                        case 4:
                            out<<rowData.longText;
                            break;
                        }
                    }else
                        continue;
                    out<<",";
                }
                out<<"\n";
                if(!isRun())
                    break;
                if(i%scheduleCount==0){
                    qint32 schedule=qMin((qreal)i/dataCount*100,99.);
                    emit SendDeviceRecvSchedule(schedule,4,1);
                }
            }
            out.flush();
            if(!isRun())
            {
                m_sessionError->setError_msg(QObject::tr("导出线程已停止。"));
                isError=true;
            }
        }
    } catch (exception &e) {
        m_sessionError->setError_msg(QObject::tr("保存异常，错误信息:%1").arg(e.what()));
        isError=true;
    } catch (...) {
        m_sessionError->setError_msg(QObject::tr("保存失败，文件数据可能有误。"));
        isError=true;
    }
end:
    try{
        file->flush();
        file->close();
        if(isError)
            file->remove();
        else{
            {
                isError=false;
                QFile fileTarget(filePath);
                if(fileTarget.exists()){
                    if(!fileTarget.remove()){
                        isError=true;
                        m_sessionError->setError_msg(QObject::tr("文件被占用，无法删除文件。"));
                    }
                }
            }
            if(!isError){
                if(!file->rename(filePath))
                    m_sessionError->setError_msg(QObject::tr("文件被占用，无法覆盖文件。"));
            }
        }
        delete file;
    } catch(...){
    }
    emit SendDeviceRecvSchedule(100,4,isRun()?1:0);
    LogHelp::write(QString("    保存协议表格线程结束"));
    setRun(0);
    shared->decrement(SAVE_FILE);
}

void ThreadWork::calcMeasureData(qint32 measureID, qint32 channelID, QVector<MeasureData>* measure, Segment* segment)
{
    setRun(1);
    segment->appendCite();
    MeasureData data;
    bool isSet=false;
    for(qint32 i=0;i<measure->count();i++){
        if(measure->at(i).id==measureID){
            data=(*measure)[i];
            isSet=true;
            break;
        }
    }
    if(isSet){
        bool isRefreshCalc;
        do{
            isRefreshCalc=false;
            calcFRing(data.start, data.end, &data, channelID, segment);
            for(qint32 i=0;i<measure->count();i++){
                if(measure->at(i).id==measureID){
                    MeasureData temp=measure->at(i);
                    isRefreshCalc=temp.isRefreshCalc;
                    if(isRefreshCalc){
                        data=temp;
                        data.isRefreshCalc=false;
                        data.isCalc=true;
                        (*measure)[i]=data;
                    }else{
                        data.isAutoOffset=temp.isAutoOffset;
                        data.isSelect=temp.isSelect;
                        data.note=temp.note;
                        data.offset=temp.offset;
                        data.isRefreshCalc=false;
                        data.isCalc=false;
                        (*measure)[i]=data;
                        break;
                    }
                }
                if(!isRun())
                    break;
            }
        }while(isRefreshCalc&&isRun());
        emit calcMeasureDataComplete(measureID);
    }
    segment->lessenCite();
    setRun(0);
}

void ThreadWork::calcFRing(qint64 start, qint64 end, MeasureData *data, qint32 channelID, Segment* segment)
{
    qint32 multiply=segment->GetMultiply();
    if(multiply<=0||!isRun()||!segment->isData[channelID]){
        data->str.minFreq="NULL";
        data->str.minFreqDetail="NULL";
        data->str.maxFreq="NULL";
        data->str.maxFreqDetail="NULL";
        data->falling=0;
        data->rising=0;
        return;
    }
    qint64 ullChunkEnd=end/multiply;
    qint32 risingCount=0,fallingCount=0;
    qint64 llBlockStart=start/multiply,llBlockEnd=llBlockStart;
    quint64 llMinFreq=0,llMaxFreq=~0ull,llFreqRecode=0,llFreqRecode2=0;
    bool isFirstAppend=true,isFirst=true;
    bool sampleRecode=segment->GetSample(llBlockStart, channelID);
    while (llBlockStart < ullChunkEnd && isRun()) {
        DataEnd data=segment->GetDataEnd(llBlockStart,channelID,false);
        llBlockEnd=data.position;
        if(llBlockEnd>ullChunkEnd)
            llBlockEnd=ullChunkEnd;
        qint64 len=llBlockEnd-llBlockStart;
        if(len){
            //计算最小最大频率
            if(isFirst)
                isFirst=false;
            else{
                //当前循环的长度下个循环再计算，这样就可以不计算最后一个长度
                if(llFreqRecode==0)
                    llFreqRecode=len;
                else if(llFreqRecode2==0)
                {
                    llFreqRecode2=llFreqRecode;
                    llFreqRecode=len;
                }
                else
                {
                    quint64 temp=llFreqRecode+llFreqRecode2;
                    if(temp<llMaxFreq)
                        llMaxFreq=temp;
                    if(temp>llMinFreq)
                        llMinFreq=temp;
                    llFreqRecode2=llFreqRecode;
                    llFreqRecode=len;
                }
            }
            if(isFirstAppend)
                isFirstAppend=false;
            else{
                if(sampleRecode)
                    risingCount++;
                else
                    fallingCount++;
            }
        }
        sampleRecode=!sampleRecode;
        llBlockStart=llBlockEnd;
        llBlockEnd=llBlockStart+1;
        if(data.isEnd)
            break;
    }
    if(llMinFreq==0)
    {
        data->str.minFreq="NULL";
        data->str.minFreqDetail="NULL";
    }else{
        llMinFreq*=multiply;
        data->str.minFreq=hzToShowStr(1000000000/(qreal)llMinFreq,2);
        data->str.minFreqDetail=QString("%1").arg(1000000000/(qreal)llMinFreq, 0, 'f', 2)+" Hz";
    }
    if(llMaxFreq==~0ull){
        data->str.maxFreq="NULL";
        data->str.maxFreqDetail="NULL";
    }else{
        llMaxFreq*=multiply;
        data->str.maxFreq=hzToShowStr(1000000000/(qreal)llMaxFreq,2);
        data->str.maxFreqDetail=QString("%1").arg(1000000000/(qreal)llMaxFreq, 0, 'f', 2)+" Hz";
    }
    data->falling=fallingCount;
    data->rising=risingCount;
}

void ThreadWork::setRun(qint8 state)
{
    m_lock.lock();
    m_run=state;
    m_lock.unlock();
}

bool ThreadWork::isRun()
{
    m_lock.lock();
    qint8 tmp=m_run;
    m_lock.unlock();
    return tmp==1;
}

QString ThreadWork::creanTimestampDir(QString path, bool isFile)
{
    SharedMemoryHelper* shared=&DataService::getInstance()->m_shared;
    QDir savePath(path);
    savePath.mkpath(path);
    QString temp;
    while(!shared->setFileDirCrean(true)){
        QThread::msleep(10);
        QApplication::processEvents(QEventLoop::AllEvents, 50);
    }
    if(path.right(1)!="/")
        path+="/";
    do{
        temp=QString::number(QDateTime::currentSecsSinceEpoch());
        if(isFile){
            QFile file(path+temp);
            if(!file.exists()){
                file.open(QIODevice::WriteOnly | QIODevice::Truncate);
                file.close();
                break;
            }
        }else{
            QDir savePath2(path+temp);
            if(!savePath2.exists()){
                savePath2.mkdir(path+temp);
                break;
            }
        }
        QThread::msleep(10);
    }while(true);
    shared->setFileDirCrean(false);
    path+=temp;
    return path;
}

