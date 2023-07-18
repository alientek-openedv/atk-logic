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

#include "draw_background.h"

DrawBackground::DrawBackground(QQuickItem *parent) : QQuickPaintedItem{parent}
{
    connect(this,SIGNAL(widthChanged()),this,SLOT(onSizeChanged()));
    connect(this,SIGNAL(heightChanged()),this,SLOT(onSizeChanged()));
}

void DrawBackground::paint(QPainter* painter)
{
    if(m_sessionID.isEmpty()||m_isExit)
        return;
    painter->save();
    drawVernier(*painter);
    drawMouseZoom(*painter);
    painter->restore();
}

void DrawBackground::drawVernier(QPainter &painter)
{
    qint64 start=m_session->m_config->m_showStartUnit;
    double pixelUnit=m_session->m_config->m_showConfig.m_pixelUnit;
    QVector<VernierData>* vernierData=&m_session->m_vernier;
    qint32 count=vernierData->count();
    for(qint32 i=0;i<count;i++){
        VernierData data=vernierData->at(i);
        if(!data.visible)
            continue;
        qint32 temp=(data.position-start)/pixelUnit;
        if(temp<0||temp>m_width)
            continue;

        QColor color=data.dataColor;
        color.setAlpha(data.isSelect?255:179);
        painter.setPen(QPen(color, 1));
        painter.drawLine(temp,0,temp,m_height);
    }
}

void DrawBackground::drawMouseZoom(QPainter &painter)
{
    if(m_showMouseZoom)
        painter.fillRect(m_mouseZoomRect.x(),m_mouseZoomRect.y()+m_mouseZoomOffsetY,m_mouseZoomRect.width(),m_mouseZoomRect.height(),QColor::fromRgb(80,80,80,204));
}

void DrawBackground::onSizeChanged()
{
    m_height=height();
    m_width=width();
}

bool DrawBackground::isExit() const
{
    return m_isExit;
}

void DrawBackground::setIsExit(bool newIsExit)
{
    if (m_isExit == newIsExit)
        return;
    m_isExit = newIsExit;
    emit isExitChanged();
}

#pragma region "QML方法" {
void DrawBackground::init(QString sessionID)
{
    setAcceptedMouseButtons(Qt::NoButton);
    DataService* dataService = DataService::getInstance();
    m_session=dataService->getSession(sessionID);
    if(m_session)
        m_sessionID=sessionID;
    update();
}

void DrawBackground::setMouseZoom(bool show, qint32 x, qint32 y, qint32 width, qint32 height)
{
    m_showMouseZoom=show;
    m_mouseZoomRect=QRect(x,y,width,height);
    update();
}

void DrawBackground::setMouseYOffset(qint32 y)
{
    m_mouseZoomOffsetY=y;
    update();
}
#pragma endregion}
