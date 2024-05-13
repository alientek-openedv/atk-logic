#include "session_controller.h"

SessionController::SessionController(QObject* parent) : QObject(parent){}

bool SessionController::cisInit()
{
    if(m_sessionType==2)
        return (!m_sessionName.isEmpty() && !m_filePath.isEmpty() && m_sessionPort!=-10 && m_channelCount!=0 && m_session==nullptr);
    else
        return (!m_sessionName.isEmpty() && m_sessionType!=-1 && m_sessionPort!=-10 && m_channelCount!=0 && m_session==nullptr);
}

void SessionController::init()
{
    m_dataService = DataService::getInstance();
    m_sessionID=m_dataService->getNewSessionID();

    if(m_sessionPort>=0)
    {
        if(!m_isBinding)
            resetBindingPort();
        else
            m_dataService->getWindowError()->setError_msg(QObject::tr("重复绑定端口！"),true);
    }

    m_session=m_dataService->newSession(m_sessionName,m_sessionID,m_channelCount,(SessionType)m_sessionType,m_filePath);
    connect(m_session,&Session::drawUpdate,this,&SessionController::sessionDrawUpdate);
    connect(m_session,&Session::refreshZoom,this,&SessionController::onRefreshZoom);
    connect(m_session,&Session::appendMeasure,this,&SessionController::appendMeasure);
    connect(m_session,&Session::appendVernier,this,&SessionController::appendVernier);
    connect(m_session,&Session::sendDeviceRecvSchedule,this,&SessionController::sendDeviceRecvSchedule);
    connect(m_session->m_workThread,&ThreadWork::sendVernierTriggerPosition,this,&SessionController::sendVernierTriggerPosition);
    connect(m_session->m_workThread,&ThreadWork::SendDeviceRecvSchedule,this,&SessionController::sendDeviceRecvSchedule);
    connect(m_session->m_workThread,&ThreadWork::saveSessionSettings,this,&SessionController::saveSessionSettings);
    connect(m_session->m_workThread,&ThreadWork::loadSessionSettings,this,&SessionController::loadSessionSettings);
    connect(m_session->m_workThread,&ThreadWork::channelNameModification,this,&SessionController::channelNameModification);
    connect(m_session->m_workThread,&ThreadWork::channelNameModification,this,&SessionController::channelNameChanged);
    connect(m_session->m_workThread,&ThreadWork::appendDecode,this,&SessionController::onAppendDecode);
    connect(m_session->m_workThread,&ThreadWork::timerDrawUpdate,this,&SessionController::timerDrawUpdate);
    connect(m_session->m_workThread,&ThreadWork::calcMeasureDataComplete,this,&SessionController::calcMeasureDataComplete);
    connect(m_session->m_workThread,&ThreadWork::sendGlitchString,this,&SessionController::sendGlitchString);
    connect(m_session->m_workThread,&ThreadWork::sendZipDirSchedule,this,&SessionController::sendZipDirSchedule);
    connect(m_session->m_workThread,&ThreadWork::sendUnZipDirSchedule,this,&SessionController::sendUnZipDirSchedule);
    connect(m_session->m_workThread,&ThreadWork::decodeToPosition,this,&SessionController::onDecodeToPosition);

    connect(this,&SessionController::measureDataChanged,m_session,&Session::measureDataChanged);
    setIsInit(1);
    if(m_sessionType>0)
        LogHelp::write(QString("会话创建完成"));
}

void SessionController::onRefreshZoom(qint32 state)
{
    if(state==1 || (state==0 && m_session->m_config->m_isCustom))
    {
        m_session->m_config->m_selectShowIndex=m_session->m_config->m_showList.count()-1;
        m_session->m_config->m_showStartUnit=0;
        quint32 temp=1000;
        while(temp!=m_session->m_config->m_selectShowIndex){
            temp=m_session->m_config->m_selectShowIndex;
            wheelRoll(false,0);
        }
    }
    else if(state==2)
        wheelRoll(true,m_session->m_config->m_width/2);
    else if(state==3)
        wheelRoll(false,m_session->m_config->m_width/2);
    else if(state==4)
        m_session->m_config->m_showStartUnit=0;
    else{
        wheelRoll(true,m_session->m_config->m_width/2);
        wheelRoll(false,m_session->m_config->m_width/2);
    }
}

