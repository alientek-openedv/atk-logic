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

#include "draw_channel.h"

DrawChannel::DrawChannel(QQuickItem* parent): QQuickPaintedItem(parent) {
    connect(this,SIGNAL(widthChanged()),this,SLOT(onSizeChanged()));
    connect(this,SIGNAL(heightChanged()),this,SLOT(onSizeChanged()));
    m_mousePoint=QPoint(-1,-1);
    m_measurePoint.x=-1;
    m_measurePoint.y=-1;
    m_measureTypeColor[0]=QColor::fromRgb(0xCC0E51);
    m_measureTypeColor[1]=QColor::fromRgb(0xDD1D0A);
    m_measureTypeColor[2]=QColor::fromRgb(0xFF5309);
    m_measureTypeColor[3]=QColor::fromRgb(0xFB9904);
    m_measureTypeColor[4]=QColor::fromRgb(0xFABC05);
    m_measureTypeColor[5]=QColor::fromRgb(0x0547FF);
    m_measureTypeColor[6]=QColor::fromRgb(0x0392CE);
    m_measureTypeColor[7]=QColor::fromRgb(0x66B132);

    m_measureSelectAlpha[0]=230;
    m_measureSelectAlpha[1]=102;
    m_measureNotSelectAlpha[0]=76;
    m_measureNotSelectAlpha[1]=38;
}

DrawChannel::~DrawChannel()
{
    if(m_doubleBuffer!=nullptr)
        delete m_doubleBuffer;
    disconnect();
}

void DrawChannel::onSizeChanged()
{
    m_height=height();
    m_width=width();
    if(m_doubleBuffer!=nullptr && !m_isExit)
        m_doubleBuffer->setSize(m_width, m_height);
}

void DrawChannel::paint(QPainter* painter)
{
    if(m_doubleBuffer!=nullptr && !m_isExit)
    {
        if(m_isDecode)
        {
            if(m_sessionID.isEmpty() || m_decodeID.isEmpty())
                return;
        }
        else
        {
            if(m_sessionID.isEmpty() || m_channelID==-1)
                return;
        }
        m_session->m_segment->appendCite();
        if(m_configRecode!=*m_session->m_config || !m_doubleBuffer->isPixmap() || (!m_isDecode&&m_session->m_segment->m_isFirst[m_channelID]) || m_isDecode)
        {
            if(!m_isDecode)
                m_session->m_segment->m_isFirst[m_channelID]=false;
            m_configRecode=*m_session->m_config;
            m_doubleBuffer->refReshBuffer();
        }
        painter->setRenderHints(QPainter::HighQualityAntialiasing, true);
        QPixmap pixmap=m_doubleBuffer->getPixmap();
        QPainter t;
        t.begin(&pixmap);
        
        if(m_isDecode){
            if(m_doubleBuffer->m_decodeHeight!=m_height)
                setHeight(m_doubleBuffer->m_decodeHeight);
        }else{
            measureMethod(m_mousePoint.x(), &t);
            if(m_measureState>0)
            {
                QColor color=m_measureTypeColor[m_session->m_measureIndex%8];
                color.setAlpha(m_measureSelectAlpha[0]);
                t.setPen(QPen(color, 1));
                if(m_measurePoint.y>=0 && m_measurePoint.x>=0)
                {
                    color.setAlpha(m_measureSelectAlpha[1]);
                    qint64 tempX=(m_measurePoint.start-m_session->m_config->m_showStartUnit)/m_session->m_config->m_showConfig.m_pixelUnit;
                    bool drawStart=false;
                    if(tempX<0)
                        tempX=0;
                    else
                        drawStart=true;
                    t.fillRect(tempX,m_measureOffset,m_measurePoint.y-tempX,m_height-m_measureOffset*2+1,color);
                    t.drawLine(m_measurePoint.y,m_measureOffset,m_measurePoint.y,m_height-m_measureOffset);
                    if(drawStart)
                        t.drawLine(tempX,m_measureOffset,tempX,m_height-m_measureOffset);
                } else if(m_measurePoint.x>=0)
                    t.drawLine(m_measurePoint.x,m_measureOffset,m_measurePoint.x,m_height-m_measureOffset);
            }
            if(m_session->m_segment->isData[m_channelID] && m_mousePoint.x()>=0 && !m_isVernierMove && m_isMouseMeasure)
                drawMouseMeasure(t, m_session->m_segment);
        }
        if(m_currentButton==Qt::RightButton)
            emit mouseZoom(true, m_zoomPoint.x(),m_zoomPoint.y(),m_zoomPointEnd.x()-m_zoomPoint.x(),m_zoomPointEnd.y()-m_zoomPoint.y());
        t.end();
        painter->save();
        painter->drawPixmap(0, 0, pixmap, 0, 0, m_width, m_height);
        painter->restore();
        m_session->m_segment->lessenCite();
    }
}

