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

#include "session.h"

Session::Session(QString name, QString id, qint32 channelCount, SessionType type, QString path, DataService* parent)
    : QObject(0),m_sessionName(name),m_sessionID(id),m_sessionType(type),m_parent(parent),m_channelCount(channelCount),m_filePath(path)
{
    m_segment=new Segment(channelCount);
    m_config=new SessionConfig();
    m_workThread=new ThreadWork();
    connect(this,&Session::sendThreadPara,m_workThread,&ThreadWork::DeviceRecvThread);
    connect(this,&Session::sendSaveFileThreadPara,m_workThread,&ThreadWork::SaveFileThreadPara);
    connect(this,&Session::sendSaveSourceFileThreadPara,m_workThread,&ThreadWork::SaveSourceFileThreadPara);
    connect(this,&Session::sendLoadSourceFileThreadPara,m_workThread,&ThreadWork::LoadSourceFileThreadPara);
    connect(this,&Session::sendGlitchRemovalThreadPara,m_workThread,&ThreadWork::GlitchRemovalThreadPara);
    connect(this,&Session::sendSaveDecodeTableThreadPara,m_workThread,&ThreadWork::SaveDecodeTableThreadPara);
    connect(this,&Session::sendCalcMeasureData,m_workThread,&ThreadWork::calcMeasureData);

    connect(m_workThread,&ThreadWork::appendMeasure,this,&Session::onAppendMeasure);
    connect(m_workThread,&ThreadWork::appendVernier,this,&Session::onAppendVernier);

    for(qint32 i=0;i<channelCount;i++)
        m_channelsName.append("Channel "+QString::number(i));
}

Session::~Session()
{
    deleteSegment();
    for(auto &i : m_decodeList)
        delete i;
    m_workThread->destroy();
    delete m_workThread;
    delete m_config;
    QObject* tab = m_parent->getRoot()->findChild<QObject*>("sessionTab");
    QVariant ret;
    QMetaObject::invokeMethod(tab, "removeSession", Q_RETURN_ARG(QVariant, ret), Q_ARG(QVariant, m_sessionID));
}

void Session::LoadFile(QString path)
{
    try {
        deleteSegment();
        m_segment = new Segment(m_channelCount);
        m_segmentRecode=m_segment;
        m_isShowDecode=true;
        pathRepair(path);
        if(path.endsWith(".atkdl")){
            emit sendLoadSourceFileThreadPara(path,m_segment,m_config,&m_measure,&m_vernier,&m_decodeList,&m_channelsName);
        }
    } catch (exception &e) {
        showErrorMsg(QObject::tr("载入文件失败，错误信息:%1").arg(e.what()),true);
    } catch (...) {
        showErrorMsg(QObject::tr("载入文件失败。"),true);
    }
}

void Session::SaveFile(QString path, QString sessionName, FileType type)
{
    try {
        bool isData=false;
        for(bool i:m_segment->isData){
            if(i)
            {
                isData=true;
                break;
            }
        }
        if(!isData)
        {
            showErrorMsg(QObject::tr("没有可保存的数据。"),true);
            return;
        }
        switch(type){
        case CSVFile:
            LogHelp::write(QString("保存CSV文件:%1").arg(path));
            emit sendSaveFileThreadPara(path,m_segment,m_channelsName);
            break;
        case SourceFile:
            LogHelp::write(QString("保存工程文件:%1").arg(path));
            emit sendSaveSourceFileThreadPara(path, sessionName, m_segmentRecode, m_channelsName,&m_measure,&m_vernier,&m_decodeList);
            break;
        default:
            break;
        }
    } catch (exception &e) {
        showErrorMsg(QObject::tr("保存文件失败，错误信息:%1").arg(e.what()),true);
    } catch (...) {
        showErrorMsg(QObject::tr("保存文件失败。"),true);
    }
}