void SessionController::onSessionSelect()
{
    emit zoomUnitShow(nsToShowStr(m_session->m_config->m_showConfig.m_pixelUnit));
}

void SessionController::onAppendDecode(QString json, bool end)
{
    if(end)
        startDecodeTmp();
    else
        m_decodeTmp.append(json);
}

void SessionController::onDecodeToPosition(qint64 position, qint64 maxSample_)
{
    m_session->m_isShowDecode=true;
    QHash<QString,DecodeController*> list=m_session->getDecodeList();
    for(auto &i : list){
        if(position<0 && maxSample_<0){
            i->stopDedoceAnalysis();
        }else if(i->decodeToPosition(m_session->m_segment->GetMultiply(),position,maxSample_)){
            connect(this, &SessionController::sendRestartDevice, i, &DecodeController::restart);
            emit sendRestartDevice(m_session->m_segment,maxSample_,true);
            disconnect(this,&SessionController::sendRestartDevice,nullptr,nullptr);
        }
    }
}

void SessionController::initFile()
{
    m_session->initFile();
}

void SessionController::resetBindingPort()
{
    USBServer* usbServer=m_dataService->getUSBServer();
    usbServer->BindingPort(m_sessionPort, m_sessionID);
    m_isBinding=true;
}

void SessionController::errorInit()
{
    SessionError *sessionError=qobject_cast <SessionError*>(m_dataService->getRoot()->findChild<QObject*>("sessionError"+m_sessionID));
    assert(sessionError!=nullptr);
    m_dataService->setSessionError(m_sessionID,sessionError);
    m_session->initError();
}

#pragma region "QML属性" {

qint32 SessionController::sessionType() const{return m_sessionType;}

void SessionController::setSessionType(qint32 newSessionType)
{
    if (m_sessionType == newSessionType)
        return;
    m_sessionType = newSessionType;
    if(cisInit())
        init();
}

const QString &SessionController::sessionName() const{return m_sessionName;}

void SessionController::setSessionName(const QString &newSessionName)
{
    if (m_sessionName == newSessionName)
        return;
    m_sessionName = newSessionName;

    if(cisInit())
        init();
}

const QString &SessionController::sessionID() const{return m_sessionID;}

qint32 SessionController::isInit() const{return m_isInit;}

void SessionController::setIsInit(qint32 newIsInit)
{
    if (m_isInit == newIsInit)
        return;
    m_isInit = newIsInit;
    emit isInitChanged();
}

int SessionController::sessionPort() const
{
    return m_sessionPort;
}

void SessionController::setSessionPort(qint32 newSessionPort)
{
    if (m_sessionPort == newSessionPort)
        return;
    m_sessionPort = newSessionPort;

    if(cisInit())
        init();
}

qint32 SessionController::channelCount() const
{
    return m_channelCount;
}

void SessionController::setChannelCount(qint32 newChannelCount)
{
    if (m_channelCount == newChannelCount)
        return;
    m_channelCount = newChannelCount;

    if(cisInit())
        init();
}

const QString &SessionController::filePath() const
{
    return m_filePath;
}

void SessionController::setFilePath(const QString &newFilePath)
{
    if (m_filePath == newFilePath)
        return;
    m_filePath = newFilePath;

    if(cisInit())
        init();
}

#pragma endregion}

#pragma region "QML方法" {