void DrawChannel::mousePressEvent(QMouseEvent* event)
{
    if(m_isExit)
        return;
    setKeepMouseGrab(true);
    m_mousePoint=event->pos();
    m_zoomPointEnd=m_mousePoint;
    if(m_currentButton==Qt::RightButton)
        zoomMouseRightClick();
    m_zoomPoint=m_zoomPointEnd;
    m_oldPoint=event->globalPos();
    m_showStartUnitRecode=m_session->m_config->m_showStartUnit;
    m_currentButton=event->button();
    if(m_currentButton==Qt::LeftButton)
    {
        m_adsorbChannelID=m_channelID;

        if(m_vernierCreateModel)
            emit vernierCreateComplete();
        else if(m_measureState==1){
            emit measureMoveState(true);
            m_measureState=2;
        }
        else if(m_measureEnter&&m_selectMeasureIndex!=-1){
            emit measureMoveState(true);
            m_measureState=4;
        }
        else if(m_selectVernierIndex!=-1)
        {
            emit vernierMoveState(true,m_session->m_vernier[m_selectVernierIndex].id);
            m_isVernierMove=true;
            emit closeMouseMeasurePopup();
            update();
        }
    }if(m_currentButton==Qt::RightButton)
    {
        if(m_vernierCreateModel)
        {
            emit vernierCreateComplete();
            emit deleteVernier(m_session->m_vernier[m_session->m_vernier.count()-1].id);
            if(m_session->m_vernierIndex>1)
                m_session->m_vernierIndex--;
        }else if(m_measureState!=0)
            setMeasureState(0);
    }
}

void DrawChannel::mouseReleaseEvent(QMouseEvent* event)
{
    if(m_isExit)
        return;
    m_zoomPointEnd=event->pos();
    setKeepMouseGrab(false);
    if(m_currentButton==Qt::LeftButton)
    {
        QRect r(m_oldPoint.x()-1,m_oldPoint.y()-1,3,3);
        if(r.contains(event->globalPos())){
            measureMethod(m_mousePoint.x(),nullptr,true);
            if(m_selectVernierIndex==-1)
                vernierMethod(true);
            emit measureSelectChanged();
            update();
        }
        appendMeasureData();
        setMeasureState(0);
        emit measureMoveState(false);
        if(m_selectVernierIndex!=-1)
        {
            m_isVernierMove=false;
            emit vernierMoveState(false,m_session->m_vernier[m_selectVernierIndex].id);
            update();
        }
    }else if(m_currentButton==Qt::RightButton){
        zoomMouseRightClick();
    }
}

