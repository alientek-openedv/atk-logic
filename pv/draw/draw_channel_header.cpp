#include "draw_channel_header.h"

DrawChannelHeader::DrawChannelHeader(QQuickItem* parent): QQuickPaintedItem(parent)
{
    connect(this,SIGNAL(widthChanged()),this,SLOT(onSizeChanged()));
    connect(this,SIGNAL(heightChanged()),this,SLOT(onSizeChanged()));
    m_vernierTypeColor[0]="#26A3D9";
    m_vernierTypeColor[1]="#ED5756";
    m_vernierTypeColor[2]="#0DA074";
    m_vernierTypeColor[3]="#7F5DCD";
    m_vernierTypeColor[4]="#F59647";
    m_vernierTypeColor[5]="#CE437E";
    m_mousePoint=QPoint(-1,-1);
}

void DrawChannelHeader::onSizeChanged()
{
    m_height=height();
    m_width=width();
}

bool DrawChannelHeader::isExit() const
{
    return m_isExit;
}

void DrawChannelHeader::setIsExit(bool newIsExit)
{
    if (m_isExit == newIsExit)
        return;
    m_isExit = newIsExit;
    emit isExitChanged();
}

void DrawChannelHeader::drawScale(QPainter* painter, quint32 position, QString text, bool isHigh)
{
    if(isHigh){
        painter->drawLine(QLineF(position, m_highLineTop, position, height()));
        //painter->fillRect(position-1,m_highLineTop,3,m_highLineBottom,m_text_color);
        painter->drawText(position,10,text);
    }else{
        painter->drawLine(QLineF(position, m_lowLineTop, position, height()));
        painter->drawText(position-4,21,"+"+text);
    }

}

QString DrawChannelHeader::time2TimeStr(qint64 time, QString unit)
{
    QString refStr="";
    QString unitList[]={"ns","μs","ms","s","m","h","null"};
    for(quint32 i=0;i<LEN(unitList);i++){
        if(unit==unitList[i])
        {
            if(refStr.size()!=0)
                refStr.push_front(" : ");
            if(i<3){
                if(time>=1000)
                {
                    refStr.push_front(QString::number(time%1000, 'f', 0)+" "+unit);
                    time/=1000;
                }else
                {
                    refStr.push_front(QString::number(time, 'f', 0)+" "+unit);
                    break;
                }
            }else{
                if(time>=60)
                {
                    refStr.push_front(QString::number(time%60, 'f', 0)+" "+unit);
                    time/=60;
                    if(i+1>=LEN(unitList))
                        break;
                }else
                {
                    refStr.push_front(QString::number(time, 'f', 0)+" "+unit);
                    break;
                }
            }
            unit=unitList[i+1];
        }
    }
    return refStr;
}

void DrawChannelHeader::appendVernierData(qint32 x)
{
    VernierData data;
    data.id=m_session->m_vernierIndex;
    data.name="T"+QString::number(data.id);
    data.dataColor=m_vernierTypeColor[m_session->m_vernierIndex%6];
    data.position=m_session->m_config->m_showConfig.m_pixelUnit*x+m_session->m_config->m_showStartUnit;
    data.positionStr=nsToShowStr(data.position);
    m_session->m_vernier.append(data);
    m_session->m_vernierIndex++;
    emit vernierAppend(data.id);
}

