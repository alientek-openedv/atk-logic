#include "draw_background_floor.h"

DrawBackgroundFloor::DrawBackgroundFloor(QQuickItem *parent) : QQuickPaintedItem{parent}
{
    connect(this,SIGNAL(widthChanged()),this,SLOT(onSizeChanged()));
    connect(this,SIGNAL(heightChanged()),this,SLOT(onSizeChanged()));
    m_mousePoint=QPoint(-1,-1);
}

void DrawBackgroundFloor::paint(QPainter* painter)
{
    Q_UNUSED(painter);
    if(m_sessionID.isEmpty()||m_isExit)
        return;
    if(m_currentButton==Qt::RightButton)
        emit mouseZoom(true, m_zoomPoint.x(),m_zoomPoint.y(),m_zoomPointEnd.x()-m_zoomPoint.x(),m_zoomPointEnd.y()-m_zoomPoint.y());
}

void DrawBackgroundFloor::mousePressEvent(QMouseEvent* event)
{
    if(m_isExit)
        return;
    setKeepMouseGrab(true);
    if(m_crossChannelMeasureState!=0){
        emit crossChannelMeasureState(true);
        return;
    }
    m_mousePoint=event->pos();
    m_oldPoint=event->globalPos();
    m_zoomPoint=m_mousePoint;
    m_zoomPointEnd=m_zoomPoint;
    m_currentButton=event->button();
    if(m_currentButton==Qt::LeftButton)
    {
        if(m_vernierCreateModel)
            emit vernierCreateComplete();
        else if(m_selectVernierIndex!=-1)
        {
            emit vernierMoveState(true,m_session->m_vernier[m_selectVernierIndex].id);
            update();
        }
    }else if(m_currentButton==Qt::RightButton)
    {
        if(m_vernierCreateModel)
        {
            emit vernierCreateComplete();
            emit deleteVernier(m_session->m_vernier[m_session->m_vernier.count()-1].id);
            if(m_session->m_vernierIndex>1)
                m_session->m_vernierIndex--;
        }
    }
}

void DrawBackgroundFloor::mouseMoveEvent(QMouseEvent* event)
{
    if(m_isExit)
        return;
    m_mousePoint=event->pos();
    m_zoomPointEnd=m_mousePoint;
    if(m_currentButton==Qt::LeftButton)
    {
        if(m_selectVernierIndex!=-1){
            emit getAdsorbChannelID(event->y());
            vernierMove(event->x());
        }
    }else if(m_currentButton==Qt::RightButton)
        update();
}

void DrawBackgroundFloor::mouseReleaseEvent(QMouseEvent* event)
{
    if(m_isExit)
        return;
    m_mousePoint=event->pos();
    m_zoomPointEnd=m_mousePoint;
    setKeepMouseGrab(false);
    if(m_currentButton==Qt::LeftButton)
    {
        QRect r(m_oldPoint.x()-1,m_oldPoint.y()-1,3,3);
        if(r.contains(event->globalPos())){
            if(m_selectVernierIndex==-1)
                vernierMethod(true);
        }
        for(qint32 i=0;i<m_session->m_measure.count();i++)
            m_session->m_measure[i].isSelect=false;
        emit measureSelectChanged();
        if(m_selectVernierIndex!=-1)
        {
            emit vernierMoveState(false,m_session->m_vernier[m_selectVernierIndex].id);
            update();
        }
    }else if(m_currentButton==Qt::RightButton){
        if(m_selectVernierIndex!=-1)
            vernierCancelMove();
        else{
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
    }
}

void DrawBackgroundFloor::hoverLeaveEvent(QHoverEvent *event)
{
    if(m_isExit)
        return;
    Q_UNUSED(event);
    m_mousePoint=QPoint(-1,-1);
    emit hoverLeave();
}

void DrawBackgroundFloor::hoverMoveEvent(QHoverEvent* event)
{
    if(m_isExit)
        return;
    m_mousePoint=event->pos();
    if(m_vernierCreateModel)
        vernierMove(m_mousePoint.x());
    else if(m_crossChannelMeasureState)
        emit sendCrossChannelMeasurePosition(2, m_mousePoint.x(), m_mousePoint.y(), m_mousePoint.y(), getAdsorbPosition(m_mousePoint.x()), false);
    else
        vernierMethod();
}

void DrawBackgroundFloor::vernierMethod(bool isClick)
{
    if(!isEnabled())
        return;
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
            m_selectVernierPosition=data.position;
        }
    }
    setShowCursor(isShow);
    if(isClick)
        emit closeVernierPopup();
    emit m_session->drawUpdate();
}

void DrawBackgroundFloor::vernierMove(qint32 x)
{
    m_session->m_vernier[m_selectVernierIndex].position=getAdsorbPosition(qMin(qMax(x,0),m_width));
    m_session->m_vernier[m_selectVernierIndex].positionStr=nsToShowStr(m_session->m_vernier[m_selectVernierIndex].position);
    emit vernierDataChanged(m_session->m_vernier[m_selectVernierIndex].id);
    emit m_session->drawUpdate();
}

qint64 DrawBackgroundFloor::getAdsorbPosition(qint32 x)
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

void DrawBackgroundFloor::onSizeChanged()
{
    m_height=height();
    m_width=width();
}

#pragma region "QML方法" {
void DrawBackgroundFloor::init(QString sessionID)
{
    DataService* dataService = DataService::getInstance();
    m_session=dataService->getSession(sessionID);
    m_adsorbChannelID=-1;
    if(m_session)
        m_sessionID=sessionID;
    update();
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
}

void DrawBackgroundFloor::vernierCancelMove()
{
    if(m_selectVernierIndex!=-1){
        qint32 tmp=m_selectVernierIndex;
        m_selectVernierIndex=-1;
        m_vernierCreateModel=false;
        m_session->m_vernier[tmp].position=m_selectVernierPosition;
        emit vernierDataChanged(m_session->m_vernier[tmp].id);
        emit m_session->drawUpdate();
    }
}

void DrawBackgroundFloor::setCrossChannelMeasureState(bool isStop)
{
    m_crossChannelMeasureState=!isStop;
}

bool DrawBackgroundFloor::showCursor() const
{
    return m_showCursor;
}

void DrawBackgroundFloor::setShowCursor(bool newShowCursor)
{
    if (m_showCursor == newShowCursor)
        return;
    m_showCursor = newShowCursor;
    emit showCursorChanged();
}

bool DrawBackgroundFloor::vernierCreateModel() const
{
    return m_vernierCreateModel;
}

void DrawBackgroundFloor::setVernierCreateModel(bool newVernierCreateModel)
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

qint32 DrawBackgroundFloor::adsorbChannelID() const
{
    return m_adsorbChannelID;
}

void DrawBackgroundFloor::setAdsorbChannelID(qint32 newAdsorbChannelID)
{
    if (m_adsorbChannelID == newAdsorbChannelID)
        return;
    m_adsorbChannelID = newAdsorbChannelID;
    emit adsorbChannelIDChanged();
}

bool DrawBackgroundFloor::isExit() const
{
    return m_isExit;
}

void DrawBackgroundFloor::setIsExit(bool newIsExit)
{
    if (m_isExit == newIsExit)
        return;
    m_isExit = newIsExit;
    emit isExitChanged();
}
#pragma endregion}
