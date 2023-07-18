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

#include "data_service.h"


static DataService* m_dataService=nullptr;
static USBServer* m_USBServer=nullptr;
static DecodeService* m_DecodeServer=nullptr;

DataService::DataService(){
    if(m_USBServer==nullptr)
        m_USBServer = new USBServer(m_channelCount);
    if(m_DecodeServer==nullptr)
        m_DecodeServer = new DecodeService();
}

DataService *DataService::getInstance()
{
    if(m_dataService==nullptr)
        m_dataService=new DataService();
    return m_dataService;
}

Session *DataService::newSession(QString name, QString id, qint32 channelCount, SessionType type, QString path)
{
    Session* newSession=new Session(name,id,channelCount,type,path,this);
    m_session_list[id] = newSession;
    return newSession;
}

QString DataService::getNewSessionID()
{
    QString id="";
    do{
        id= getRandString(16);
        if(m_session_list.isEmpty() || !m_session_list.contains(id))
            break;
    }while(1);
    return id;
}

Session *DataService::getSession(QString id)
{
    return m_session_list[id];
}

void DataService::removeSession(QString id)
{
    delete m_session_list[id];
    m_session_list.remove(id);
    m_USBServer->DeleteDevice(id);
}

SessionError *DataService::getSessionError(QString id)
{
    return m_sessionError_list[id];
}

void DataService::setSessionError(QString id, SessionError *error)
{
    m_sessionError_list[id]=error;
}

WindowError *DataService::getWindowError()
{
    return m_windowError;
}

void DataService::setWindowError(WindowError *error)
{
    m_windowError=error;
}

void DataService::setRoot(QObject *root)
{
    m_root=root;
}

QObject *DataService::getRoot()
{
    return m_root;
}

USBServer *DataService::getUSBServer()
{
    return m_USBServer;
}

DecodeService *DataService::getDecodeServer()
{
    return m_DecodeServer;
}


