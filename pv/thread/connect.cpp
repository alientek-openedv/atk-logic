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

#include "connect.h"

ConnectDevice::ConnectDevice(QObject *parent)
    : QObject{parent}
{
    m_thread.start();
    this->moveToThread(&m_thread);
    m_netManger = new QNetworkAccessManager();
    m_netManger->moveToThread(&m_thread);
    QObject::connect(m_netManger, &QNetworkAccessManager::finished, this, &ConnectDevice::finishedSlot);
}

ConnectDevice::~ConnectDevice()
{
    m_thread.requestInterruption();
    m_thread.quit();
    m_thread.wait();
    disconnect();
}

void ConnectDevice::CheckDeviceCreanInfo(USBControl *usb, qint32 port)
{
    LogHelp::write(QString("    连接设备端口:%1").arg(QString::number(port)));
    emit SendConnectSchedule(0,0);
    QString usbName="NULL",name="";
    qint32 count=0,mcuVersions=0,fpgaVersions=0,minVersion=0,level=0;
    bool isActive=false;
    qint32 maxCount=5;
    qint32 schedule=100/maxCount;
    if(g_updataState==1)
    {
        LogHelp::write(QString("    更新等待:2500ms"));
        QThread::msleep(2500);
        g_updataState=0;
    }
    QThread::msleep(300);
    do{
        Data_* data2 = new Data_();
        usb->GetMCUVersion();
        while(usb->ReadSynchronous(data2))
            delete[] data2->buf;
        LogHelp::write(QString("    第%1次尝试连接").arg(QString::number(count+1)));
        LogHelp::write(QString("    获取MCU信息.."));
        if(mcuVersions==0&&usb->GetMCUVersion() && usb->ReadSynchronous(data2) && data2->len>0){
            if(data2->buf[0]==0x0a && data2->buf[1]==0x81 && data2->buf[2]==0x01){
                if(data2->buf[3]==0x61)
                {
                    level=data2->buf[8];
                    mcuVersions=data2->buf[4]*10+data2->buf[5];
                    usb->SetResetState(1);
                    if(data2->buf)
                    {
                        delete[] data2->buf;
                        data2->buf=nullptr;
                    }
                    while(usb->ReadSynchronous(data2))
                        delete[] data2->buf;
                    isActive=true;
                }
                else if(data2->buf[3]==0x62)
                {
                    delete[] data2->buf;
                    delete data2;
                    emit EnterBootloader(port);
                    return;
                }
            }
        }else
            LogHelp::write(QString("    获取MCU信息失败"));
        if(isActive){
            if(data2->buf)
            {
                delete[] data2->buf;
                data2->buf=nullptr;
            }
            LogHelp::write(QString("    获取FPGA信息.."));
            usb->GetDeviceData(false);
            if(usb->ReadSynchronous(data2, 2048)){
                Analysis analusis(data2->buf,data2->len);
                AnalysisData data=analusis.getNextData();
                while(data.order!=-1){
                    if(data.order==2){
                        if(*(data.pData+2)!=1){
                            delete[] data2->buf;
                            delete data2;
                            LogHelp::write(QString("    FPGA状态异常"));
                            emit SendDeviceCreanInfo("","",port,0,0,0,1);
                            return;
                        }
                        if(*(data.pData+3)==2)
                            usbName="2.0";
                        else if(*(data.pData+3)==3)
                            usbName="3.0";
                        fpgaVersions=(*(data.pData+5))*100+(*(data.pData+6));
                        minVersion=(*(data.pData+7))*100+(*(data.pData+8));
                        if(FPGA_MIN_VERSION_NUM>fpgaVersions)
                        {
                            delete[] data2->buf;
                            delete data2;
                            emit SendDeviceCreanInfo("","",port,0,0,0,6);
                            return;
                        }else if(APP_VERSION_NUM<minVersion){
                            delete[] data2->buf;
                            delete data2;
                            emit SendDeviceCreanInfo("","",port,0,0,0,7);
                            return;
                        }
                        QByteArray array((const char *)(data.pData+9),data.ullLen-9);
                        name = QString::fromLocal8Bit(array);
                        break;
                    }
                    data=analusis.getNextData();
                }
            }else
                LogHelp::write(QString("    获取FPGA信息失败"));
        }
        if(data2->buf)
            delete[] data2->buf;
        delete data2;
        QThread::msleep(300);
        count++;
        if(count>maxCount || usb->m_NoDevice)
            break;
        emit SendConnectSchedule(qMin(count*schedule,99),0);
    }while(name=="" || usbName=="NULL" || mcuVersions==0 || fpgaVersions==0);
    usb->m_busy=false;
    if(count>maxCount){
        LogHelp::write(QString("    设备连接异常"));
        emit SendDeviceCreanInfo("","",port,0,0,0,2);
        return;
    }
    if(usb->m_NoDevice){
        LogHelp::write(QString("    设备断开"));
        emit SendDeviceCreanInfo("","",port,0,0,0,3);
        return;
    }else{
        usb->CloseAllPWM();
        QThread::msleep(30);
        usb->SetResetState(0);
        Data_* data2 = new Data_();
        usb->ReadSynchronous(data2);
        if(data2->buf)
        {
            delete[] data2->buf;
            data2->buf=nullptr;
        }
        delete data2;
    }
    LogHelp::write(QString("    设备连接成功:%1,%2,MFW:%3,FFW:%4").arg(name+(level==1?" Plus":""),QString::number(port),
                                                                 QString::number(mcuVersions),QString::number(fpgaVersions)));
    emit SendDeviceCreanInfo(name,usbName,port,mcuVersions,fpgaVersions,level,0);
}