bool Session::OrderStart(QByteArray dataBytes,QByteArray setBytes, double second, quint64 samplingDepth, quint64 triggerSamplingDepth,
                         qint64 samplingFrequency, qint32 type, QVector<qint8> channelIDList, bool isBuffer, bool isRLE)
{
    bool reft=false;
    Data_* data = new Data_();
    try {
        USBControl* usb=getUSBControl();
        if(usb!=nullptr){
            fpgaActive();
            QThread::msleep(100);
            LogHelp::write(QString("    清空缓存"));
            while(usb->ReadSynchronous(data))
                delete[] data->buf;
            LogHelp::write(QString("    发送配置参数指令，长度：%1").arg(setBytes.size()));
            reft=usb->ParameterSetting((quint8*)setBytes.data(),setBytes.size());
            if(reft){
                QThread::msleep(100);
                LogHelp::write(QString("    发送启动采集指令，长度：%1").arg(dataBytes.size()));
                switch (type) {
                case 0:
                    reft=usb->SimpleTrigger((quint8*)dataBytes.data(),dataBytes.size());
                    break;
                }
                if(reft){
                    usb->m_SessionID=m_sessionID;
                    usb->m_ThreadState=1;
                    deleteSegment();
                    m_segment = new Segment(m_channelCount);
                    m_segmentRecode = m_segment;
                    m_segment->m_SamplingDepth=samplingDepth;
                    m_segment->SetSamplingFrequency(samplingFrequency/1000);
                    m_segment->m_TriggerSamplingDepth=triggerSamplingDepth;
                    m_isShowDecode=false;
                    QVector<qint8>* temp=new QVector<qint8>();
                    temp->append(channelIDList);
                    emit sendThreadPara(usb, m_segment, m_config, second, temp, isBuffer, isRLE);
                }
            }
        }
    } catch (exception &e) {
        reft=false;
        showErrorMsg(QObject::tr("设备启动失败，请尝试重新插拔设备，或者更新固件，错误信息:%1").arg(e.what()),false);
    } catch (...) {
        reft=false;
        showErrorMsg(QObject::tr("设备启动失败，请尝试重新插拔设备，或者更新固件"),false);
    }
    delete data;
    return reft;
}

void Session::showErrorMsg(QString error_msg, bool waitClose_)
{
    m_parent->getSessionError(m_sessionID)->setError_msg(error_msg, waitClose_);
}

void Session::OrderStop()
{
    try {
        USBControl* usb=getUSBControl();
        if(usb&&usb->m_ThreadState==1){
            LogHelp::write(QString("    用户手动停止任务线程"));
            usb->m_ThreadState=2;
        }
    } catch (exception &e) {
        showErrorMsg(QObject::tr("停止失败，错误信息:%1").arg(e.what()),false);
    } catch (...) {
        showErrorMsg(QObject::tr("停止失败。"),false);
    }
}

bool Session::PWM(QByteArray dataBytes)
{
    bool reft=false;
    try {
        reft=getUSBControl()->PWM((quint8*)dataBytes.data(),dataBytes.size());
    } catch (exception &e) {
        reft=false;
        showErrorMsg(QObject::tr("启动失败，错误信息:%1").arg(e.what()),true);
    } catch (...) {
        reft=false;
        showErrorMsg(QObject::tr("启动失败。"),true);
    }
    return reft;
}

qint64 Session::getSamplingFrequency()
{
    if(m_segment!=NULL)
        return m_segment->GetSamplingFrequency();
    return 0;
}

QString Session::newDecode()
{
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    DecodeController* temp=new DecodeController(timestamp);
    m_decodeList[timestamp]=temp;
    return timestamp;
}

QHash<QString, DecodeController *> Session::getDecodeList()
{
    return m_decodeList;
}

void Session::resetDecode(QString decodeID)
{
    if(m_decodeList.contains(decodeID)){
        removeDecode(decodeID);
        DecodeController* temp=new DecodeController(decodeID);
        m_decodeList[decodeID]=temp;
    }
}

void Session::removeDecode(QString decodeID)
{
    if(m_decodeList.contains(decodeID)){
        m_decodeList[decodeID]->stopDedoceAnalysis();
        m_decodeList[decodeID]->waitStopDedoceAnalysis();
        delete m_decodeList[decodeID];
        m_decodeList.remove(decodeID);
    }
}