bool SessionController::start(QJsonObject json,qint32 type)
{
    m_progressBarShow=false;
    if(m_session==nullptr || m_sessionType!=1)
        return false;
    LogHelp::write(QString("启动采集"));
    QByteArray setArray,dataBytes;
    QString log="采集参数: ";
    quint64 samplingDepth=0,triggerSamplingDepth=0;
    double second;
    bool isBuffer=false,isRLE=false,isTrigger=false;
    uchar b=0;
    //设置属性
    {
        QJsonObject settingJson=json["settingData"].toObject();
        //RLE
        isRLE=settingJson["RLE"].toBool();
        //运行模式
        isBuffer=settingJson["isBuffer"].toBool();
        log="RLE:"+QString::number(isRLE);
        log+=",Buffer:"+QString::number(isBuffer);
        if(isBuffer)
            b+=128;
        //RLE
        if(isRLE)
            b+=64;
        //时钟下降沿采样
        //        if(settingJson["clockNegedge"].toBool())
        //            b+=32;
        //滤波器设置
        //        if(settingJson["filterTargets"].toInt()==2||settingJson["filterTargets"].toInt()==3)
        //            b+=16;
        //        if(settingJson["filterTargets"].toInt()==1||settingJson["filterTargets"].toInt()==3)
        //            b+=8;
        setArray.append(b);
        //电压阈值
        b=0;
        log+=",阈值:"+QString::number(settingJson["thresholdLevel"].toDouble());
        if(settingJson["thresholdLevel"].toDouble()<0)
            b+=128;
        b+=qAbs(qRound(settingJson["thresholdLevel"].toDouble()*10));
        setArray.append(b);
        //Hz
        setArray.append((uchar)settingJson["selectHzIndex"].toInt()+1);
        //Buffer=采集深度(Sa) Sa=秒*Hz
        second=settingJson["setTime"].toDouble()/1000;
        log+=",hz:"+QString::number(settingJson["setHz"].toDouble());
        log+=",time:"+QString::number(settingJson["setTime"].toDouble())+"ms";
        samplingDepth=(quint64)settingJson["setHz"].toDouble()/1000*settingJson["setTime"].toDouble();
        log+=",采样深度:"+QString::number(samplingDepth);
        setArray.append(intToBytes(samplingDepth,5));
        //触发位置
        triggerSamplingDepth=(quint64)(samplingDepth/100 * settingJson["triggerPosition"].toDouble());
        log+=",触发位置:"+QString::number(triggerSamplingDepth);
        setArray.append(intToBytes(triggerSamplingDepth,5));
        m_session->m_config->m_maxUnit=(qint64)settingJson["setTime"].toDouble()*1000L*1000L;
    }
    QVector<qint8> channelIDList;
    //通道设置
    if(type==0){
        //简单触发模式
        log+=",立即采集:"+QString::number(json["isInstantly"].toBool());
        log+=",开启通道:";
        QJsonArray array=json["channelsSet"].toArray();
        for(int i=0;i<array.count();i+=2){
            b=0;
            for(int ii=0;ii<2;ii++){
                int index=i+ii;
                if(array[index].toObject()["enable"].toBool()){
                    log+=QString::number(index)+",";
                    channelIDList.append(index);
                    b+=(uchar)(ii%2==0?128:8);
                    switch((ChannelTriggerButton)array[index].toObject()["triggerType"].toInt()){
                    case RisingEdge:
                        log+="RisingEdge";
                        b+=(uchar)(ii%2==0?16:1);
                        isTrigger=true;
                        break;
                    case HighLevel:
                        log+="HighLevel";
                        b+=(uchar)(ii%2==0?64:4);
                        isTrigger=true;
                        break;
                    case FallingEdge:
                        log+="FallingEdge";
                        b+=(uchar)(ii%2==0?32:2);
                        isTrigger=true;
                        break;
                    case LowLevel:
                        log+="LowLevel";
                        isTrigger=true;
                        break;
                    case DoubleEdge:
                        log+="DoubleEdge";
                        b+=(uchar)(ii%2==0?16:1);
                        b+=(uchar)(ii%2==0?32:2);
                        isTrigger=true;
                        break;
                    default:
                        log+="Default";
                        b+=(uchar)(ii%2==0?16:1);
                        b+=(uchar)(ii%2==0?32:2);
                        b+=(uchar)(ii%2==0?64:4);
                        break;
                    }
                    log+=";";
                }
            }
            dataBytes.append(b);
        }
        if(json["isInstantly"].toBool())
            isTrigger=false;
        dataBytes.append((uchar)(json["isInstantly"].toBool()?1:0));
    }else if(type==1){
        //多级触发模式
        QJsonObject stageJson=json["stageTriggerData"].toObject();
    }
    LogHelp::write(log);
    if(!m_session->OrderStart(dataBytes,setArray,second,samplingDepth,triggerSamplingDepth,
                              json["settingData"].toObject()["setHz"].toInt(),type,channelIDList,isBuffer,isRLE))
    {
        m_dataService->getSessionError(m_sessionID)->setError_msg(QObject::tr("USB通讯失败，无法发送指令。"),false);
        return false;
    }
    if(!isTrigger && !isBuffer)
        return true;
    m_progressBarShow=true;
    return true;
}

