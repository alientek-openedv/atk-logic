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

#ifndef DRAW_CHANNEL_H
#define DRAW_CHANNEL_H

#include <QQuickPaintedItem>
#include <QColor>
#include <QPainter>
#include <QImage>
#include <QMimeData>
#include "double_buffering.h"
#include "pv/static/data_service.h"

class DrawChannel : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit DrawChannel(QQuickItem *parent = 0);
    ~DrawChannel();

private:
    void init(QString sessionID);
    void refreshDoubleBuffer();
    void appendMeasureData();
    void measureMethod(qint32 mouseX, QPainter* painter=nullptr, bool isClick=false);
    void vernierMethod(bool isClick=false);
    void drawMousePopup(qint64 start,qint64 end,bool isTop,QPainter &painter);
    qint64 getAdsorbPosition(qint32 x);
    void drawMouseMeasure(QPainter &t, Segment* segment);
    void vernierMove(qint32 x);
    void zoomMouseRightClick();

signals:
    void showMouseMeasurePopup(qint32 x_,QString period,QString freq,QString duty);
    void closeMouseMeasurePopup();
    void mouseZoom(bool show, qint32 x, qint32 y, qint32 width, qint32 height);
    void measureDataChanged(qint32 measureID);
    void vernierDataChanged(qint32 vernierID);
    void vernierMoveState(bool isMove,qint32 id);
    void measureMoveState(bool isMove);
    void showViewScope(qint64 start, qint64 end);
    void measureSelectChanged();
    void closeVernierPopup();
    void deleteVernier(qint32 vernierID);
    void vernierCreateComplete();
    bool sendMouseEvent(qint32 x);
    void hoverEnter();
    void hoverLeave();
    void getAdsorbChannelID(qint32 y);
    void setLiveFollowing(bool isEnable);

private slots:
    void onSizeChanged();

private:
    struct measurePoint{
        qint32 x;
        qint32 y;
        qint64 start;
        qint64 end;
    };
    SessionConfig m_configRecode;
    Session* m_session=nullptr;
    QString m_sessionID="";
    qint32 m_channelID=-1;
    QString m_decodeID="";
    qint32 m_height=0;
    qint32 m_width=0;
    bool m_isDecode=false;
    DoubleBuffering* m_doubleBuffer=nullptr;
    qint32 m_measureOffset=4;
    measurePoint m_measurePoint;
    QColor m_measureTypeColor[8];
    qint32 m_measureSelectAlpha[2];
    qint32 m_measureNotSelectAlpha[2];
    Qt::MouseButton m_currentButton;
    QColor m_mouseMeasureLineColor;
    QColor m_mouseMeasureBackgroundColor;
    QColor m_mouseMeasureTextColor;

protected:
    void paint(QPainter *painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override;

    
public:
    Q_INVOKABLE void init(QString sessionID,int channelID);
    Q_INVOKABLE void decodeInit(QString sessionID,QString decodeID);
    Q_INVOKABLE qint32 getLastMeasureID();
    Q_INVOKABLE void forceRefresh();
    Q_INVOKABLE void setTheme(QString theme);


    const QColor &DataColor() const;
    void setDataColor(const QColor &newDataColor);

    qint32 measureState() const;
    void setMeasureState(qint32 newMeasureState);

    bool showCursor() const;
    void setShowCursor(bool newShowCursor);

    bool state_() const;
    void setState(bool newState);

    bool isMouseMeasure() const;
    void setIsMouseMeasure(bool newIsMouseMeasure);

    QColor TextColor() const;
    void setTextColor(const QColor &newTextColor);

    QString decodeChannelDesc() const;
    void setDecodeChannelDesc(const QString &newDecodeChannelDesc);

    bool vernierCreateModel() const;
    void setVernierCreateModel(bool newVernierCreateModel);

    qint32 adsorbChannelID() const;
    void setAdsorbChannelID(qint32 newAdsorbChannelID);

    bool isRunCollect() const;
    void setIsRunCollect(bool newIsRunCollect);

    bool isExit() const;
    void setIsExit(bool newIsExit);

signals:
    void measureStateChanged();
    void showCursorChanged();
    void isMouseMeasureChanged();
    void decodeChannelDescChanged();
    void vernierCreateModelChanged();
    void adsorbChannelIDChanged();
    void isRunCollectChanged();
    void isExitChanged();

private:
    QColor m_DataColor;
    qint32 m_measureState=0;
    QPoint m_mousePoint;
    QPoint m_oldPoint;
    QPoint m_zoomPoint,m_zoomPointEnd;
    QString m_theme="";
    qint64 m_showStartUnitRecode;
    qint32 m_lastMeasureID=0;
    qint32 m_measureEnter=0;
    bool m_showCursor=false;
    qint32 m_selectMeasureIndex=-1;
    qint32 m_selectVernierIndex=-1;
    QImage m_arrowsImage[2];
    bool m_isMouseMeasure=true;
    QString m_decodeChannelDesc="";
    bool m_vernierCreateModel=false;
    bool m_isVernierMove=false;
    bool m_isRunCollect=false;
    bool m_isExit=false;
    qint32 m_adsorbChannelID;

    Q_PROPERTY(QColor showDataColor READ DataColor WRITE setDataColor)
    Q_PROPERTY(qint32 measureState READ measureState WRITE setMeasureState NOTIFY measureStateChanged)
    Q_PROPERTY(bool showCursor READ showCursor WRITE setShowCursor NOTIFY showCursorChanged)
    Q_PROPERTY(bool isMouseMeasure READ isMouseMeasure WRITE setIsMouseMeasure NOTIFY isMouseMeasureChanged)
    Q_PROPERTY(QString decodeChannelDesc READ decodeChannelDesc WRITE setDecodeChannelDesc NOTIFY decodeChannelDescChanged)
    Q_PROPERTY(bool vernierCreateModel READ vernierCreateModel WRITE setVernierCreateModel NOTIFY vernierCreateModelChanged)
    Q_PROPERTY(qint32 adsorbChannelID READ adsorbChannelID WRITE setAdsorbChannelID NOTIFY adsorbChannelIDChanged)
    Q_PROPERTY(bool isRunCollect READ isRunCollect WRITE setIsRunCollect NOTIFY isRunCollectChanged)
    Q_PROPERTY(bool isExit READ isExit WRITE setIsExit NOTIFY isExitChanged)
};

#endif 