void DrawChannel::mouseMoveEvent(QMouseEvent *event)
{
    if(m_isExit)
        return;
    m_mousePoint=event->pos();
    m_zoomPointEnd=m_mousePoint;
    if(m_currentButton==Qt::LeftButton)
    {
        if(m_measureState==2 && m_measurePoint.x>=0)
        {
            m_measurePoint.end=getAdsorbPosition(event->x());
            m_measurePoint.y=qMin((m_measurePoint.end-m_session->m_config->m_showStartUnit)/m_session->m_config->m_showConfig.m_pixelUnit, (qreal)m_session->m_config->m_width+10);
            update();
        }else if(m_measureEnter && m_selectMeasureIndex>-1 &&m_measureState==4)
        {
            qint64 temp=getAdsorbPosition(event->x());
            qint64* pTemp;
            if(m_measureEnter==1)
                pTemp=&m_session->m_measure[m_selectMeasureIndex].start;
            else
                pTemp=&m_session->m_measure[m_selectMeasureIndex].end;
            *pTemp=temp;
            if(m_session->m_measure[m_selectMeasureIndex].start==m_session->m_measure[m_selectMeasureIndex].end)
            {
                if((*pTemp)-1>=0)
                    (*pTemp)--;
                else
                    (*pTemp)++;
            }
            if(m_session->m_measure[m_selectMeasureIndex].start>m_session->m_measure[m_selectMeasureIndex].end)
            {
                m_session->m_measure[m_selectMeasureIndex].start ^=m_session->m_measure[m_selectMeasureIndex].end;
                m_session->m_measure[m_selectMeasureIndex].end ^= m_session->m_measure[m_selectMeasureIndex].start;
                m_session->m_measure[m_selectMeasureIndex].start ^= m_session->m_measure[m_selectMeasureIndex].end;
                if(m_measureEnter==1)
                    m_measureEnter=2;
                else
                    m_measureEnter=1;
            }
            m_session->m_measure[m_selectMeasureIndex].isCalc=true;
            m_session->calcMeasureData(m_session->m_measure[m_selectMeasureIndex].id, m_channelID);
            update();
            emit measureDataChanged(m_session->m_measure[m_selectMeasureIndex].id);
        }else if(m_selectVernierIndex!=-1)
        {
            emit getAdsorbChannelID(event->y());
            vernierMove(event->x());
        }
        else{
            emit setLiveFollowing(false);
            SessionConfig* config=m_session->m_config;
            SessionShowConfig showConfig=config->m_showConfig;
            QPoint newPoint=event->globalPos()-m_oldPoint;
            config->m_showStartUnit=m_showStartUnitRecode-newPoint.x()*showConfig.m_pixelUnit;
            if(config->m_showStartUnit<0)
                config->m_showStartUnit=0;

            double office=15;
            if(config->m_showConfig.m_pixelUnit<1)
                office=1/config->m_showConfig.m_pixelUnit;

            if(config->m_showStartUnit+(config->m_width-office)*showConfig.m_pixelUnit>config->m_maxUnit)
            {
                config->m_showStartUnit=config->m_maxUnit-(config->m_width-office)*showConfig.m_pixelUnit;
                if(config->m_showStartUnit<0)
                    config->m_showStartUnit=0;
            }
            emit m_session->drawUpdate();
        }
    }else if(m_currentButton==Qt::RightButton)
        update();
}

void DrawChannel::hoverMoveEvent(QHoverEvent* event)
{
    if(m_isExit)
        return;
    m_mousePoint=event->pos();
    if(m_vernierCreateModel)
        vernierMove(m_mousePoint.x());
    else{
        if(m_measureState==1)
        {
            m_measurePoint.start=getAdsorbPosition(m_mousePoint.x());
            m_measurePoint.x=qMin((m_measurePoint.start-m_session->m_config->m_showStartUnit)/m_session->m_config->m_showConfig.m_pixelUnit, (qreal)m_session->m_config->m_width+10);
            m_measurePoint.x=qMax(m_measurePoint.x,0);
        }
        vernierMethod();
    }
}


void DrawChannel::hoverEnterEvent(QHoverEvent *event)
{
    if(m_isExit)
        return;
    Q_UNUSED(event);
    emit hoverEnter();
}

void DrawChannel::hoverLeaveEvent(QHoverEvent *event)
{
    if(m_isExit)
        return;
    Q_UNUSED(event);
    m_mousePoint=QPoint(-1,-1);
    m_measurePoint.x=-1;
    m_measurePoint.y=-1;
    m_measureEnter=0;
    m_selectMeasureIndex=-1;
    update();
    emit hoverLeave();
}