bool SessionController::getProgressBarShow(){
    return m_progressBarShow;
}

void SessionController::stop()
{
    if(m_session==nullptr || m_sessionType!=1)
        return;
    m_session->OrderStop();
}

bool SessionController::pwmStart(qint8 index, qint32 hz, qint32 duty)
{
    if(m_session==nullptr || m_sessionType!=1)
        return false;
    LogHelp::write(QString("PWM%1设置Hz:%2，duty:%3").arg(QString::number(index),QString::number(hz),QString::number(duty)));
    QByteArray dataBytes;
    qint32 maxHz=200000000;
    maxHz=qRound(maxHz/(double)hz);
    dataBytes.append(index?0x21:0x11);
    dataBytes.append(intToBytes(maxHz));
    dataBytes.append(intToBytes(qRound(maxHz*((double)duty/100))));
    return m_session->PWM(dataBytes);
}

bool SessionController::pwmStop(qint8 index)
{
    if(m_session==nullptr)
        return false;
    LogHelp::write(QString("PWM%1停止").arg(QString::number(index)));
    QByteArray dataBytes;
    dataBytes.append(index?0x20:0x10);
    return m_session->PWM(dataBytes);
}

QString SessionController::newDecode()
{
    if(m_session==nullptr)
        return "";
    return m_session->newDecode();
}

bool SessionController::decode(QString decodeID, QJsonObject json)
{
    if(m_session==nullptr)
        return false;
    m_session->m_segment->appendCite();
    bool isAdd=m_session->getDecode(decodeID)->analysisJSON(json, m_session->getSamplingFrequency()*1000);
    if(!isAdd)
    {
        m_session->m_segment->lessenCite();
        m_dataService->getSessionError(m_sessionID)->setError_msg(QObject::tr("创建编码器失败。"),false);
        return false;
    }
    connect(m_session->getDecode(decodeID), &DecodeController::sendDecodeReset,this, &SessionController::sendDecodeReset);
    connect(m_session->getDecode(decodeID), &DecodeController::sendDecodeSchedule,this, &SessionController::sendDecodeSchedule);
    connect(this, &SessionController::sendDecodeAnalysis, m_session->getDecode(decodeID), &DecodeController::decodeAnalysis);
    quint32 multiply=qMax((qint32)m_session->m_segment->GetMultiply(),1);
    emit sendDecodeAnalysis(m_session->m_segment, json["main"].toObject()["start"].toString().toLongLong(),
            json["main"].toObject()["end"].toString().toLongLong(),m_session->m_config->m_maxUnit/multiply, false);
    disconnect(this,&SessionController::sendDecodeAnalysis,nullptr,nullptr);
    m_session->m_segment->lessenCite();
    return true;
}

