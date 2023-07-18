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

#ifndef DRAW_BACKGROUND_H
#define DRAW_BACKGROUND_H

#include <QQuickPaintedItem>
#include <QColor>
#include <QPainter>
#include "pv/static/data_service.h"

class DrawBackground : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit DrawBackground(QQuickItem *parent = nullptr);

private:
    void drawVernier(QPainter &painter);
    void drawMouseZoom(QPainter &painter);

private slots:
    void onSizeChanged();

private:
    Session* m_session=nullptr;
    QString m_sessionID="";
    qint32 m_height=0;
    qint32 m_width=0;
    bool m_showMouseZoom=false;
    QRect m_mouseZoomRect;
    qint32 m_mouseZoomOffsetY=0;
    bool m_isExit=false;

    Q_PROPERTY(bool isExit READ isExit WRITE setIsExit NOTIFY isExitChanged)

protected:
    void paint(QPainter *painter) override;

public:
    Q_INVOKABLE void init(QString sessionID);
    Q_INVOKABLE void setMouseZoom(bool show, qint32 x, qint32 y, qint32 width, qint32 height);
    Q_INVOKABLE void setMouseYOffset(qint32 y);
    bool isExit() const;
    void setIsExit(bool newIsExit);
signals:
    void isExitChanged();
};

#endif 
