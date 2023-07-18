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

#ifndef THREAD_DOWNLOAD_H
#define THREAD_DOWNLOAD_H

#include <QObject>
#include <QCoreApplication>
#include <QThread>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QEventLoop>
#include <pv/usb/usb_server.h>
#include <pv/static/log_help.h>

extern int g_updataState;

enum Status
{
    requestHead=0,
    requestBody,
    requestComplete
};

class ThreadDownload : public QObject
{
    Q_OBJECT
public:
    explicit ThreadDownload(USBServer* usbServer, USBControl* usb, QObject *parent = nullptr);
    ~ThreadDownload();
    void startDownload(qint32 index);
    void startUpdate();
    void startUpdate(QString fpgaURL, QString mcuURL);
    void startDownloadFirmware();
    void stopDownLoad();

private:
    bool updateData(bool isMCU, qint32 scheduleStart);

signals:
    void SendDownloadSchedule(qint32 schedule, qint32 type, qint32 index);

public slots:
    void onEnterBootloader(qint32 port);

private slots:
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onFinishedRequest();
    void onReadyRead();
    void onError(QNetworkReply::NetworkError error);

public:
    USBControl* m_usb;

private:
    QThread m_thread;
    QNetworkAccessManager *m_netWorkManager;
    QNetworkReply *m_reply;
    USBServer* m_usbServer;
    qint32 m_port;
    qint32 m_index;

    QString m_mcuURL;
    QString m_fpgaURL;

    qint64 m_fileSize;
    int m_statusCode=0;
    qint64 m_downLoadedBytes;
    qint64 m_currentLoadedBytes;

    QString m_fileName;
    QString m_url;
    Status m_state;

    QString m_path;
    bool m_IsDownloading=false;
    QFile m_file;
};

#endif // THREAD_DOWNLOAD_H