void SessionController::stopAllDecode()
{
    if(m_session==nullptr)
        return;
    QHash<QString,DecodeController*> list=m_session->getDecodeList();
    if(list.count()>0){
        QThread::msleep(50);
        for(auto &i : list)
            i->stopDedoceAnalysis();

        for(auto &i : list)
            i->waitStopDedoceAnalysis();
    }
}

void SessionController::restartAllDecode()
{
    if(m_session==nullptr)
        return;
    m_session->m_isShowDecode=true;
    QHash<QString,DecodeController*> list=m_session->getDecodeList();
    for(auto &i : list){
        connect(this, &SessionController::sendRestartDevice, i, &DecodeController::restart);
        emit sendRestartDevice(m_session->m_segment,m_session->m_config->m_maxUnit,false);
        disconnect(this,&SessionController::sendRestartDevice,nullptr,nullptr);
    }
}

void SessionController::restartDecode(QString decodeID)
{
    m_session->m_isShowDecode=true;
    DecodeController* decode=m_session->getDecode(decodeID);
    decode->stopDedoceAnalysis();
    decode->waitStopDedoceAnalysis();
    connect(this, &SessionController::sendRestartDevice, decode, &DecodeController::restart);
    emit sendRestartDevice(m_session->m_segment,m_session->m_config->m_maxUnit,false);
    disconnect(this,&SessionController::sendRestartDevice,nullptr,nullptr);
}

void SessionController::setRecodeJSON(QString decodeID, QJsonObject json)
{
    m_session->setRecodeJSON(decodeID,json);
    restartDecode(decodeID);
}

void SessionController::stopDecode(QString decodeID)
{
    if(m_session==nullptr)
        return;
    m_session->getDecode(decodeID)->stopDedoceAnalysis();
}

bool SessionController::resetDecode(QString decodeID, QJsonObject json)
{
    if(m_session==nullptr)
        return false;
    m_session->resetDecode(decodeID);
    return decode(decodeID,json);
}

void SessionController::removeDecode(QString decodeID)
{
    if(m_session==nullptr)
        return;
    m_session->removeDecode(decodeID);
}

void SessionController::cleanAllDecode()
{
    QHash<QString,DecodeController*> list=m_session->getDecodeList();
    for(auto &i : list){
        i->stopDedoceAnalysis();
        i->waitStopDedoceAnalysis();
        i->deleteData();
    }
}

void SessionController::setDecodeShowJson(QString decodeID, QJsonObject json)
{
    if(m_session==nullptr)
        return;
    m_session->setDecodeShowJson(decodeID, json);
}

QVariantMap SessionController::getShowConfig()
{
    QVariantMap res;
    if(m_session!=nullptr)
    {
        res["position"]=(double)m_session->m_config->m_showStartUnit/m_session->m_config->m_maxUnit;
        res["size"]=(m_session->m_config->m_width-15)*m_session->m_config->m_showConfig.m_pixelUnit/m_session->m_config->m_maxUnit;
    }
    return res;
}

void SessionController::setScrollBarPosition(double position)
{
    m_session->m_config->m_showStartUnit=position*m_session->m_config->m_maxUnit;
    emit sessionDrawUpdate(-2);
}

void SessionController::saveData(QString path, QString sessionName)
{
    FileType type=FileType::NotFile;
    QString suffix=path.right(path.length()-path.lastIndexOf(".")).toLower();
    pathRepair(path);
    if(suffix==".csv")
        type=FileType::CSVFile;
    else if(suffix==".atkdl")
        type=FileType::SourceFile;
    if(type!=FileType::NotFile)
        m_session->SaveFile(path, sessionName, type);
}

void SessionController::saveBinData(QString path, qint32 type_, QString channels)
{
    FileType type=FileType::NotFile;
    QString suffix=path.right(path.length()-path.lastIndexOf(".")).toLower();
    pathRepair(path);
    if(suffix==".bin"){
        if(type_==0)
            type=FileType::BinSampleFile;
        else if(type_==1)
            type=FileType::BinNsFile;
        if(type!=FileType::NotFile)
            m_session->SaveFile(path, channels, type);
    }
}

