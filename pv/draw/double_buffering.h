#ifndef DOUBLE_BUFFERING_H
#define DOUBLE_BUFFERING_H

#include <QPixmap>
#include <QPainter>
#include <QPainterPath>

#include <pv/data/session.h>

class DoubleBuffering : public QObject
{
    Q_OBJECT
public:
    DoubleBuffering(QString sessionID,int channelID, QObject *parent = nullptr);
    DoubleBuffering(QString sessionID, QString decodeID, QObject *parent = nullptr);
    ~DoubleBuffering();
    void setSize(qint32 width, qint32 height);
    void refReshBuffer(QPainter *painter);
    void init(QString sessionID);

private:
    inline void DrawChannelLine(QPainter *painter, double start, double end, bool isHigh, bool isEndLine);
    inline void DrawChannelLine(QPainter* painter,qint32 x, qint8 data);
    void DrawDecode(QPainter &painter);
    void DrawDecodeFill(QPainter* painter, qint32 startWidth, qint32 endWidth, qint32 rowIndex, QPen& blockFillPen);
    void DrawDecodeRow(QPainter* painter, qint32 start, qint32 end, DecodeRowData data, qint32 rowIndex, qint32 ellipseWidth, QPen& blockFillPen);
    void DrawDecodeRowName(QPainter* painter, qint32 rowCount, QString text);
    bool DrawDecodeText(QPainter* painter, qint32 start, qint32 end, qint32 y, DecodeRowData data);
    void DrawLogic(QPainter &painter);
    void DrawLogicBasics(QPainter &painter);

public:
    QColor m_DataColor;
    QColor m_TextColor;
    QColor m_DashLineColor;
    qint32 m_decodeHeight;
    QString m_decodeChannelDesc="";
    bool m_isExit=false;

private:
    Session* m_session=nullptr;
    QString m_sessionID="";
    qint32 m_channelID=-1;
    QString m_decodeID="";
    bool m_isDecode=false;

    qint32 m_height=0;//通道高度
    qint32 m_width=0;//通道宽度
    qint32 m_decodeRowInitHeight=14;//协议行初始高度
    qint32 m_decodeRowHeight=14;//协议行高度
    qint32 m_decodeRowInterval=2;//协议行间隔
    qint32 m_topOffset=6;
    QPainterPath m_trianglePath;
    QColor m_decodeTypeColor[16];
    QVector<qreal> m_blockDash;
};

#endif // DOUBLE_BUFFERING_H
