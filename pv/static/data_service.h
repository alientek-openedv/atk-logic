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

#ifndef DATASERVICE_H
#define DATASERVICE_H


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
    qint32 m_channelCount=16;
    QApplication* m_app=nullptr;
    QQmlEngine* m_engine=nullptr;

private:
    DataService();

private:
    QHash<QString,Session*> m_session_list;
    QHash<QString,SessionError*> m_sessionError_list;
    WindowError* m_windowError;
    QObject* m_root;
};

#endif // DATASERVICE_H