bool SessionController::needSaveData()
{
    Segment* segment=m_session->m_segmentRecode;
    if(!segment)
        segment=m_session->m_segment;
    return hasData() && !segment->m_isSave;
}

bool SessionController::hasData()
{
    bool isData=false;
    Segment* segment=m_session->m_segmentRecode;
    if(!segment)
        segment=m_session->m_segment;
    if(segment){
        for(bool i : segment->isData){
            if(i)
            {
                isData=true;
                break;
            }
        }
    }
    return isData;
}

void SessionController::channelNameChanged(qint8 channelID, QString name)
{
    m_session->channelNameChanged(channelID,name);
}

void SessionController::threadStop(bool waitClose)
{
    m_session->m_workThread->stop(waitClose);
}

void SessionController::setLiveFollowing(bool isEnable)
{
    m_session->m_workThread->setLiveFollowing(isEnable);
}


bool SessionController::setGlitchRemoval(QJsonArray json)
{
    stopAllDecode();
    return m_session->setGlitchRemoval(json);
}

QJsonArray SessionController::getGlitchRemoval()
{
    return m_session->getGlitchRemoval();
}

void SessionController::openFile(QString path)
{
    m_session->LoadFile(path,true);
}

qint64 SessionController::getChannelMaxSample(qint8 channelID)
{
    return m_session->m_segment->GetMaxSample(channelID);
}

void SessionController::deleteSetDirectory(QString path)
{
    QDir savePath(path);
    if(savePath.exists())
        savePath.removeRecursively();
    savePath.rmpath(path);
}

QString SessionController::getChannelName(qint8 channelID)
{
    if(channelID>=0 && channelID<m_session->m_channelsName.count())
        return m_session->m_channelsName[channelID];
    return "";
}

void SessionController::log(QString log)
{
    LogHelp::write(log);
}

void SessionController::forceRefresh()
{
    for(qint32 i=0;i<m_session->m_segment->m_isFirst.count();i++)
        m_session->m_segment->m_isFirst[i]=true;
    emit sessionDrawUpdate();
}

void SessionController::resetDisplayRange()
{
    SessionShowConfig config=m_session->m_config->m_showConfig;
    if(config.m_pixelUnit*m_session->m_config->m_width > m_session->m_config->m_maxUnit)
        onRefreshZoom(1);
    else if(m_session->m_config->m_showStartUnit+config.m_pixelUnit*m_session->m_config->m_width > m_session->m_config->m_maxUnit)
        m_session->m_config->m_showStartUnit=m_session->m_config->m_maxUnit-config.m_pixelUnit*m_session->m_config->m_width;
    emit m_session->drawUpdate();
}

void SessionController::fpgaActive()
{
    m_session->fpgaActive();
}

void SessionController::fpgaDormancy()
{
    m_session->fpgaDormancy();
}

void SessionController::restartGlitchRemoval()
{
    if(isGlitchRemoval())
        m_session->setGlitchRemoval(m_session->m_GlitchRemovalJsonArray);
}

bool SessionController::isGlitchRemoval()
{
    return m_session->m_GlitchRemovalJsonArray.count()>0;
}

void SessionController::startDecodeTmp()
{
    for(qint32 i=0;i<m_decodeTmp.count();i++){
        emit appendDecode(m_decodeTmp[i]);
        QThread::msleep(10);
    }
    m_decodeTmp.clear();
}

void SessionController::setMeasureSelect(qint32 measureID)
{
    for(qint32 i=0;i<m_session->m_measure.count();i++){
        if(m_session->m_measure[i].id==measureID)
            m_session->m_measure[i].isSelect=true;
        else
            m_session->m_measure[i].isSelect=false;
    }
    emit measureSelectChanged();
}

void SessionController::saveDecodeTable(QString filePath)
{
    m_session->saveDecodeTable(filePath);
}