void DrawChannelHeader::paint(QPainter* painter)
{
    if(m_sessionID.isEmpty()||m_isExit)
        return;

    m_highLineBottom=height()-m_highLineTop;
    SessionConfig* config=m_session->m_config;
    SessionShowConfig showConfig=config->m_showConfig;

    QFont font = painter->font();
    font.setPixelSize(11);
    painter->setFont(font);

    //绘制标尺
    painter->setPen(QPen(m_text_color, 1));

    double drawPosition=0;
    qint64 drawNum=0;
    qint32 drawEnd=width();

    double office=qRound64(showConfig.m_pixelUnit*showConfig.m_scalePixel);
    if(office<1)
        office=1;
    double unitOffice=config->m_showStartUnit/office;
    unitOffice=(unitOffice-ceil(unitOffice))*office/showConfig.m_pixelUnit;
    drawPosition=-unitOffice;
    switch(showConfig.m_multiply){
    case -10:
    case 0:
        drawNum=ceil(config->m_showStartUnit/office);
        break;
    default:
        if(showConfig.m_multiply>0)
            drawNum=ceil(config->m_showStartUnit/office)*showConfig.m_multiply;
    }
    while(drawPosition<drawEnd){
        switch(showConfig.m_multiply){
        case -10:
            if(drawNum%10==0)
                drawScale(painter,drawPosition,time2TimeStr(drawNum/10,showConfig.m_showUint),true);
            else
                drawScale(painter,drawPosition,"0."+QString::number(drawNum%10)+showConfig.m_showUint,false);
            drawNum+=1;
            break;
        case 0:
            if(drawNum%10==0)
                drawScale(painter,drawPosition,time2TimeStr(drawNum,showConfig.m_showUint),true);
            else
                drawScale(painter,drawPosition,QString::number(drawNum%10)+showConfig.m_showUint,false);
            drawNum+=1;
            break;
        case 10:
        {
            qint32 tmp=100;
            if(showConfig.m_showUint=="s")
                tmp=60;
            if(drawNum%tmp==0)
                drawScale(painter,drawPosition,time2TimeStr(drawNum,showConfig.m_showUint),true);
            else
                drawScale(painter,drawPosition,QString::number(drawNum%tmp)+showConfig.m_showUint,false);
            drawNum+=10;
            break;
        }
        case 100:
            if(drawNum%600==0)
                drawScale(painter,drawPosition,time2TimeStr(drawNum,showConfig.m_showUint),true);
            else
                drawScale(painter,drawPosition,QString::number(drawNum%600)+showConfig.m_showUint,false);
            drawNum+=100;
            break;
        }
        drawPosition+=showConfig.m_scalePixel;
    }

    font.setPixelSize(10);
    painter->setFont(font);
    vernierMethod(painter);
}


bool DrawChannelHeader::vernierMethod(QPainter *painter)
{
    qint64 start=m_session->m_config->m_showStartUnit;
    double pixelUnit=m_session->m_config->m_showConfig.m_pixelUnit;
    QVector<VernierData>* vernierData=&m_session->m_vernier;
    qint32 top=m_height-24;
    bool ret=false;
    bool firstSelect=true;
    for(qint32 i=0;i<vernierData->count();i++){
        auto data=vernierData->at(i);
        if(!data.visible)
            continue;
        qint32 temp=(data.position-start)/pixelUnit;
        if(temp<-10||temp>m_width+10)
            continue;
        QRect r(temp-8,top,17,24);
        if(painter)
        {
            QPainterPath vernierPath;
            vernierPath.moveTo(r.left(),top);
            vernierPath.lineTo(r.right(),top);
            vernierPath.lineTo(r.right(),top+16);
            vernierPath.lineTo(temp,top+20);
            vernierPath.lineTo(r.left(),top+16);
            QColor color=data.dataColor;
            color.setAlpha(data.isSelect?255:179);
            painter->fillPath(vernierPath, QBrush(color));
            painter->setPen(QPen(color, 1));
            painter->drawLine(temp,m_height-4,temp,m_height);
            QString text=data.id==0?"T":QString::number(data.id);
            qint32 fontWidth=painter->fontMetrics().horizontalAdvance(text);
            painter->setPen(QPen(Qt::white, 1));
            painter->drawText(temp-fontWidth/2,top+12,text);
            if(data.isSelect)
            {
                emit showVernierPopup(data.id, temp+10);
                ret=true;
            }
        }else
        {
            bool isSelect=r.contains(m_mousePoint);
            if(isSelect && !firstSelect)
                isSelect=false;
            else if(isSelect && firstSelect)
                firstSelect=false;
            m_session->m_vernier[i].isSelect=isSelect;
            if(isSelect)
            {
                ret=true;
                emit showVernierPopup(data.id,m_mousePoint.x()+10);
                if(data.id!=0){
                    m_vernierMoveIndex=i;
                    m_vernierMovePosition=data.position;
                }
            }
        }
    }
    if(!ret)
        emit closeVernierPopup();
    return ret;
}

void DrawChannelHeader::hoverMoveEvent(QHoverEvent *event)
{
    if(m_isExit)
        return;
    m_mousePoint=event->pos();
    if(m_isCreating && m_selectVernierIndex<m_session->m_vernier.count())
    {
        m_session->m_vernier[m_selectVernierIndex].position=getAdsorbPosition(qMin(qMax(m_mousePoint.x(),0),m_width));
        m_session->m_vernier[m_selectVernierIndex].positionStr=nsToShowStr(m_session->m_vernier[m_selectVernierIndex].position);
        emit vernierDataChanged(m_session->m_vernier[m_selectVernierIndex].id);
        emit m_session->drawUpdate();
    }
}

