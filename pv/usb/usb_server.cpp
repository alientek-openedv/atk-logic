#include "usb_server.h"

USBServer::USBServer(qint32 channelCount) : m_channelCount(channelCount){}

USBServer::~USBServer() {
    QMap<int, USBControl*>::const_iterator it = m_usbControl.constBegin();
    while (it != m_usbControl.constEnd())
    {
        delete it.value();
        ++it;
    }
    m_usbControl.clear();
}

USBControl* USBServer::IdGetUSB(QString windowsID) {
    QMap<QString, int>::const_iterator it = m_usbPort.constBegin();
    while (it != m_usbPort.constEnd())
    {
        if (windowsID == it.key()) {
            return PortGetUSB(it.value());
        }
        ++it;
    }
    return nullptr;
}

int USBServer::IdGetPort(QString windowsID)
{
    QMap<QString, int>::const_iterator it = m_usbPort.constBegin();
    while (it != m_usbPort.constEnd())
    {
        if (windowsID == it.key()) {
            return it.value();
        }
        ++it;
    }
    return -1;
}

USBControl* USBServer::PortGetUSB(int port) {
    QMap<int, USBControl*>::const_iterator it = m_usbControl.constBegin();
    while (it != m_usbControl.constEnd())
    {
        if (port == it.key()) {
            return it.value();
        }
        ++it;
    }
    return nullptr;
}

int USBServer::GetNewDevice(QList<quint8> connectPort) {
    USBControl* usb = new USBControl();
    QList<FindData> data = usb->Find();
    int errorDevice=0;
    int index = -1;
    int port = -1;

    //寻找未初始化的设备
    for (int i = 0; i < data.size(); i++) {
        bool isAdd = true;
        QMap<int, USBControl*>::const_iterator it = m_usbControl.constBegin();
        while (it != m_usbControl.constEnd())
        {
            if (data[i].port == it.key())
            {
                isAdd = false;
                break;
            }
            ++it;
        }
        if (isAdd && !connectPort.contains(data[i].port)) {
            if (usb->Init(data[i].dev, data[i].context, m_channelCount, data[i].port)) {
                m_usbControl.insert(data[i].port, usb);
                port = data[i].port;
                index = i;
                break;
            }else
                errorDevice++;
        }
    }
    if(errorDevice==data.size() && index==-1)
        m_lastError=usb->m_lastError;

    //删除多余的设备
    for (int i = 0; i < data.size(); i++) {
        if (i != index)
            usb->Find_Free(data[i].dev);
    }
    if (index == -1)
        delete usb;
    return port;
}

int USBServer::GetDeleteDevice()
{
    QList<FindData> data = USBControl::Find();
    int ret=-1;//返回连接中被删除的设备
    //判断错误设备是否存在
    if(m_errorPort!=-1){
        bool errorDeviceExist=false;
        for (int i = 0; i < data.size(); i++) {
            if(data[i].port == m_errorPort)
            {
                errorDeviceExist=true;
                break;
            }
        }
        if(!errorDeviceExist)
        {
            DeleteDevice(m_errorPort);
            m_errorPort=-1;
        }
    }
    //寻找已删除设备
    m_delete.clear();
    for (auto iter = m_usbPort.begin(); iter != m_usbPort.end(); iter++) {
        bool isExist = false;
        for (int i = 0; i < data.size(); i++) {
            if (data[i].port == iter.value()) {
                isExist = true;
                break;
            }
        }
        if (!isExist && iter.value() < 999)
            m_delete.push_back(iter.key());
    }

    if(m_delete.count()==0){
        for (auto iter = m_usbControl.begin(); iter != m_usbControl.end(); iter++) {
            bool isExist = false;
            for (int i = 0; i < data.size(); i++) {
                if (data[i].port == iter.key()) {
                    isExist = true;
                    break;
                }
            }
            if (!isExist)
            {
                ret=iter.key();
                break;
            }

        }
    }

    //删除多余的设备
    for (int i = 0; i < data.size(); i++)
        USBControl::Find_Free(data[i].dev);
    return ret;
}

int USBServer::NewDevice() {
    int newPort = GetNewFilePort();
    USBControl* usb = new USBControl();
    m_usbControl.insert(newPort, usb);
    return newPort;
}

qint32 USBServer::DeviceToFile(QString windowsID)
{
    int newPort = GetNewFilePort();
    DeleteDevice(windowsID);
    USBControl* usb = new USBControl();
    m_usbControl.insert(newPort, usb);
    m_usbPort.insert(windowsID, newPort);
    return newPort;
}

qint32 USBServer::GetNewFilePort()
{
    int newPort = 1000;
    bool isSet;
    do
    {
        isSet = true;
        for (auto iter = m_usbControl.begin(); iter != m_usbControl.end(); iter++) {
            if (newPort == iter.key()) {
                isSet = false;
                newPort++;
                break;
            }
        }
    } while (!isSet);
    return newPort;
}

void USBServer::DeleteDevice(QString windowsID) {

    delete m_usbControl[m_usbPort[windowsID]];
    m_usbControl.erase(m_usbControl.find(m_usbPort[windowsID]));
    m_usbPort.erase(m_usbPort.find(windowsID));
}

void USBServer::DeleteDevice(int port)
{
    delete m_usbControl[port];
    m_usbControl.erase(m_usbControl.find(port));
}

QList<USBControl *> USBServer::GetAllDevice(bool containError)
{
    QList<USBControl *> ret;
    for (auto iter = m_usbControl.begin(); iter != m_usbControl.end(); iter++) {
        bool isAppend=false;
        if((containError && m_errorPort==-1) || !containError)
            isAppend=true;
        else
        {
            if(iter.key()!=m_errorPort)
                isAppend=true;
        }
        if(isAppend && iter.key()<1000)
            ret.append(iter.value());
    }
    return ret;
}

void USBServer::BindingPort(int port, QString windowsID) {
    if(m_usbPort.contains(windowsID))
        m_usbPort[windowsID]=port;
    else
        m_usbPort.insert(windowsID, port);
}