void DrawChannel::init(QString sessionID)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    DataService* dataService = DataService::getInstance();
    m_session=dataService->getSession(sessionID);
    if(m_session)
        m_sessionID=sessionID;
}

void DrawChannel::refreshDoubleBuffer()
{
    if(m_doubleBuffer!=nullptr)
        m_doubleBuffer->refReshBuffer();
}

void DrawChannel::appendMeasureData()
{
    if(m_measurePoint.x>=0 && m_measurePoint.y>=0 && m_measurePoint.x!=m_measurePoint.y && m_measureState>0){
        MeasureData data;
        if(m_measurePoint.start<m_measurePoint.end)
        {
            data.start=m_measurePoint.start;
            data.end=m_measurePoint.end;
        }else{
            data.start=m_measurePoint.end;
            data.end=m_measurePoint.start;
        }
        if(data.start==data.end)
            return;
        m_lastMeasureID=m_session->m_measureIndex;
        data.id=m_lastMeasureID;
        data.channelID=m_channelID;
        data.name="M"+QString::number(data.id);
        data.dataColor=m_measureTypeColor[data.id%8];
        data.isSelect=true;
        data.isCalc=true;

        QVector<MeasureData> measure=m_session->m_measure;
        qint32 measureCount=measure.count()-1;
        for(qint32 i=measureCount;i>=0;i--){
            if(measure[i].isSelect)
                m_session->m_measure[i].isSelect=false;
        }
        m_session->m_measure.append(data);
        m_session->m_measureIndex++;
        m_session->calcMeasureData(data.id, m_channelID);
        setMeasureState(3);
    }
}

void DrawChannel::measureMethod(qint32 mouseX, QPainter *painter, bool isClick)
{
    QVector<MeasureData>* measureList=&m_session->m_measure;
    double pixelUnit=m_session->m_config->m_showConfig.m_pixelUnit;
    qint64 showStart=m_session->m_config->m_showStartUnit;
    qint64 showEnd=showStart+pixelUnit*m_width;
    qint32 measureCount=measureList->count()-1;
    bool firstSelect=true;
    bool isMeasureEnter=false;
    for(qint32 i=measureCount;i>=0;i--){
        MeasureData measure=measureList->at(i);
        if(measure.end>showStart && measure.start<=showEnd && measure.channelID==m_channelID){
            qint64 drawStart=(measure.start-showStart)/pixelUnit;
            qint64 drawEnd=(measure.end-showStart)/pixelUnit;
            if(painter){
                QColor color=measure.dataColor;
                color.setAlpha(measure.isSelect?m_measureSelectAlpha[0]:m_measureNotSelectAlpha[0]);
                painter->setPen(QPen(color, 1));
                if(drawStart>=0)
                    painter->drawLine(drawStart,m_measureOffset,drawStart,m_height-m_measureOffset);
                else
                    drawStart=0;
                if(drawEnd<=m_width-20)
                    painter->drawLine(drawEnd,m_measureOffset,drawEnd,m_height-m_measureOffset);
                else
                    drawEnd=m_width-20;
                color.setAlpha(measure.isSelect?m_measureSelectAlpha[1]:m_measureNotSelectAlpha[1]);
                if(measure.isSelect){
                    qint32 temp=drawStart+((drawEnd-drawStart)/2-394/2);
                    if((temp>=-394 && temp<=m_width+394)||(measure.offset>=-394 && measure.offset<=m_width+394))
                    {
                        m_session->m_measure[i].offset=temp;
                        emit measureDataChanged(measure.id);
                    }
                }
                painter->fillRect(drawStart,m_measureOffset,qCeil(drawEnd-drawStart),m_height-m_measureOffset*2+1,color);
                if(m_measureState!=4 && mouseX>-1){
                    if(drawStart-2<mouseX&&drawStart+2>mouseX){
                        
                        m_selectMeasureIndex=i;
                        m_measureEnter=1;
                        isMeasureEnter=true;
                        setShowCursor(true);
                    }else if(drawEnd-2<mouseX&&drawEnd+2>mouseX){
                        
                        m_selectMeasureIndex=i;
                        m_measureEnter=2;
                        isMeasureEnter=true;
                        setShowCursor(true);
                    }else if(!isMeasureEnter && m_selectVernierIndex==-1)
                    {
                        m_measureEnter=0;
                        setShowCursor(false);
                    }
                }
            }else
            {
                bool isSelect=(drawStart+1<mouseX&&drawEnd-1>mouseX);
                if(isSelect && !firstSelect)
                    isSelect=false;
                else if(isSelect && firstSelect)
                    firstSelect=false;
                m_session->m_measure[i].isSelect=isSelect;
            }
        }else if(!painter && isClick)
            m_session->m_measure[i].isSelect=false;
        else if(painter && measure.isSelect)
        {
            qint32 drawStart=qMax((qint32)((measure.start-showStart)/pixelUnit),0);
            qint32 drawEnd=(measure.end-showStart)/pixelUnit;
            if(drawEnd<=0 || drawStart>=m_width-20)
            {
                m_session->m_measure[i].offset=-500;
                emit measureDataChanged(measure.id);
            }
        }
    }
}