void DrawChannelHeader::mousePressEvent(QMouseEvent* event)
{
    if(m_isExit)
        return;
    m_mousePoint=event->pos();
    m_currentButton=event->button();
    if(m_currentButton==Qt::LeftButton){
        if(m_isCreating)
            emit vernierCreateComplete();
        else
            vernierMethod(nullptr);

        if(m_vernierMoveIndex!=-1)
        {
            emit vernierMoveState(true,m_session->m_vernier[m_vernierMoveIndex].id);
            update();
        }
    }else if(m_currentButton==Qt::RightButton){
        if(m_isCreating){
            emit vernierCreateComplete();
            emit deleteVernier(m_session->m_vernier[m_session->m_vernier.count()-1].id);
            if(m_session->m_vernierIndex>1)
                m_session->m_vernierIndex--;
        }
    }
}

void DrawChannelHeader::mouseMoveEvent(QMouseEvent* event)
{
    if(m_isExit)
        return;
    m_mousePoint=event->pos();
    if(m_vernierMoveIndex!=-1)
    {
        m_session->m_vernier[m_vernierMoveIndex].position=m_session->m_config->m_showConfig.m_pixelUnit*qMax(event->x(),0)+m_session->m_config->m_showStartUnit;
        m_session->m_vernier[m_vernierMoveIndex].positionStr=nsToShowStr(m_session->m_vernier[m_vernierMoveIndex].position);
        emit vernierDataChanged(m_session->m_vernier[m_vernierMoveIndex].id);
        emit m_session->drawUpdate();
    }
}

void DrawChannelHeader::mouseReleaseEvent(QMouseEvent* event)
{
    if(m_isExit)
        return;
    m_mousePoint=event->pos();
    if(m_currentButton==Qt::LeftButton){
        vernierMethod(nullptr);
        if(m_vernierMoveIndex!=-1)
            emit vernierMoveState(false,m_session->m_vernier[m_vernierMoveIndex].id);
        m_vernierMoveIndex=-1;
    }else if(m_currentButton==Qt::RightButton){
        if(m_vernierMoveIndex!=-1)
            vernierCancelMove();
    }
    emit m_session->drawUpdate();
}

void DrawChannelHeader::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(m_isExit)
        return;
    m_mousePoint=event->pos();
    m_currentButton=event->button();
    if(m_currentButton==Qt::LeftButton){
        QRect r(m_mousePoint.x()-1,m_mousePoint.y()-1,3,3);
        if(r.contains(event->pos())){
            if(!vernierMethod() && !m_isCreating)
                appendVernierData(event->x());
            update();
        }else if(!m_isCreating)
            appendVernierData(event->x());
    }

}

qint64 DrawChannelHeader::getAdsorbPosition(qint32 x)
{
    return m_session->m_config->m_showConfig.m_pixelUnit*qMax(x,0)+m_session->m_config->m_showStartUnit;
}

#pragma region "QML方法" {
void DrawChannelHeader::init(QString sessionID)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    DataService* dataService = DataService::getInstance();
    m_session=dataService->getSession(sessionID);
    if(m_session)
        m_sessionID=sessionID;
    else
        return;
    if(m_session->m_vernier.count()==0){
        VernierData data;
        data.id=0;
        data.name="Trigger";
        data.dataColor=m_vernierTypeColor[0];
        data.position=0;
        data.positionStr=nsToShowStr(data.position);
        data.isSelect=false;
        data.visible=false;
        m_session->m_vernier.append(data);
        emit vernierAppend(data.id);
    }
    update();
}

void DrawChannelHeader::setTheme(QString theme)
{
    if(theme=="dark")
        m_text_color=QColor::fromRgb(0x8e8e8e);
    else
        m_text_color=QColor::fromRgb(0x383838);
    update();
}

void DrawChannelHeader::createVernier()
{
    if(!m_isCreating){
        m_isCreating=true;
        appendVernierData(0);
        m_selectVernierIndex=m_session->m_vernier.count()-1;
        emit vernierCreate();
    }
}

void DrawChannelHeader::vernierCancelMove()
{
    if(m_vernierMoveIndex!=-1){
        qint32 tmp=m_selectVernierIndex;
        m_selectVernierIndex=-1;
        m_isCreating=false;
        m_session->m_vernier[tmp].position=m_vernierMovePosition;
        emit vernierDataChanged(m_session->m_vernier[tmp].id);
        emit m_session->drawUpdate();
    }
}

bool DrawChannelHeader::isCreating() const
{
    return m_isCreating;
}

void DrawChannelHeader::setIsCreating(bool newIsCreating)
{
    if (m_isCreating == newIsCreating)
        return;
    m_isCreating = newIsCreating;
    if(m_isCreating==false)
        m_selectVernierIndex=-1;
}

#pragma endregion}