void SessionController::setShowStart(qint64 showStart, qint32 refreshState)
{
    for(qint32 i=0;i<m_session->m_segment->m_isFirst.count();i++)
        m_session->m_segment->m_isFirst[i]=true;
    if(showStart>=0)
        m_session->m_config->m_showStartUnit=getShowStart(showStart);
    emit sessionDrawUpdate(refreshState);
}

void SessionController::switchVernier(bool isUp,bool isZoom)
{
    qint32 count=m_session->m_vernier.count();
    bool isSet=false;
    for(qint32 i=0;i<count;i++){
        if(m_session->m_vernier[i].isSelect && m_session->m_vernier[i].visible){
            qint32 index=i;
            if(isUp){
                index--;
                if(index<0)
                    index=count-1;
                while(index!=i){
                    if(m_session->m_vernier[index].visible){
                        m_session->m_vernier[i].isSelect=false;
                        m_session->m_vernier[index].isSelect=true;
                        showViewScope(m_session->m_vernier[index].position,m_session->m_vernier[index].position,isZoom);
                        isSet=true;
                        break;
                    }
                    index--;
                    if(index<0)
                        index=count-1;
                }
            }else{
                index++;
                if(index>=count)
                    index=0;
                while(index!=i){
                    if(m_session->m_vernier[index].visible){
                        m_session->m_vernier[i].isSelect=false;
                        m_session->m_vernier[index].isSelect=true;
                        showViewScope(m_session->m_vernier[index].position,m_session->m_vernier[index].position,isZoom);
                        isSet=true;
                        break;
                    }
                    index++;
                    if(index>=count)
                        index=0;
                }
            }
            break;
        }
    }
    if(!isSet){
        for(qint32 i=0;i<count;i++){
            if(m_session->m_vernier[i].visible){
                m_session->m_vernier[i].isSelect=true;
                showViewScope(m_session->m_vernier[i].position,m_session->m_vernier[i].position,isZoom);
                break;
            }
        }
    }
}

qint64 SessionController::getShowStart()
{
    return m_session->m_config->m_showStartUnit;
}

qint64 SessionController::getShowStart(qint64 showStart)
{
    if(showStart>m_session->m_config->m_maxUnit-m_session->m_config->m_showConfig.m_pixelUnit*(m_session->m_config->m_width-15))
        showStart=m_session->m_config->m_maxUnit-m_session->m_config->m_showConfig.m_pixelUnit*(m_session->m_config->m_width-15);
    if(showStart<=0)
        showStart=0;
    return showStart;
}

qint64 SessionController::getShowStartMultiply()
{
    if(m_session->m_segment!=nullptr && m_session->m_segment->GetMultiply()>0)
        return m_session->m_config->m_showStartUnit/m_session->m_segment->GetMultiply();
    return 0;
}

qint64 SessionController::getXWheelPosition(bool isUp, qint32 count)
{
    if(isUp)
        return m_session->m_config->m_showStartUnit-m_session->m_config->m_showConfig.m_pixelUnit*(m_session->m_config->m_width*0.2)*count;
    else
        return m_session->m_config->m_showStartUnit+m_session->m_config->m_showConfig.m_pixelUnit*(m_session->m_config->m_width*0.2)*count;
}

void SessionController::reloadDecoder()
{
    QHash<QString,DecodeController*> list=m_session->getDecodeList();
    for(auto &i : list)
        i->reloadDecoder();
}