void DrawChannel::vernierMethod(bool isClick)
{
    qint64 start=m_session->m_config->m_showStartUnit;
    double pixelUnit=m_session->m_config->m_showConfig.m_pixelUnit;
    QVector<VernierData>* vernierData=&m_session->m_vernier;
    qint32 count=vernierData->count();
    QRect r(m_mousePoint.x()-2,m_mousePoint.y()-2,5,5);
    bool isShow=false;
    if(!isClick)
        m_selectVernierIndex=-1;
    for(qint32 i=0;i<count;i++){
        auto data=vernierData->at(i);
        qint32 temp=(data.position-start)/pixelUnit;
        if(isClick)
            m_session->m_vernier[i].isSelect=false;
        else if(r.contains(QPoint(temp,m_mousePoint.y())) && data.visible && data.id!=0)
        {
            isShow=true;
            m_selectVernierIndex=i;
        }
    }
    setShowCursor(isShow);
    if(isClick)
        emit closeVernierPopup();
    emit m_session->drawUpdate();
}

void DrawChannel::drawMousePopup(qint64 start, qint64 end, bool isTop, QPainter &t)
{
    t.setPen(QPen(m_mouseMeasureLineColor, 1));
    qint64 showStartUnit=m_session->m_config->m_showStartUnit;
    double pixelUnit=m_session->m_config->m_showConfig.m_pixelUnit;
    qint64 drawLeft=(start-showStartUnit)/pixelUnit;
    qint64 drawRight=(end-showStartUnit)/pixelUnit;
    qint32 top=isTop?7:m_height-14;
    qint64 drawLeftR=drawLeft,drawRightR=drawRight;
    if(drawLeft<-15)
        drawLeft=-15;
    if(drawLeftR<0)
        drawLeftR=0;
    if(drawRight>m_width+15)
        drawRight=m_width+15;
    if(drawRightR>m_width-20)
        drawRightR=m_width-20;
    qint64 drawLength=drawRight-drawLeft;
    t.drawLine(drawLeft,top,drawLeft,top+7);
    t.drawLine(drawRight,top,drawRight,top+7);
    if(drawLength>6){
        
        t.drawImage(drawLeft+2,top+1,m_arrowsImage[0]);
        
        t.drawImage(drawRight-7,top+1,m_arrowsImage[1]);
        t.drawLine(drawLeft+2,top+3,drawRight-2,top+3);
    }else
        t.drawLine(drawLeft,top+3,drawRight,top+3);
    QString drawStr=nsToShowStr(end-start,5);
    qint32 fontWidth=t.fontMetrics().width(drawStr);
    if(drawRightR-drawLeftR>fontWidth+10)
        drawRight=drawLeftR+((drawRightR-drawLeftR)-fontWidth-10)/2+1;
    else if(drawRight+fontWidth+30>m_width)
        drawRight=m_width-fontWidth-30;
    else
        drawRight+=1;
    t.fillRect(drawRight,top+(isTop?9:-17),fontWidth+10,16,m_mouseMeasureBackgroundColor);
    t.setPen(QPen(m_mouseMeasureTextColor, 1));
    t.drawText(drawRight+5,top+(isTop?21:-5),drawStr);
}

