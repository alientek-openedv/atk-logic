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

#ifndef SESSION_H
#define SESSION_H

#pragma once
#include <QObject>
#include <QString>
#include <QFile>
#include <QColor>
#include "session_config.h"
#include "segment.h"
#include "decode_controller.h"
#include "pv/thread/thread_work.h"
#include "pv/static/data_service.h"
#include "pv/usb/usb_control.h"

class ThreadWork;
class DataService;
enum class SessionType{
    Demo=0,
    Device=1,
    File=2
};

struct MeasureDataStr
{
    QString start;
    QString startDetail;
    QString end;
    QString endDetail;
    QString time;
    QString timeDetail;
    QString minFreq;
    QString minFreqDetail;
    QString maxFreq;
    QString maxFreqDetail;
};

struct MeasureData
{
    qint32 id;
    QString name;
    qint32 channelID;
    qint64 start;
    qint64 end;
    qint32 rising;
    qint32 falling;
    QColor dataColor;
    MeasureDataStr str;
    QString note;
    qint32 offset;
    bool isAutoOffset=true;
    bool isSelect;
    bool isCalc=false;
    bool isRefreshCalc=false;
};

struct VernierDataBusy{
    QString id;
    qint32 flag;
    QString str;
};

struct VernierData
{
    QString name;
    qint32 id;
    QColor dataColor;
    QString positionStr;
    qint64 position;
    QString note;
    bool visible=true;
    bool isSelect=false;
    QVector<VernierDataBusy> busyList;
};

enum FileType{
    NotFile,
    CSVFile,
    SourceFile
};

class Session : public QObject
{
    Q_OBJECT
public:
    Session(QString name,QString id,qint32 channelCount,SessionType type,QString path,DataService* parent);
    ~Session();
    void LoadFile(QString path);
    void SaveFile(QString path, QString sessionName, FileType type);
    bool OrderStart(QByteArray dataBytes,QByteArray setBytes, double second, quint64 samplingDepth, quint64 triggerSamplingDepth,
                    qint64 samplingFrequency, qint32 type, QVector<qint8> channelIDList, bool isBuffer, bool isRLE);
    void showErrorMsg(QString error_msg, bool waitClose_=false);
    void OrderStop();
    bool PWM(QByteArray dataBytes);
    qint64 getSamplingFrequency();
    QString newDecode();
    QHash<QString,DecodeController*> getDecodeList();
    void resetDecode(QString decodeID);
    void removeDecode(QString decodeID);
    DecodeController* getDecode(QString decodeID);
    USBControl *getUSBControl();
    void channelNameChanged(qint8 channelID, QString name);
    bool setGlitchRemoval(QJsonArray json);
    QJsonArray getGlitchRemoval();
    void setDecodeShowJson(QString decodeID, QJsonObject json);
    void setRecodeJSON(QString decodeID, QJsonObject json);
    void initError();
    void calcMeasureData(qint32 measureID, qint32 channelID);
    void fpgaActive();
    void fpgaDormancy();
    void saveDecodeTable(QString filePath);

private:
    void deleteSegment();

public slots:
    void onAppendMeasure(qint32 measureIndex);
    void onAppendVernier(qint32 vernierIndex);

signals:
    void drawUpdate(bool isPoll=false);
    void refreshZoom(qint32 state);
    void sendThreadPara(USBControl* usb, Segment* segment, SessionConfig* config, double second, void* channelIDList, bool isBuffer, bool isRLE);
    void sendSaveFileThreadPara(QString filePath, Segment* segment, QVector<QString> channelsName);
    void sendSaveSourceFileThreadPara(QString filePath, QString sessionName, Segment* segment,
                                      QVector<QString> channelsName, QVector<MeasureData>* measureList, QVector<VernierData>* vernierList,
                                      QHash<QString,DecodeController*>* decodes);
    void sendLoadSourceFileThreadPara(QString filePath, Segment* segment, SessionConfig* config,
                                      QVector<MeasureData>* measureList, QVector<VernierData>* vernierList, QHash<QString,DecodeController*>* decodes,
                                      QVector<QString>* channelsNames);
    void sendSaveDecodeTableThreadPara(QString filePath, qint32 multiply, QVector<DecodeRowData>* data, QVector<QMap<QString,QJsonObject>>* rowType,
                                       QVector<bool>* saveList);
    void sendCalcMeasureData(qint32 measureID, qint32 channelID, QVector<MeasureData>* measure, Segment* segment);
    void sendGlitchRemovalThreadPara(Segment* segment);
    void measureDataChanged();
    void appendMeasure(qint32 measureID);
    void appendVernier();
    void sendDeviceRecvSchedule(qint32 schedule, qint32 type, qint32 state_);

public:
    ThreadWork* m_workThread;
    SessionConfig* m_config;
    Segment* m_segment=nullptr;
    Segment* m_segmentRecode=nullptr;
    QVector<MeasureData> m_measure;
    QVector<VernierData> m_vernier;
    qint32 m_measureIndex=0;
    qint32 m_vernierIndex=1;
    qint32 m_channelCount;
    bool m_isShowDecode=false;
    QHash<QString,DecodeController*> m_decodeList;
    QVector<QString> m_channelsName;
    QJsonArray m_GlitchRemovalJsonArray;

    
    QVector<DecodeRowData>* m_tableData=nullptr;
    QVector<QMap<QString,QJsonObject>>* m_tableRowType=nullptr;
    QVector<bool> m_tableSaveIndex;
    qint32 m_tableMultiply;

private:
    QString m_sessionName;
    QString m_sessionID;
    SessionType m_sessionType;
    DataService* m_parent;
    QString m_filePath;
};

#endif 