DecodeController *Session::getDecode(QString decodeID)
{
    if(m_decodeList.contains(decodeID))
        return m_decodeList[decodeID];
    else
        return nullptr;
}

USBControl *Session::getUSBControl()
{
    USBServer* usb=m_parent->getUSBServer();
    if(usb->m_usbPort.contains(m_sessionID))
        if(usb->m_usbControl.contains(usb->m_usbPort[m_sessionID]))
            return usb->m_usbControl[usb->m_usbPort[m_sessionID]];
    return nullptr;
}

void Session::channelNameChanged(qint8 channelID, QString name)
{
    m_channelsName[channelID]=name;
}

bool Session::setGlitchRemoval(QJsonArray json)
{
    LogHelp::write(QString("设置毛刺过滤"));
    m_isShowDecode=false;
    bool reft=true;
    try {
        if(m_segmentRecode){
            bool isSet=false;
            Segment* segment;
            for(int i=0;i<json.count();i++){
                if(json[i].toObject()["enable"].toBool())
                {
                    m_segmentRecode->SetChannelGlitchRemoval(i,json[i].toObject()["num"].toInt());
                    isSet=true;
                }
            }
            m_segmentRecode->m_enableGlitchRemoval=isSet;
            if(isSet){
                segment=m_segmentRecode->Clone();
                if(segment->m_isMemoryError){
                    delete segment;
                    m_segmentRecode->m_enableGlitchRemoval=false;
                    showErrorMsg(QObject::tr("内存不足，克隆数据失败，无法进行毛刺过滤。"),true);
                    return false;
                }
            }
            if(m_segmentRecode!=m_segment){
                qint32 count=0;
                while(m_segment->isCite()){
                    QApplication::processEvents(QEventLoop::AllEvents, 50);
                    QThread::msleep(10);
                    count++;
                    if(count>1000)
                        break;
                }
                delete m_segment;
            }
            
            if(isSet){
                m_segment=segment;
                emit sendGlitchRemovalThreadPara(m_segment);
            }else
            {
                m_segment=m_segmentRecode;
                while(json.count())
                    json.removeFirst();
                for(qint32 i=0;i<m_segment->m_isFirst.count();i++)
                    m_segment->m_isFirst[i]=true;
                emit drawUpdate();
                emit sendDeviceRecvSchedule(100,6,1);
            }
        }
        m_GlitchRemovalJsonArray=json;
    } catch (exception &e) {
        reft=false;
        showErrorMsg(QObject::tr("设置失败，错误信息:%1").arg(e.what()),true);
    } catch (...) {
        reft=false;
        showErrorMsg(QObject::tr("设置失败。"),true);
    }
    return reft;
}

QJsonArray Session::getGlitchRemoval()
{
    QJsonArray array;
    m_segment->appendCite();
    for(qint32 i=0;i<m_segment->GetChannelNum();i++){
        QJsonObject jb;
        qint32 t=m_segment->GetChannelGlitchRemoval(i);
        if(!m_segment->m_enableGlitchRemoval || t<1)
            jb["enable"]=false;
        else
            jb["enable"]=true;
        if(t<1)
            t=1;
        jb["num"]=t;
        array.append(jb);
    }
    m_segment->lessenCite();
    return array;
}

void Session::setDecodeShowJson(QString decodeID, QJsonObject json)
{
    DecodeController* decode=getDecode(decodeID);
    if(decode){
        decode->setRecodeJSON(json);
        for(QString &key : decode->m_rowList.keys()){
            if(json.contains(key)){
                for(auto t : json[key].toObject()["annotation_rows"].toArray()){
                    QJsonObject json=t.toObject();
                    for(qint32 i=0;i<decode->m_rowList[key]->row.count();i++){
                        DecodeRow* row=&decode->m_rowList[key]->row[i];
                        if(row->id==json["id"].toString()){
                            row->isShow=json["isShow"].toBool();
                            break;
                        }
                    }
                }
            }
        }
        emit drawUpdate();
    }
}

void Session::setRecodeJSON(QString decodeID, QJsonObject json)
{
    DecodeController* decode=getDecode(decodeID);
    if(decode)
        decode->setRecodeJSON(json);
}