qint64 DrawChannel::getAdsorbPosition(qint32 x)
{
    m_session->m_segment->appendCite();
    qint32 multiply=m_session->m_segment->GetMultiply();
    if(multiply<1)
        multiply=1;
    qint64 start=(m_session->m_config->m_showConfig.m_pixelUnit*qMax(x,0)+m_session->m_config->m_showStartUnit);
    if(m_adsorbChannelID<0){
        m_session->m_segment->lessenCite();
        return start;
    }
    qint64 left=m_session->m_segment->GetDataEnd(start/multiply,m_adsorbChannelID,true).position*multiply,right=m_session->m_segment->GetDataEnd(start/multiply,m_adsorbChannelID,false).position*multiply;
    qint32 leftPix=(left-m_session->m_config->m_showStartUnit)/m_session->m_config->m_showConfig.m_pixelUnit;
    qint32 rightPix=(right-m_session->m_config->m_showStartUnit)/m_session->m_config->m_showConfig.m_pixelUnit;
    m_session->m_segment->lessenCite();
    if(rightPix-leftPix>6)
    {
        if(leftPix+3>=x)
            return left;
        else if(rightPix-3<=x)
            return right;
        else
            return start;
    }else
        return start;
}

void DrawChannel::drawMouseMeasure(QPainter &t, Segment *segment)
{
    qint32 multiply=segment->GetMultiply();
    if(multiply<=0||m_isRunCollect)
    {
        emit closeMouseMeasurePopup();
        return;
    }
    qint64 showStartUnit=m_session->m_config->m_showStartUnit;
    double pixelUnit=m_session->m_config->m_showConfig.m_pixelUnit;
    qint64 position=(pixelUnit*m_mousePoint.x()+showStartUnit)/multiply;
    if(position>=segment->GetMaxSample(m_channelID)){
        emit closeMouseMeasurePopup();
        return;
    }
    qint64 maxShow=m_session->m_config->m_maxUnit/multiply;
    bool startData=segment->GetSample(position,m_channelID);
    qint64 dataStart=segment->GetDataEnd(position,m_channelID,true).position;
    qint64 dataEnd=segment->GetDataEnd(position,m_channelID,false).position;
    qint64 dataEnd2=segment->GetDataEnd(dataEnd,m_channelID,false).position;
    if(dataStart>maxShow)
        dataStart=maxShow;
    if(dataEnd>maxShow)
        dataEnd=maxShow;
    if(dataEnd2>maxShow)
        dataEnd2=maxShow;
    if(dataStart==dataEnd && dataEnd==dataEnd2)
    {
        emit closeMouseMeasurePopup();
        return;
    }
    if(dataStart==dataEnd)
    {
        dataEnd=maxShow;
        dataEnd2=dataEnd;
    }
    dataStart*=multiply;
    dataEnd*=multiply;
    dataEnd2*=multiply;
    drawMousePopup(dataStart,dataEnd,true,t);
    if(dataEnd!=dataEnd2)
        drawMousePopup(dataStart,dataEnd2,false,t);
    qint64 period=dataEnd2-dataStart;
    qreal duty;
    if(startData)
        duty=(dataEnd-dataStart)/(qreal)period*100;
    else
        duty=(dataEnd2-dataEnd)/(qreal)period*100;
    emit showMouseMeasurePopup(m_mousePoint.x(),nsToShowStr(period,4),hzToShowStr(1000000000/(qreal)period,2),
                               QString::asprintf("%.2f", duty)+"%");
}

