#ifndef DECODE_CONTROLLER_H
#define DECODE_CONTROLLER_H

#include <QJsonObject>
#include <QJsonArray>
#include <QThread>
#include <QColor>
#include <QApplication>
#include <QPoint>
#include <QMutex>
#include <atk_decoder.h>
#include "pv/static/util.h"
#include "pv/static/log_help.h"
#include "segment.h"

struct DecodeRowData
{
    qint8 decodeIndex;
    quint8 type;
    qint64 start;
    qint64 end;
    QString longText;
    QString text;
};

struct DecodeRowDataNode
{
    qint32 index;
    qint64 start;
};

struct DecodeRow
{
    QString id;
    QString desc;
    bool isShow;
    qint32 dataCount;
    qint8 colorState;
    QColor color;
    QList<DecodeRowData> data;
    QList<DecodeRowData> tmpData;
    QList<DecodeRowDataNode> node;
    QMap<QString,QJsonObject> rowType;
    DecodeRow(){
        DecodeRowDataNode temp;
        temp.index=0;
        temp.start=0;
        node.append(temp);
        dataCount=0;
        colorState=0;
    }
};

struct DataOrder
{
    QString name;
    qint32 rowIndex;
    qint32 len;
    DataOrder(QString Name,qint32 Row,qint32 Len){
        name=Name;
        rowIndex=Row;
        len=Len;
    }
};

struct DecodeRowRoot
{
    QVector<DecodeRow> row;
    bool isSingle;
    QMutex mutex;
};

static void Insert_sort(QList<DecodeRowData> &arr);

static void _output_callback(struct atk_proto_data *pData, void *cb_data);

static void _output_binary_callback(struct atk_proto_data *pData, void *cb_data);

class DecodeController;

static void data_insert(DecodeController* decode, QString name, qint32 row);

struct restartRecode
{
    bool isRecode=false;
    QJsonObject json;
    qint64 ullDecodeStart;
    qint64 ullDecodeEnd;
    qint64 ullMaxSample;
};

class DecodeController : public QObject
{
    Q_OBJECT
public:
    explicit DecodeController(const QString decodeID, QObject *parent = nullptr);
    ~DecodeController();
    bool analysisJSON(QJsonObject json, qint64 samplingFrequency);
    void decodeAnalysis(Segment* segment, qint64 ullDecodeStart, qint64 ullDecodeEnd, qint64 maxSample, bool isContinuous);
    bool decodeToPosition(qint32 multiply, qint64 ullPosition, qint64 maxSample_);
    void stopDedoceAnalysis();
    void waitStopDedoceAnalysis();
    void setRecodeJSON(QJsonObject json);
    bool restart(Segment* segment, qint64 maxUnit, bool isContinuous);
    void deleteData();
    qint32 findDecodeRowFirstData(DecodeRowRoot* rows, qint32 index, qint32 left, qint32 right, qint64 start);
    void findDecodeRowDataNode(DecodeRowRoot* rows, qint32 index, qint32 &left_, qint32 &right_, qint64 start);
    bool isRun();
    QString getDecodeID();
    QJsonObject getJsonObject();
    void reloadDecoder();

private:
    void setState(qint8 state);

signals:
    void sendDecodeSchedule(QString decodeID, double schedule);
    void sendDecodeReset(QString decodeID);

public:
    QHash<QString, DecodeRowRoot*> m_rowList;//协议返回通道数据
    QMutex m_rowListMutex;
    QList<DataOrder> m_orderList;//协议排序索引
    QMutex m_orderListMutex;
    QList<QString> m_decodeIndex;//协议Name顺序

    qint32 m_multiply;
    qint64 m_decodeStart;
    qint32 m_height=1;//高度倍数
    bool m_isLockRow=false;
    bool m_isDelete=false;
    bool m_isStop=false;
    bool m_isReportedError=false;
    bool m_isRestart=false;

private:
    const qint32 _MAX_CHUNK_SIZE = 1024 * 64;	/* 发送数据包最大长度 */
    QMutex m_lock;
    qint8 m_state=0;//0=无运行线程，1=运行中，2=退出中
    QThread m_thread;
    QString m_decodeID;
    atk_session* m_decodeSession=nullptr;
    atk_decoder_inst *m_decoder_inst=nullptr;
    QMutex m_streamLock;
    restartRecode m_streamRecode;
    restartRecode m_recode;
    QVector<int> m_channelList;//获取数据的对应通道
    QVector<int> m_sendChannelList;//发送数据的对应通道
    bool m_isFirst=true;//是否首次加载
};
#endif // DECODE_CONTROLLER_H
