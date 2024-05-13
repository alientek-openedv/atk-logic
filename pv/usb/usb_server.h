/* Copyright(c),广州星翼电子科技有限公司
* All rights reserved.*
* 文件名称：usbServer.h
* 摘    要：usb协议服务层
* 当前版本：1.0
* 作    者：J(俊杰)
* 完成日期：2022年7月14日
*/

#pragma once

#include "usb_control.h"
#include <QList>
#include <QString>
#include <QMap>

class USBServer
{
public:
    USBServer(qint32 channelCount);
    ~USBServer();
    USBControl* IdGetUSB(QString windowsID);
    int IdGetPort(QString windowsID);
    USBControl* PortGetUSB(int port);
    int GetNewDevice(QList<quint8> connectPort);
    int GetDeleteDevice();
    void BindingPort(int port, QString windowsID);
    void DeleteDevice(QString windowsID);
    void DeleteDevice(int port);
    QList<USBControl*> GetAllDevice(bool containError=false);
    int NewDevice();
    qint32 DeviceToFile(QString windowsID);

private:
    qint32 GetNewFilePort();

public:
    qint32 m_channelCount;
    QMap<QString, int> m_usbPort;			/* 窗口ID，port */
    QMap<int, USBControl*> m_usbControl;	/* port，USB控制层 */
    QList<QString> m_delete;
    qint32 m_errorPort=-1;                  /* 连接失败的设备 */
    qint32 m_lastError=0;                   /* 最后连接错误 */
};