void Session::initError()
{
    SessionError *initError=m_parent->getSessionError(m_sessionID);
    assert(initError!=nullptr);
    m_workThread->initError(initError);

    
    if(m_sessionType==SessionType::File)
    {
        if(m_filePath.endsWith(".atkdl"))
            LogHelp::write(QString("打开工程文件:%1").arg(m_filePath));
        else
            LogHelp::write(QString("打开CSV文件:%1").arg(m_filePath));
        LoadFile(m_filePath);
    }
}


void Session::calcMeasureData(qint32 measureID, qint32 channelID)
{
    qint32 count=m_measure.count();
    bool isSend=true;
    for(qint32 i=0;i<count;i++){
        if(m_measure[i].id==measureID){
            MeasureData temp=m_measure[i];
            m_measure[i].str.time=nsToShowStr(temp.end-temp.start,2);
            m_measure[i].str.timeDetail=QString::number(temp.end-temp.start)+" ns";
            m_measure[i].str.start=nsToShowStr(temp.start,2);
            m_measure[i].str.startDetail=QString::number(temp.start)+" ns";
            m_measure[i].str.end=nsToShowStr(temp.end,2);
            m_measure[i].str.endDetail=QString::number(temp.end)+" ns";
            if(m_measure[i].isCalc){
                if(m_measure[i].isRefreshCalc)
                    isSend=false;
                else
                    m_measure[i].isRefreshCalc=true;

            }
            break;
        }
    }
    if(isSend)
        emit sendCalcMeasureData(measureID, channelID, &m_measure, m_segment);
}

void Session::fpgaActive()
{
    USBControl* usb=getUSBControl();
    if(usb!=nullptr){
        usb->SetResetState(1);
        Data_* data = new Data_();
        if(usb->ReadSynchronous(data))
            delete[] data->buf;
        delete data;
    }
}

void Session::fpgaDormancy()
{
    try {
        getUSBControl()->SetResetState(0);
    } catch (exception &e) {
        showErrorMsg(QObject::tr("设置休眠失败，错误信息:%1").arg(e.what()),true);
    } catch (...) {
        showErrorMsg(QObject::tr("设置休眠失败。"),true);
    }
}

void Session::saveDecodeTable(QString filePath)
{
    if(m_tableData!=nullptr && m_tableRowType!=nullptr){
        pathRepair(filePath);
        if(filePath.endsWith(".csv"))
            emit sendSaveDecodeTableThreadPara(filePath,m_tableMultiply,m_tableData,m_tableRowType,&m_tableSaveIndex);
    }
    m_tableData=nullptr;
    m_tableRowType=nullptr;
}

void Session::deleteSegment()
{
    if (m_segment != nullptr){
        qint32 count=0;
        while(m_segment->isCite()){
            QApplication::processEvents(QEventLoop::AllEvents, 50);
            QThread::msleep(10);
            count++;
            if(count>1000)
                break;
        }
        delete m_segment;
    }
    if (m_segmentRecode != m_segment && m_segmentRecode != nullptr){
        qint32 count=0;
        while(m_segmentRecode->isCite()){
            QApplication::processEvents(QEventLoop::AllEvents, 50);
            QThread::msleep(10);
            count++;
            if(count>1000)
                break;
        }
        delete m_segmentRecode;
    }
}

void Session::onAppendMeasure(qint32 measureIndex)
{
    MeasureData* data=&m_measure[measureIndex];
    data->id=m_measureIndex;
    data->isSelect=false;
    data->isCalc=true;
    calcMeasureData(data->id, data->channelID);
    m_measureIndex++;
    emit appendMeasure(data->id);
}

void Session::onAppendVernier(qint32 vernierIndex)
{
    if(vernierIndex>=0){
        VernierData* data=&m_vernier[vernierIndex];
        data->positionStr=nsToShowStr(data->position);
        data->visible=true;
        data->id=m_vernierIndex;
        m_vernierIndex++;
        emit appendVernier();
    }else if(vernierIndex==-1)
        m_vernierIndex=1;
}