void SessionController::wheelRoll(bool isUp,qint32 x)
{
    SessionShowConfig config=m_session->m_config->m_showConfig;
    if(isUp)
    {
        m_session->m_config->m_selectShowIndex++;
        if((qint32)m_session->m_config->m_selectShowIndex>=m_session->m_config->m_showList.count())
            m_session->m_config->m_selectShowIndex=m_session->m_config->m_showList.count()-1;
        m_session->m_config->m_showConfig=m_session->m_config->m_showList[m_session->m_config->m_selectShowIndex];
        m_session->m_config->m_showStartUnit+=(config.m_pixelUnit-m_session->m_config->m_showConfig.m_pixelUnit)*x;
    }
    else
    {
        if(m_session->m_config->m_selectShowIndex!=0)
            m_session->m_config->m_selectShowIndex--;
        m_session->m_config->m_showConfig=m_session->m_config->m_showList[m_session->m_config->m_selectShowIndex];
        qreal offset=m_session->m_config->m_showConfig.m_pixelUnit*x-config.m_pixelUnit*x;
        if(m_session->m_config->m_showStartUnit-offset<=0)
        {
            m_session->m_config->m_showStartUnit=0;
            if(m_session->m_config->m_showConfig.m_pixelUnit*m_session->m_config->m_width > m_session->m_config->m_maxUnit)
                goto custom;
        }
        else if(m_session->m_config->m_showStartUnit+m_session->m_config->m_showConfig.m_pixelUnit*m_session->m_config->m_width > m_session->m_config->m_maxUnit)
        {
            double office=5;
            if(config.m_pixelUnit<1)
                office=1/config.m_pixelUnit;
            m_session->m_config->m_showStartUnit=m_session->m_config->m_maxUnit-(m_session->m_config->m_width-office)*m_session->m_config->m_showConfig.m_pixelUnit;
            if(m_session->m_config->m_showStartUnit<0)
                goto custom;
        }
        else
            m_session->m_config->m_showStartUnit-=offset;
    }
    if(m_session->m_config->m_showStartUnit<0)
        m_session->m_config->m_showStartUnit=0;
    m_session->m_config->m_isCustom=false;
    emit zoomUnitShow(nsToShowStr(m_session->m_config->m_showConfig.m_pixelUnit));
    emit sessionDrawUpdate();
    return;
custom:
    config.m_pixelUnit=(qreal)m_session->m_config->m_maxUnit/(m_session->m_config->m_width-15);
    config.m_scalePixel=config.m_unitMultiply/config.m_pixelUnit;
    m_session->m_config->m_showStartUnit=0;
    //刷新最大索引
    m_session->m_config->m_selectShowIndex=m_session->m_config->m_showList.count()-1;
    while(m_session->m_config->m_selectShowIndex>0){
        SessionShowConfig config_tmp=m_session->m_config->m_showList[m_session->m_config->m_selectShowIndex];
        if((config_tmp.m_pixelUnit*m_session->m_config->m_width) > m_session->m_config->m_maxUnit)
            break;
        else
            m_session->m_config->m_selectShowIndex--;
    }
    m_session->m_config->m_showConfig=config;
    m_session->m_config->m_isCustom=true;
    emit zoomUnitShow(nsToShowStr(m_session->m_config->m_showConfig.m_pixelUnit));
    emit sessionDrawUpdate();
}

void SessionController::showViewScope(qint64 start, qint64 end,bool isZoom)
{
    qint64 len=end-start+1;
    SessionShowConfig config=m_session->m_config->m_showConfig;
    if(isZoom){
        config=m_session->m_config->m_showList[0];
        m_session->m_config->m_selectShowIndex=0;
        for(qint32 i=0;i<m_session->m_config->m_showList.count();i++){
            if(m_session->m_config->m_showList[i].m_pixelUnit*m_session->m_config->m_width<len)
                break;
            config=m_session->m_config->m_showList[i];
            m_session->m_config->m_selectShowIndex++;
        }
        m_session->m_config->m_showConfig=config;
    }
    m_session->m_config->m_showStartUnit=start-(config.m_pixelUnit*m_session->m_config->m_width-len)/2;
    if(m_session->m_config->m_showStartUnit<0)
        m_session->m_config->m_showStartUnit=0;
    emit zoomUnitShow(nsToShowStr(m_session->m_config->m_showConfig.m_pixelUnit));
    emit sessionDrawUpdate();
}

void SessionController::setWidth(qint32 width)
{
    m_session->m_config->m_width=width;
}

#pragma endregion}


