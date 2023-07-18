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

#ifndef DRAW_BACKGROUND_FLOOR_H
#define DRAW_BACKGROUND_FLOOR_H

#include <QQuickPaintedItem>
#include "pv/static/data_service.h"

class DrawBackgroundFloor : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit DrawBackgroundFloor(QQuickItem *parent = nullptr);

private:
    void vernierMethod(bool isClick=false);
    void vernierMove(qint32 x);
    qint64 getAdsorbPosition(qint32 x);

private slots:
    void onSizeChanged();

signals:
    void vernierDataChanged(qint32 vernierID);
    void measureSelectChanged();
    void closeVernierPopup();
    void hoverLeave();
    void drawUpdate();
    void showViewScope(qint64 start, qint64 end);
    void mouseZoom(bool show, qint32 x, qint32 y, qint32 width, qint32 height);
    void deleteVernier(qint32 vernierID);
    void vernierCreateComplete();
    void getAdsorbChannelID(qint32 y);
    void vernierMoveState(bool isMove, qint32 id);

private:
    Session* m_session=nullptr;
    QString m_sessionID="";
    qint32 m_height=0;
    qint32 m_width=0;
    QPoint m_mousePoint;
    QPoint m_oldPoint;
    qint32 m_selectVernierIndex=-1;
    QPoint m_zoomPoint,m_zoomPointEnd;
    Qt::MouseButton m_currentButton;

protected:
    void paint(QPainter *painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override;

public:
    Q_INVOKABLE void init(QString sessionID);

    bool showCursor() const;
    void setShowCursor(bool newShowCursor);

    bool vernierCreateModel() const;
    void setVernierCreateModel(bool newVernierCreateModel);

    qint32 adsorbChannelID() const;
    void setAdsorbChannelID(qint32 newAdsorbChannelID);

    bool isExit() const;
    void setIsExit(bool newIsExit);

signals:
    void showCursorChanged();

    void vernierCreateModelChanged();

    void adsorbChannelIDChanged();

    void isExitChanged();

private:
    bool m_showCursor=false;
    bool m_vernierCreateModel=false;
    bool m_isExit=false;
    qint32 m_adsorbChannelID;

    Q_PROPERTY(bool showCursor READ showCursor WRITE setShowCursor NOTIFY showCursorChanged)
    Q_PROPERTY(bool vernierCreateModel READ vernierCreateModel WRITE setVernierCreateModel NOTIFY vernierCreateModelChanged)
    Q_PROPERTY(qint32 adsorbChannelID READ adsorbChannelID WRITE setAdsorbChannelID NOTIFY adsorbChannelIDChanged)
    Q_PROPERTY(bool isExit READ isExit WRITE setIsExit NOTIFY isExitChanged)
};

#endif 
