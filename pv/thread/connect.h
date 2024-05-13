#ifndef CONNECT_H
#define CONNECT_H

#include <QApplication>
#include <QMutex>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "pv/static/data_service.h"
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
    void CheckHardwareUpdate(int MCUVersion, int FPGAVersion, int deviceVersion);

signals:
    void SendDeviceCreanInfo(QString name, QString usbName, qint32 port, qint32 mcuVersions, qint32 fpgaVersions, qint16 deviceVersion, qint32 level, qint8 state);
    void SendConnectSchedule(qint32 schedule, qint8 state);
    void SendCheckUpdateDate(int state, QString url, QJsonObject json);//state:0=无更新、1=有更新、2=获取错误、3=升级标记
    void SendHardwareCheckUpdateDate(int state, QString url, QJsonObject json);//state:0=无更新、1=有更新、2=获取错误、3=等待连接设备、4=获取固件版本失败
    void EnterBootloader(qint32 port, bool errorModel);

private slots:
    void finishedSlot(QNetworkReply* reply);

private:
    QThread m_thread;
    QNetworkAccessManager *m_netManger;
    CheckType m_type;
    QMutex m_updateMutex;
    int m_MCUVersion;
    int m_FPGAVersion;
    int m_deviceVersion;
    bool m_isShowError;
};

#endif // CONNECT_H
