#include "draw_background.h"

DrawBackground::DrawBackground(QQuickItem *parent) : QQuickPaintedItem{parent}
{
    connect(this,SIGNAL(widthChanged()),this,SLOT(onSizeChanged()));
    connect(this,SIGNAL(heightChanged()),this,SLOT(onSizeChanged()));

    qreal lineWidth=4,lineHeight=4;
    m_crossChannelHitLines.append(QLine(0,0,lineWidth,lineHeight));
    m_crossChannelHitLines.append(QLine(0,qCeil(lineHeight/2),lineWidth,qCeil(lineHeight/2)));
    m_crossChannelHitLines.append(QLine(0,lineHeight,lineWidth,0));
}

void DrawBackground::paint(QPainter* painter)
{
    if(m_sessionID.isEmpty()||m_isExit)
        return;
    painter->save();
    drawVernier(*painter);
    drawMouseZoom(*painter);
    drawCrossChannelMeasure(*painter);
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
        painter.fillRect(m_mouseZoomRect.x(),m_mouseZoomRect.y()+m_mouseZoomOffsetY,
                         m_mouseZoomRect.width(),m_mouseZoomRect.height(),QColor::fromRgb(80,80,80,204));
}

void DrawBackground::drawCrossChannelMeasure(QPainter &painter)
{
    if(m_crossChannelMeasureState){
        {
            QPoint startPoint=QPoint(m_crossChannelMeasureStartPosition.point.x(),
                                     m_crossChannelMeasureStartPosition.point.y()+m_crossChannelMeasureStartPosition.YOffset);
            QPoint endPoint=QPoint(m_crossChannelMeasureEndPosition.point.x(),
                                   m_crossChannelMeasureEndPosition.point.y()+m_crossChannelMeasureEndPosition.YOffset);
            painter.setPen(QPen(QColor::fromRgb(0xe58175), 1));
            QPainterPath bezier(startPoint);
            bezier.cubicTo(QPoint((startPoint.x() + endPoint.x()) / 2, startPoint.y()),
                           QPoint((startPoint.x() + endPoint.x()) / 2, endPoint.y()),
                           endPoint);
            painter.drawPath(bezier);
        }
        if(m_crossChannelMeasureStartPosition.isHit){
            QVector<QLine> lines=m_crossChannelHitLines;
            qint32 XOffset=m_crossChannelMeasureStartPosition.point.x();
            qint32 YOffset=m_crossChannelMeasureStartPosition.point.y()+m_crossChannelMeasureStartPosition.YOffset;
            for(auto &i : lines){
                qint32 xMiddle=XOffset-(i.x2()-i.x1())/2;
                qint32 yMiddle=YOffset-(i.x2()-i.x1())/2;
                i.setLine(i.x1()+xMiddle, i.y1()+yMiddle, i.x2()+xMiddle, i.y2()+yMiddle);
            }
            painter.drawLines(lines);
        }

        if(m_crossChannelMeasureEndPosition.isHit){
            QVector<QLine> lines=m_crossChannelHitLines;
            qint32 XOffset=m_crossChannelMeasureEndPosition.point.x();
            qint32 YOffset=m_crossChannelMeasureEndPosition.point.y()+m_crossChannelMeasureEndPosition.YOffset;
            for(auto &i : lines){
                qint32 xMiddle=XOffset-(i.x2()-i.x1())/2;
                qint32 yMiddle=YOffset-(i.x2()-i.x1())/2;
                i.setLine(i.x1()+xMiddle, i.y1()+yMiddle, i.x2()+xMiddle, i.y2()+yMiddle);
            }
            painter.drawLines(lines);
        }
        qint64 startPosition=m_crossChannelMeasureStartPosition.position;
        qint64 endPosition=m_crossChannelMeasureEndPosition.position;
        if(startPosition>endPosition)
        {
            startPosition^=endPosition;
            endPosition^=startPosition;
            startPosition^=endPosition;
        }
        {
            //绘制浮窗
            QString str;
            if(endPosition-startPosition>0)
                str=nsToShowStr(endPosition-startPosition, 12)+" / "+hzToShowStr(1000000000./(endPosition-startPosition), 8);
            else
                str="0 ns / 0 hz";
            QFont font=painter.font();
            painter.setPen(QPen(QColor::fromRgb(0x383838), 0));
            font.setPixelSize(12);
            painter.setFont(font);
            qint32 fontWidth=painter.fontMetrics().horizontalAdvance(str);
            QPainterPath path;
            qint32 y=m_crossChannelMeasureEndPosition.mouseY+m_crossChannelMeasureEndPosition.YOffset+22;
            qint32 x=m_crossChannelMeasureEndPosition.point.x()+10;
            if(x+fontWidth+25>m_width)
                x=m_width-fontWidth-25;
            path.addRoundedRect(QRect(x,y,fontWidth+20,20),6,6);
            painter.fillPath(path, QBrush(QColor::fromRgba(0xddbcedff)));
            painter.drawText(x+10,y+13,str);
        }
    }
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

void DrawBackground::setCrossChannelMeasureState(bool isStop)
{
    m_crossChannelMeasureState=!isStop;
    m_crossChannelMeasureStartPosition=CrossChannelMeasure();
    m_crossChannelMeasureEndPosition=CrossChannelMeasure();
    update();
}

void DrawBackground::setCrossChannelMeasurePosition(qint32 type, qint32 x, qint32 y, qint32 mouseY, qint64 position, bool isHit)
{
    if(type==1){
        m_crossChannelMeasureStartPosition.point=QPoint(x,y);
        m_crossChannelMeasureStartPosition.position=position;
        m_crossChannelMeasureStartPosition.isHit=isHit;
        m_crossChannelMeasureStartPosition.mouseY=mouseY;
    }else{
        m_crossChannelMeasureEndPosition.point=QPoint(x,y);
        m_crossChannelMeasureEndPosition.position=position;
        m_crossChannelMeasureEndPosition.isHit=isHit;
        m_crossChannelMeasureEndPosition.mouseY=mouseY;
    }
    update();
}

void DrawBackground::setCrossChannelMeasureYOffset(qint32 type, qint32 YOffset)
{
    if(type==1)
        m_crossChannelMeasureStartPosition.YOffset=YOffset;
    else
        m_crossChannelMeasureEndPosition.YOffset=YOffset;
}
#pragma endregion}
