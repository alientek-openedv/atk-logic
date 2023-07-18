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
    int GetNewDevice();
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
