﻿/**
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

#include "thread_download.h"

int g_updataState=0;

ThreadDownload::ThreadDownload(USBServer* usbServer, USBControl* usb, QObject *parent)
    : QObject{parent}, m_usbServer(usbServer), m_usb(usb)
{
    m_thread.start();
    this->moveToThread(&m_thread);
    m_path=QDir::tempPath()+"/ATK-Logic/download/";
    m_mcuURL="";
    m_fpgaURL="";
    m_netWorkManager=new QNetworkAccessManager();
    m_netWorkManager->moveToThread(&m_thread);
}

ThreadDownload::~ThreadDownload()
{
    m_thread.requestInterruption();
    m_thread.quit();
    m_thread.wait();
    disconnect();
}

void ThreadDownload::startDownload(qint32 index)
{
    m_index=index;
    switch(index){
    case 0:
        LogHelp::write(QString("    启动FPGA固件下载"));
        m_url=m_fpgaURL;
        break;
    case 1:
        LogHelp::write(QString("    启动MCU固件下载"));
        m_url=m_mcuURL;
    }
    if(m_url.isEmpty()) return;
    if(!m_IsDownloading)
    {
        m_IsDownloading=true;
        
        m_fileName=m_url.split("/").last();
        
        QNetworkRequest request;
        QUrl url = QUrl(m_url);
        request.setUrl(url);
        m_reply = m_netWorkManager->head(request);
        m_state = requestHead;
        connect(m_reply,SIGNAL(finished()),this,SLOT(onFinishedRequest()));
        connect(m_reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(onError(QNetworkReply::NetworkError)));
    }else
        emit SendDownloadSchedule(100,4,m_index);
}

void ThreadDownload::startUpdate()
{
    startUpdate(m_fpgaURL,m_mcuURL);
}

void ThreadDownload::startUpdate(QString fpgaURL, QString mcuURL)
{
    LogHelp::write(QString("启动硬件更新"));
    m_index=0;
    emit SendDownloadSchedule(0,2,m_index);
    m_mcuURL=mcuURL;
    m_fpgaURL=fpgaURL;
    if(!m_usb->EnterBootloader())
    {
        LogHelp::write(QString("EnterBootloader Error"));
        emit SendDownloadSchedule(100,4,m_index);
    }
}

void ThreadDownload::startDownloadFirmware()
{
    LogHelp::write(QString("    开始升级FPGA"));
    m_index=0;
    m_fileName=m_mcuURL.split("/").last();
    if(!updateData(true,0))
        goto error;
    LogHelp::write(QString("    开始升级MCU"));
    m_index=1;
    m_fileName=m_fpgaURL.split("/").last();
    if(!updateData(false,50))
        goto error;
    if(!m_usb->RestartMCU())
        goto error;
    m_usb->m_busy=false;
    m_usbServer->DeleteDevice(m_port);
    emit SendDownloadSchedule(100,2,m_index);
    return;
error:
    LogHelp::write(QString("    升级失败"));
    m_usb->m_busy=false;
    g_updataState=0;
    m_usbServer->DeleteDevice(m_port);
    emit SendDownloadSchedule(100,4,m_index);
}

void ThreadDownload::stopDownLoad()
{
    if(m_reply == nullptr) return;

    disconnect(m_reply,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
    disconnect(m_reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(onError(QNetworkReply::NetworkError)));
    disconnect(m_reply,SIGNAL(finished()),this,SLOT(onFinishedRequest()));
    disconnect(m_reply,&QNetworkReply::downloadProgress,this,&ThreadDownload::onDownloadProgress);
    m_reply->abort();
    m_reply->deleteLater();
    m_file.close();
    m_IsDownloading = false;
    emit SendDownloadSchedule(100,1,m_index);
}

bool ThreadDownload::updateData(bool isMCU, qint32 scheduleStart)
{
    QFile file(m_path+m_fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        LogHelp::write(QString("OpenFile Error"));
        emit SendDownloadSchedule(100,4,0);
        return false;
    }
    QByteArray data = file.readAll();
    file.close();

    const uint16_t pack_size = 256;
    const unsigned pack_count = data.size()/pack_size;
    const uint16_t remain = data.size()%pack_size;
    const uint8_t *p = reinterpret_cast<const uint8_t *>(data.constData());

    emit SendDownloadSchedule(scheduleStart,2,m_index);
    qint32 schedule=0;
    LogHelp::write(QString("    %1进入Bootloader").arg(isMCU?"MCU":"FPGA"));
    if(!m_usb->EnterHardwareUpdate(isMCU))
        goto error;
    
    LogHelp::write(QString("    开始升级"));
    for(unsigned i = 0; i < pack_count; i++){
        if(!m_usb->SendUpdateData(p, pack_size, isMCU))
            goto error;
        p += pack_size;
        qint32 temp=scheduleStart+qMin(i/(qreal)pack_count*45, 45.);
        if(schedule!=temp)
        {
            schedule=temp;
            emit SendDownloadSchedule(schedule,2,m_index);
        }
    }
    emit SendDownloadSchedule(scheduleStart+45,2,m_index);
    
    if(!m_usb->SendUpdateData(p, remain, isMCU, true))
        goto error;
    LogHelp::write(QString("    升级完成"));
    return true;
error:
    LogHelp::write(QString("    升级错误，下发数据失败"));
    emit SendDownloadSchedule(100,4,m_index);
    return false;
}

void ThreadDownload::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if(bytesReceived==bytesTotal){
        m_state = requestComplete;
        LogHelp::write(QString("    固件下载完成"));
        stopDownLoad();
    }
}

void ThreadDownload::onFinishedRequest()
{
    if(m_reply==nullptr) return;
    if(m_state == requestHead)
    {
        m_fileSize=0;
        QNetworkRequest request;
        request.setUrl(QUrl(m_url));
        QEventLoop *loop = new QEventLoop;
        connect(m_netWorkManager, SIGNAL(finished(QNetworkReply*)), loop, SLOT(quit()));
        m_reply = m_netWorkManager->get(request);
        loop->exec();

        
        m_statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        m_state = requestBody;
    }
    QNetworkRequest request;
    if(m_statusCode==200)
        request.setUrl(QUrl(m_url));
    else if(m_statusCode == 302)    
    {
        
        QUrl realUrl = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        request.setUrl(realUrl);
    }
    else if(m_statusCode == 301)
    {
        QUrl realUrl = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        m_reply->deleteLater();
        request.setUrl(realUrl);
    }
    else
        return;
    disconnect(m_reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(onError(QNetworkReply::NetworkError)));
    disconnect(m_reply,SIGNAL(finished()),this,SLOT(onFinishedRequest()));
    m_reply->abort();
    m_reply->deleteLater();
    m_reply = m_netWorkManager->get(request);
    connect(m_reply,SIGNAL(finished()),this,SLOT(onFinishedRequest()));
    connect(m_reply,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
    connect(m_reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(onError(QNetworkReply::NetworkError)));
    connect(m_reply,&QNetworkReply::downloadProgress,this,&ThreadDownload::onDownloadProgress);
    LogHelp::write(QString("    取得服务器连接，开始接收数据"));
}

void ThreadDownload::onReadyRead()
{
    if(m_reply==nullptr) return;

    if(!m_file.isOpen())
    {
        QDir downLoadDir(m_path);
        if(!downLoadDir.exists())
        {
            downLoadDir.mkdir(m_path);
        }
        m_file.setFileName(m_path + m_fileName);
        m_file.open(QIODevice::Truncate|QIODevice::WriteOnly);
    }

    m_file.write(m_reply->readAll());

    m_downLoadedBytes =m_file.size();
    if(m_file.size() == m_fileSize)
    {
        m_state = requestComplete;
        stopDownLoad();
    }
}

void ThreadDownload::onError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);
    if(m_reply==nullptr) return;
    LogHelp::write(QString("    访问服务器出错:%1").arg(m_reply->errorString()));
    emit SendDownloadSchedule(100,4,m_index);
}

void ThreadDownload::onEnterBootloader(qint32 port)
{
    g_updataState=1;
    m_port=port;
    m_usb=m_usbServer->PortGetUSB(port);
    startDownload(0);
}
