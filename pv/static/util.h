#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QRandomGenerator>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QtGui/private/qzipreader_p.h>
#include <QtGui/private/qzipwriter_p.h>
#include <pv/static/log_help.h>
#include "qdebug.h"
#include "qmath.h"
#ifdef Q_OS_LINUX
#include <sys/sysinfo.h>
#endif

#ifdef QT_NO_DEBUG
#define Debug(str)
#else
#define Debug(str) (qDebug() << str)
#endif

#define PRODUCT_ID 0xffcc
#define VENDOR_ID 0x1a86
#define LEN(addr) sizeof(addr)/sizeof(addr[0])

extern bool g_isLinuxMemoryLimit;

QString getRandString(qint32 len);
qint64 frequencyToHz(double num, QString unit);
qint64 unitToBit(double num, QString unit);
QString getStringMiddle(QString source, QString before, QString after, int offset=0);
quint32 gCRC32(quint8* p, qint32 len);
QByteArray intToBytes(quint64 value, qint8 len=4);
QByteArray triggerStringToByte(QJsonArray channelArray,QString text);
QString nsToShowStr(double time,int decimal=6);
QString hzToShowStr(double hz, int decimal=6);
QString bitToShowStr(double bit, int decimal=6);
QString moveDecimal(quint64 num, int decimal);
QString reverseString(QString str);
QString JsonToString(const QJsonObject& json);
QJsonObject StringToJson(const QString& str);
QJsonArray StringToJsonArray(const QString& str);
qint64 GetTickCount_();
void GetDirFilesCount(const QString& path, qint32& count);
void pathRepair(QString &path);
void deleteEmptyDir(QString path);
void* malloc_(size_t len);
void showHex(quint8* data,int len);

#endif // UTIL_H
