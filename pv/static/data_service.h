#ifndef DATASERVICE_H
#define DATASERVICE_H


#include "pv/static/shared_memory_helper.h"
#pragma once
#include <QHash>
#include <QString>
#include <QVector>
#include <QApplication>
#include <QQmlApplicationEngine>
#include "util.h"
#include "pv/controller/session_error.h"
#include "pv/static/window_error.h"
#include "pv/data/session.h"
#include "pv/usb/usb_server.h"
#include "decode_service.h"

enum class SessionType;
class Session;
class FramelessWindow;
class DataService
{
public:
    static DataService* getInstance();

    Session* newSession(QString name, QString id, qint32 channelCount, SessionType type, QString path);
    QString getNewSessionID();
    Session* getSession(QString id);

    void removeSession(QString id);

    SessionError* getSessionError(QString id);
    void setSessionError(QString id, SessionError* error);

    WindowError* getWindowError();
    void setWindowError(WindowError* error);

    void setRoot(QObject* root);
    QObject* getRoot();

    USBServer* getUSBServer();
    DecodeService* getDecodeServer();

public:
    qint32 m_channelCount=16;//全局通道数
    QApplication* m_app=nullptr;//app全局
    QQmlEngine* m_engine=nullptr;//全局窗口
    SharedMemoryHelper m_shared;//全局共享内存
    QVariantList m_log;         //全局日志列表
    QMutex m_logLock;           //全局日志锁
    QString m_tempDir;          //全局写出目录
    QString m_downloadPath="http://47.111.11.73/ATK-Prod/ATK-Logic";//全局固件下载地址

private:
    DataService();

private:
    QHash<QString,Session*> m_session_list;
    QHash<QString,SessionError*> m_sessionError_list;
    WindowError* m_windowError;
    QObject* m_root;
};

#endif // DATASERVICE_H
