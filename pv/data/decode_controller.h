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
    qint8 type;
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
    QVector<DecodeRowData> data;
    QVector<DecodeRowDataNode> node;
    QMap<QString,QJsonObject> rowType;
    DecodeRow(){
        DecodeRowDataNode temp;
        temp.index=0;
        temp.start=0;
        node.append(temp);
        dataCount=0;
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

static void _output_callback(struct atk_proto_data *pData, void *cb_data);

struct restartRecode
{
    bool isRecode=false;
    QJsonObject json;
    qint32 channelCount;
    qint64 ullDecodeStart;
    qint64 ullDecodeEnd;
};

class DecodeController : public QObject
{
    Q_OBJECT
public:
    explicit DecodeController(const QString decodeID, QObject *parent = nullptr);
    ~DecodeController();
    bool analysisJSON(QJsonObject json, qint64 samplingFrequency);
    void decodeAnalysis(Segment* segment, qint32 channelCount, qint64 ullDecodeStart, qint64 ullDecodeEnd, qint64 maxSample);
    void stopDedoceAnalysis();
    void waitStopDedoceAnalysis();
    void setRecodeJSON(QJsonObject json);
    bool restart(Segment* segment, qint64 maxUnit);
    void deleteData();
    qint32 findDecodeRowFirstData(DecodeRowRoot* rows, qint32 index, qint32 left, qint32 right, qint64 start);
    void findDecodeRowDataNode(DecodeRowRoot* rows, qint32 index, qint32 &left_, qint32 &right_, qint64 start);
    bool isRun();
    QString getDecodeID();
    QJsonObject getJsonObject();

private:
    void setState(qint8 state);

signals:
    void sendDecodeSchedule(QString decodeID, double schedule);
    void sendDecodeReset(QString decodeID);

public:
    QHash<QString, DecodeRowRoot*> m_rowList;
    QMutex m_rowListMutex;
    QVector<DataOrder> m_orderList;
    QMutex m_orderListMutex;
    QVector<QString> m_decodeIndex;

    qint32 m_multiply;
    qint64 m_decodeStart;
    bool m_isDelete=false;
    bool m_isStop=false;
    bool m_isReportedError=false;

private:
    const qint32 _MAX_CHUNK_SIZE = 1024 * 64;
    QMutex m_lock;
    qint8 m_state=0;
    QThread m_thread;
    QString m_decodeID;
    atk_session* m_decodeSession=nullptr;
    atk_decoder_inst *m_decoder_inst=nullptr;
    restartRecode m_recode;
    QVector<int> m_channelList;
    QVector<int> m_sendChannelList;
    bool m_isFirst=true;
};
#endif 
