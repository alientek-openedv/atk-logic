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
#include "qdebug.h"
#include "qmath.h"

#ifdef QT_NO_DEBUG
#define Debug(str)
#else
#define Debug(str) (qDebug() << str)
#endif

#define PRODUCT_ID 0xffcc
#define VENDOR_ID 0x1a86
#define LEN(addr) sizeof(addr)/sizeof(addr[0])

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
bool zipDir(QString& dirPath, QString savePath);
bool QZipWriterEx(QZipWriter *writer, QString tmpPath, QString basePath);
bool unZipDir(QString& filePath, QString savePath);
QString JsonToString(const QJsonObject& json);
QJsonObject StringToJson(const QString& str);
QJsonArray StringToJsonArray(const QString& str);
qint64 GetTickCount_();
void pathRepair(QString &path);
void deleteEmptyDir(QString path);


#endif // UTIL_H
