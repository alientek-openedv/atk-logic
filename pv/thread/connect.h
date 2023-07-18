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

#ifndef CONNECT_H
#define CONNECT_H

#include <QApplication>
#include <QMutex>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include "pv/usb/usb_control.h"
#include "pv/thread/thread_download.h"

enum CheckType{
    Application,
    Hardware
};

class ConnectDevice : public QObject
{
    Q_OBJECT
public:
    explicit ConnectDevice(QObject *parent = nullptr);
    ~ConnectDevice();
    void CheckDeviceCreanInfo(USBControl* usb, qint32 port);
    void CheckUpdate(int version, bool m_isShowError);
    void CheckHardwareUpdate(int MCUVersion, int FPGAVersion);

signals:
    void SendDeviceCreanInfo(QString name, QString usbName, qint32 port, qint32 mcuVersions, qint32 fpgaVersions, qint32 level, qint8 state);
    void SendConnectSchedule(qint32 schedule, qint8 state);
    void SendCheckUpdateDate(int state, QString url, QJsonObject json);
    void SendHardwareCheckUpdateDate(int state, QString url, QJsonObject json);
    void EnterBootloader(qint32 port);

private slots:
    void finishedSlot(QNetworkReply* reply);

private:
    QThread m_thread;
    QNetworkAccessManager *m_netManger;
    CheckType m_type;
    QMutex m_updateMutex;
    int m_MCUVersion;
    int m_FPGAVersion;
    bool m_isShowError;
};

#endif // CONNECT_H