void ConnectDevice::CheckUpdate(int version, bool isShowError)
{
    LogHelp::write(QString("检查软件更新"));
    while(!m_updateMutex.tryLock(10))
        QApplication::processEvents(QEventLoop::AllEvents, 100);
    m_MCUVersion=version;
    m_isShowError=isShowError;
    m_type=CheckType::Application;
    m_netManger->get(QNetworkRequest(QUrl("")));
}

void ConnectDevice::CheckHardwareUpdate(int MCUVersion, int FPGAVersion)
{
    LogHelp::write(QString("检查硬件更新"));
    while(!m_updateMutex.tryLock(10))
        QApplication::processEvents(QEventLoop::AllEvents, 100);
    m_MCUVersion=MCUVersion;
    m_FPGAVersion=FPGAVersion;
    m_type=CheckType::Hardware;
    m_netManger->get(QNetworkRequest(QUrl("")));
}

void ConnectDevice::finishedSlot(QNetworkReply *reply)
{
    QJsonObject json;
    if (reply->error() == QNetworkReply::NoError)
    {
        QString string = QString(reply->readAll()),url;
        QJsonArray version,date,text;
        QRegularExpression reA("##version##([0-9\\;]+)##([0-9\\.]+)##([0-9\\.]+)##\n##info##\n((?:.|\n)*?)\n##\n##url##\n((?:.|\n)*?)\n##urlend##");
        QRegularExpressionMatchIterator i = reA.globalMatch(string);
        int isUpdate=0;
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            if (match.hasMatch()) {
                QString str=match.captured(1);
                if(str.contains(";"))
                {
                    QStringList list=str.split(";");
                    if(list.count()!=2)
                        continue;
                    if(list[0].toInt()>=m_MCUVersion && list[1].toInt()>=m_FPGAVersion){
                        version.append(match.captured(2));
                        date.append(match.captured(3));
                        text.append(match.captured(4).replace("\n","\r\n"));
                        if(url.isEmpty())
                            url=match.captured(5);
                        if(list[0].toInt()>m_MCUVersion || list[1].toInt()>m_FPGAVersion)
                            isUpdate=1;
                    }
                }else{
                    if(str.toInt()>=m_MCUVersion){
                        version.append(match.captured(2));
                        date.append(match.captured(3));
                        text.append(match.captured(4).replace("\n","\r\n"));
                        if(url.isEmpty())
                            url=match.captured(5);
                        if(match.captured(1).toInt()>m_MCUVersion)
                            isUpdate=1;
                    }
                }
            }
        }
        json["version"]=version;
        json["date"]=date;
        json["text"]=text;
        json["count"]=version.count();
        switch (m_type) {
        case CheckType::Application:
            LogHelp::write(QString("检查软件更新完成"));
            if(!m_isShowError)
            {
                if(isUpdate==1)
                    emit SendCheckUpdateDate(3, url, json);
            }
            else
                emit SendCheckUpdateDate(isUpdate, url, json);
            break;
        case CheckType::Hardware:
            LogHelp::write(QString("检查硬件更新完成"));
            emit SendHardwareCheckUpdateDate(isUpdate, url, json);
            break;
        }
    }
    else
    {
        LogHelp::write(QString("检查更新错误:%1").arg(reply->errorString()));
        switch (m_type) {
        case CheckType::Application:
            if(m_isShowError)
                emit SendCheckUpdateDate(2, "", json);
            break;
        case CheckType::Hardware:
            emit SendHardwareCheckUpdateDate(2, "", json);
            break;
        }
    }
    reply->deleteLater();
    m_updateMutex.unlock();
}