void DrawChannel::vernierMove(qint32 x)
{
    if(m_session->m_vernier.count()>m_selectVernierIndex){
        m_session->m_vernier[m_selectVernierIndex].position=getAdsorbPosition(qMin(qMax(x,0),m_width));
        m_session->m_vernier[m_selectVernierIndex].positionStr=nsToShowStr(m_session->m_vernier[m_selectVernierIndex].position);
        emit vernierDataChanged(m_session->m_vernier[m_selectVernierIndex].id);
        emit m_session->drawUpdate();
    }
}

void DrawChannel::zoomMouseRightClick()
{
    if(qAbs(m_zoomPoint.x()-m_zoomPointEnd.x())>2){
        qint64 start=m_zoomPoint.x()*m_session->m_config->m_showConfig.m_pixelUnit+m_session->m_config->m_showStartUnit;
        qint64 end=qMax(qMin(m_zoomPointEnd.x(), m_width),0)*m_session->m_config->m_showConfig.m_pixelUnit+m_session->m_config->m_showStartUnit;
        if(start>end)
        {
            start^=end;
            end^=start;
            start^=end;
        }
        start=qMax(start,0ll);
        end=qMin(end,m_session->m_config->m_maxUnit);
        emit showViewScope(start,end);
    }
    m_currentButton=Qt::LeftButton;
    emit mouseZoom(false,0,0,0,0);
}


#pragma region QML方法 {
void DrawChannel::init(QString sessionID,int channelID)
{
    m_channelID=channelID;
    m_adsorbChannelID=channelID;
    m_isDecode=false;
    init(sessionID);
    m_doubleBuffer=new DoubleBuffering(sessionID, channelID);
    m_doubleBuffer->m_DataColor=m_DataColor;
    m_doubleBuffer->m_decodeChannelDesc=m_decodeChannelDesc;
    m_doubleBuffer->setSize(m_width, m_height);
    update();
}

void DrawChannel::decodeInit(QString sessionID, QString decodeID)
{
    m_decodeID=decodeID;
    m_isDecode=true;
    init(sessionID);
    m_doubleBuffer=new DoubleBuffering(sessionID, decodeID);
    m_doubleBuffer->m_DataColor=m_DataColor;
    m_doubleBuffer->setSize(m_width, m_height);
    update();
}

qint32 DrawChannel::getLastMeasureID()
{
    return m_lastMeasureID;
}

void DrawChannel::forceRefresh()
{
    if(m_doubleBuffer!=nullptr)
        m_doubleBuffer->refReshBuffer();
    update();
}

void DrawChannel::setTheme(QString theme)
{
    m_theme=theme;
    m_arrowsImage[0].load(":/resource/icon/"+m_theme+"/ArrowsLeft.png");
    m_arrowsImage[1].load(":/resource/icon/"+m_theme+"/ArrowsRight.png");
    if(m_doubleBuffer!=nullptr){
        if(m_theme=="dark")
        {
            m_doubleBuffer->m_TextColor=QColor::fromRgb(0xffffff);
            m_doubleBuffer->m_DashLineColor=QColor::fromRgb(0x37373b);
        }
        else
        {
            m_doubleBuffer->m_TextColor=QColor::fromRgb(0x383838);
            m_doubleBuffer->m_DashLineColor=QColor::fromRgb(0xd5d5d5);
        }
    }
    if(m_theme=="dark"){
        m_mouseMeasureLineColor=QColor::fromRgb(0x8e8e8e);
        m_mouseMeasureBackgroundColor=QColor::fromRgb(0xffffff);
        m_mouseMeasureTextColor=QColor::fromRgb(0x383838);
    }else{
        m_mouseMeasureLineColor=QColor::fromRgb(0x545454);
        m_mouseMeasureBackgroundColor=QColor::fromRgb(0x848484);
        m_mouseMeasureTextColor=QColor::fromRgb(0xffffff);
    }

    forceRefresh();
}

#pragma endregion}

#pragma region QML属性 {
const QColor &DrawChannel::DataColor() const
{
    return m_DataColor;
}

