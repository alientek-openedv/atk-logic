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
    QString start;//起始位置
    QString startDetail;//起始位置详细
    QString end;//结束位置
    QString endDetail;//结束位置详细
    QString time;//时间
    QString timeDetail;//时间详细
    QString minFreq;//最小频率
    QString minFreqDetail;//最小频率详细
    QString maxFreq;//最大频率
    QString maxFreqDetail;//最大频率详细
};

struct MeasureData
{
    qint32 id;//ID
    QString name;//名称
    qint32 channelID;//通道ID
    qint64 start;//起始位置
    qint64 end;//结束位置
    qint32 rising;//上升沿
    qint32 falling;//下降沿
    QColor dataColor;//显示颜色
    MeasureDataStr str;//显示文本
    //以下为控制属性
    QString note;//注释
    qint32 offset;
    bool isAutoOffset=true;
    bool isSelect;//是否选中
    bool isCalc=false;//是否计算中
    bool isRefreshCalc=false;//是否重复再次计算
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
    SourceFile,
    BinNsFile,
    BinSampleFile
};

class Session : public QObject
{
    Q_OBJECT
public:
    Session(QString name,QString id,qint32 channelCount,SessionType type,QString path,DataService* parent);
    ~Session();
    void initFile();
    void LoadFile(QString path, bool isSendDecode);
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
    void drawUpdate(qint32 isPoll=-1);//ispoll是区分滚动条数值刷新
    void refreshZoom(qint32 state);
    void sendThreadPara(USBControl* usb, Segment* segment, SessionConfig* config, double second, void* channelIDList, bool isBuffer, bool isRLE);
    void sendSaveFileThreadPara(QString filePath, Segment* segment, QVector<QString> channelsName);
    void sendSaveSourceFileThreadPara(QString filePath, QString sessionName, Segment* segment,
                                      QVector<QString> channelsName, QVector<MeasureData>* measureList, QVector<VernierData>* vernierList,
                                      QHash<QString,DecodeController*>* decodes);
    void sendLoadSourceFileThreadPara(QString filePath, Segment* segment, SessionConfig* config,
                                      QVector<MeasureData>* measureList, QVector<VernierData>* vernierList, QHash<QString,DecodeController*>* decodes,
                                      QVector<QString>* channelsNames, bool isSendDecode);
    void sendSaveDecodeTableThreadPara(QString filePath, qint32 multiply, QList<DecodeRowData>* data, QList<QMap<QString,QJsonObject>>* rowType,
                                       QVector<bool>* saveList);
    void sendSaveBinFileThreadPara(QString filePath, Segment *segment, qint32 type, QString channels);
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
    Segment* m_segmentRecode=nullptr;//毛刺过滤才会用这记录原始数据
    QVector<MeasureData> m_measure;
    QVector<VernierData> m_vernier;
    qint32 m_measureIndex=0;
    qint32 m_vernierIndex=1;
    qint32 m_channelCount;
    bool m_isShowDecode=false;
    QHash<QString,DecodeController*> m_decodeList;
    QVector<QString> m_channelsName;
    QJsonArray m_GlitchRemovalJsonArray;

    //保存协议表格
    QList<DecodeRowData>* m_tableData=nullptr;
    QList<QMap<QString,QJsonObject>>* m_tableRowType=nullptr;
    QVector<bool> m_tableSaveIndex;
    qint32 m_tableMultiply;

private:
    QString m_sessionName;
    QString m_sessionID;
    SessionType m_sessionType;
    DataService* m_parent;
    QString m_filePath;
};

#endif // SESSION_H
