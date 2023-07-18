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

#ifndef THREAD_WORK_H
#define THREAD_WORK_H

class ThreadRead;

#include <QFile>
#include <QApplication>
#include <QDataStream>
#include <QJsonArray>
#include "pv/controller/session_error.h"
#include <pv/data/session_config.h>
#include "pv/data/analysis.h"
#include "pv/usb/usb_control.h"
#include "pv/data/session.h"
#include "pv/data/segment.h"
#include "thread_read.h"
#include "qtextcodec.h"
#include <pv/static/log_help.h>

struct MeasureDataStr;
struct MeasureData;
struct VernierData;
class ThreadWork : public QObject
{
    Q_OBJECT
public:
    explicit ThreadWork(QObject *parent = nullptr);
    ~ThreadWork();

    void DeviceRecvThread(USBControl* usb, Segment* segment, SessionConfig* config,
                          double second, void* channelIDList, bool isBuffer, bool isRLE);
    void SaveFileThreadPara(QString filePath, Segment* segment, QVector<QString> channelsName);
    void SaveSourceFileThreadPara(QString filePath, QString sessionName, Segment* segment,
                                  QVector<QString> channelsName, QVector<MeasureData>* measureList, QVector<VernierData>* vernierList,
                                  QHash<QString,DecodeController*>* decodes);
    void LoadSourceFileThreadPara(QString filePath, Segment* segment, SessionConfig* config,
                                  QVector<MeasureData>* measureList, QVector<VernierData>* vernierList, QHash<QString,DecodeController*>* decodes,
                                  QVector<QString>* channelsNames);
    void GlitchRemovalThreadPara(Segment* segment);
    void SaveDecodeTableThreadPara(QString filePath, qint32 multiply, QVector<DecodeRowData>* data, QVector<QMap<QString,QJsonObject>>* rowType, QVector<bool>* saveList);
    void calcMeasureData(qint32 measureID, qint32 channelID, QVector<MeasureData>* measure, Segment* segment);
    void destroy();
    void stop(bool waitClose=false);
    void onDataIn(Data_* data);
    void setLiveFollowing(bool isEnable);
    void initError(SessionError* sessionError);

private:
    void calcFRing(qint64 start,qint64 end,MeasureData* data, qint32 channelID, Segment* segment);
    void setRun(qint8 state);
    bool isRun();

signals:
    void SendReadThreadStart(USBControl* usb, qint32 maxTransferSize, ThreadWork* parent);
    void SendDeviceRecvSchedule(qint32 schedule, qint32 type, qint32 state_);
    void sendVernierTriggerPosition(quint64 position);
    void timerDrawUpdate(qint64 showStart);
    void saveSessionSettings(QString path);
    void loadSessionSettings(QString name, QString path);
    void channelNameModification(qint32 channelID, QString name);
    void appendMeasure(qint32 measureIndex);
    void appendVernier(qint32 vernierIndex);
    void appendDecode(QString json, bool end);
    void calcMeasureDataComplete(qint32 measureID);
    void sendGlitchString(QString str);

private:
    QThread m_thread;
    QMutex m_dataMutex;
    QVector<Data_*> m_dataList;
    Data_* m_dataBuffer = nullptr;
    SessionError* m_sessionError;
    bool m_liveFollowing=true;
    QMutex m_lock;
    qint8 m_run=0;
};

#endif // THREAD_WORK_H