void DrawChannel::setDataColor(const QColor &newDataColor)
{
    if (m_DataColor == newDataColor)
        return;
    m_DataColor = newDataColor;
    if(m_doubleBuffer!=nullptr)
        m_doubleBuffer->m_DataColor=m_DataColor;
}

qint32 DrawChannel::measureState() const
{
    return m_measureState;
}

void DrawChannel::setMeasureState(qint32 newMeasureState)
{
    if (m_measureState == newMeasureState)
        return;
    if(newMeasureState==-1)
    {
        m_measureState=0;
        m_measurePoint.x=-1;
        m_measurePoint.y=-1;
    }
    else{
        if(newMeasureState==0)
        {
            m_measurePoint.x=-1;
            m_measurePoint.y=-1;
        }
        m_adsorbChannelID=m_channelID;
        m_measureEnter=0;
        m_selectMeasureIndex=-1;
        m_measureState = newMeasureState;
        emit measureStateChanged();
    }
}

bool DrawChannel::showCursor() const
{
    return m_showCursor;
}

void DrawChannel::setShowCursor(bool newShowCursor)
{
    if (m_showCursor == newShowCursor)
        return;
    m_showCursor = newShowCursor;
    emit showCursorChanged();
}

bool DrawChannel::isMouseMeasure() const
{
    return m_isMouseMeasure;
}

void DrawChannel::setIsMouseMeasure(bool newIsMouseMeasure)
{
    if (m_isMouseMeasure == newIsMouseMeasure)
        return;
    m_isMouseMeasure = newIsMouseMeasure;
    emit isMouseMeasureChanged();
}

QString DrawChannel::decodeChannelDesc() const
{
    return m_decodeChannelDesc;
}

void DrawChannel::setDecodeChannelDesc(const QString &newDecodeChannelDesc)
{
    if (m_decodeChannelDesc == newDecodeChannelDesc)
        return;
    m_decodeChannelDesc = newDecodeChannelDesc;
    if(m_doubleBuffer!=nullptr){
        m_doubleBuffer->m_decodeChannelDesc=newDecodeChannelDesc;
        m_doubleBuffer->refReshBuffer();
    }
    emit decodeChannelDescChanged();
}

bool DrawChannel::vernierCreateModel() const
{
    return m_vernierCreateModel;
}

void DrawChannel::setVernierCreateModel(bool newVernierCreateModel)
{
    if (m_vernierCreateModel == newVernierCreateModel)
        return;
    if(newVernierCreateModel)
        m_selectVernierIndex=m_session->m_vernier.count()-1;
    else
        m_selectVernierIndex=-1;
    m_vernierCreateModel = newVernierCreateModel;
    emit vernierCreateModelChanged();
    update();
}

qint32 DrawChannel::adsorbChannelID() const
{
    return m_adsorbChannelID;
}

void DrawChannel::setAdsorbChannelID(qint32 newAdsorbChannelID)
{
    if (m_adsorbChannelID == newAdsorbChannelID || (m_channelID<0 && !m_isDecode))
        return;
    if(newAdsorbChannelID<0||newAdsorbChannelID>=m_session->m_channelCount){
        if(newAdsorbChannelID!=m_channelID)
            setAdsorbChannelID(m_channelID);
    }
    else
    {
        m_adsorbChannelID = newAdsorbChannelID;
        emit adsorbChannelIDChanged();
    }
}

bool DrawChannel::isRunCollect() const
{
    return m_isRunCollect;
}

void DrawChannel::setIsRunCollect(bool newIsRunCollect)
{
    if (m_isRunCollect == newIsRunCollect)
        return;
    m_isRunCollect = newIsRunCollect;
    emit isRunCollectChanged();
}

bool DrawChannel::isExit() const
{
    return m_isExit;
}

void DrawChannel::setIsExit(bool newIsExit)
{
    if (m_isExit == newIsExit)
        return;
    m_isExit = newIsExit;
    if(m_doubleBuffer!=nullptr)
        m_doubleBuffer->m_isExit=newIsExit;
    emit isExitChanged();
}

#pragma endregion}
