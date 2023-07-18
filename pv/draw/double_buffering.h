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
    QPixmap getPixmap();
    bool isPixmap();
    void setSize(qint32 width, qint32 height);
    void refReshBuffer();
    void init(QString sessionID);

private:
    inline void DrawChannelLine(QPainter *painter, double start, double end, bool isHigh, bool isEndLine);
    inline void DrawChannelLine(QPainter* painter,qint32 x, qint8 data);
    void DrawDecode(QPainter &painter);
    void DrawDecodeRow(QPainter* painter, qint32 start, qint32 end, DecodeRowData data, qint32 rowIndex, qint32 rowNameWidth);
    void DrawDecodeText(QPainter* painter, qint32 start, qint32 end, qint32 y, DecodeRowData data);
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
    QPixmap* m_pPixmap=nullptr;
    QPixmap* m_pPixmap2=nullptr;
    Session* m_session=nullptr;
    QString m_sessionID="";
    qint32 m_channelID=-1;
    QString m_decodeID="";
    bool m_isDecode=false;

    qint32 m_height=0;
    qint32 m_width=0;
    qint32 m_decodeRowHeight=20;
    qint32 m_decodeRowInterval=2;
    QPainterPath m_trianglePath;
    QColor m_decodeTypeColor[16];
};

#endif 
