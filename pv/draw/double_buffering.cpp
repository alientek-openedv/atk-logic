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

#include "double_buffering.h"

DoubleBuffering::DoubleBuffering(QString sessionID, int channelID, QObject* parent)
    : QObject(parent), m_sessionID(sessionID), m_channelID(channelID)
{
    init(sessionID);
}

DoubleBuffering::DoubleBuffering(QString sessionID, QString decodeID, QObject* parent)
    : QObject(parent), m_sessionID(sessionID), m_decodeID(decodeID), m_isDecode(true)
{
    init(sessionID);
}

DoubleBuffering::~DoubleBuffering()
{
    if(m_pPixmap!=nullptr)
        delete m_pPixmap;
    if(m_pPixmap2!=nullptr)
        delete m_pPixmap2;
    disconnect();
}

void DoubleBuffering::init(QString sessionID)
{
    DataService* dataService = DataService::getInstance();
    m_session=dataService->getSession(sessionID);

    m_decodeTypeColor[0]=QColor::fromRgb(0xFF,0x26,0x68);
    m_decodeTypeColor[1]=QColor::fromRgb(0xF6,0x6A,0x32);
    m_decodeTypeColor[2]=QColor::fromRgb(0xFC,0xAE,0x3E);
    m_decodeTypeColor[3]=QColor::fromRgb(0xFB,0xCA,0x47);
    m_decodeTypeColor[4]=QColor::fromRgb(0xFC,0xE9,0x4F);
    m_decodeTypeColor[5]=QColor::fromRgb(0xCD,0xF0,0x40);
    m_decodeTypeColor[6]=QColor::fromRgb(0x8A,0xE2,0x34);
    m_decodeTypeColor[7]=QColor::fromRgb(0x4E,0xDC,0x44);
    m_decodeTypeColor[8]=QColor::fromRgb(0x55,0xD7,0x95);
    m_decodeTypeColor[9]=QColor::fromRgb(0x64,0xD1,0xD2);
    m_decodeTypeColor[10]=QColor::fromRgb(0x72,0x9F,0xCF);
    m_decodeTypeColor[11]=QColor::fromRgb(0xD4,0x76,0xC4);
    m_decodeTypeColor[12]=QColor::fromRgb(0x9D,0x79,0xB9);
    m_decodeTypeColor[13]=QColor::fromRgb(0xAD,0x7F,0xA8);
    m_decodeTypeColor[14]=QColor::fromRgb(0xC2,0x62,0x9B);
    m_decodeTypeColor[15]=QColor::fromRgb(0xD7,0x47,0x6F);
}

void DoubleBuffering::setSize(qint32 width, qint32 height)
{
    m_width=qMax(width,1);
    m_height=qMax(height,1);

    m_trianglePath.clear();
    qint32 temp=m_height/2;
    m_trianglePath.moveTo(0, temp-4);
    m_trianglePath.lineTo(4,temp);
    m_trianglePath.lineTo(0,temp+4);
    refReshBuffer();
}

void DoubleBuffering::refReshBuffer()
{
    if(m_width<1||m_height<1||m_isExit)
        return;
    if(m_pPixmap2!=nullptr)
        delete m_pPixmap2;
    m_pPixmap2=m_pPixmap;
    m_pPixmap=new QPixmap(m_width, m_height);
    m_pPixmap->fill(Qt::transparent);

    QPainter painter;
    if(painter.begin(m_pPixmap))
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
        
        if(m_isDecode)
            DrawDecode(painter);
        else
            DrawLogic(painter);
        m_session->m_segment->lessenCite();
        painter.end();
    }
}

void DoubleBuffering::DrawChannelLine(QPainter *painter, double start, double end, bool isHigh,bool isEndLine)
{
    qint32 top=15;
    qint32 bottom=m_height-15;
    if(isHigh)
        painter->drawLine(QLineF(start, top, end, top));
    else
        painter->drawLine(QLineF(start, bottom, end, bottom));
    if(!isEndLine)
        painter->drawLine(QLineF(end, bottom, end, top));
}

void DoubleBuffering::DrawChannelLine(QPainter *painter, qint32 x, qint8 data)
{
    qint32 top=15;
    qint32 bottom=m_height-15;
    switch (data) {
    case 0:
        painter->drawLine(QLineF(x, bottom, x, bottom));
        break;
    case 1:
        painter->drawLine(QLineF(x, top, x, top));
        break;
    case 2:
        painter->drawLine(QLineF(x, bottom, x, top));
        break;
    }
}

void DoubleBuffering::DrawDecode(QPainter &painter)
{
    
    SessionConfig* config=m_session->m_config;
    SessionShowConfig showConfig=config->m_showConfig;
    qint32 multiply=m_session->m_segment->GetMultiply();
    qint64 maxUnit=config->m_maxUnit;
    qint64 startUnit=config->m_showStartUnit;
    {
        qint64 temp=showConfig.m_pixelUnit*(m_width+5)+startUnit+3;
        if(temp>maxUnit)
            temp=maxUnit;
        maxUnit=temp;
    }
    if(multiply<=0)
        multiply=1;
    QFont font = painter.font();
    font.setPixelSize(16);
    painter.setPen(QPen(m_TextColor, 0));
    painter.setFont(font);
    qint32 rowCount=0;
    QString errorString="";
    if(m_session->m_isShowDecode){
        DecodeController* decode=m_session->getDecode(m_decodeID);
        if(!decode)
            return;
        if(decode->m_isReportedError)
            errorString=QObject::tr("数据解码出错，请检查协议参数");
        if(errorString.isEmpty()){
            if(showConfig.m_pixelUnit<1000*multiply){
                for(QString &key : decode->m_rowList.keys()){
                    decode->m_rowList[key]->mutex.lock();
                    qint32 count_=decode->m_rowList[key]->row.count();
                    decode->m_rowList[key]->mutex.unlock();
                    for(qint32 i=0;i<count_;i++){
                        decode->m_rowList[key]->mutex.lock();
                        DecodeRow* row=&decode->m_rowList[key]->row[i];
                        qint32 dataCount_=row->data.length();
                        decode->m_rowList[key]->mutex.unlock();
                        if(row->isShow && dataCount_>0)
                        {
                            if(m_isExit)
                                return;
                            qint32 fontWidth=painter.fontMetrics().width(key+":"+row->desc)+2;
                            bool isShow=false;
                            
                            qint32 left_=0,right_=0;
                            decode->findDecodeRowDataNode(decode->m_rowList[key],i,left_,right_,startUnit/multiply);
                            
                            qint32 selectStart=decode->findDecodeRowFirstData(decode->m_rowList[key],i,left_,right_,startUnit/multiply);
                            selectStart-=10;
                            if(selectStart<0)
                                selectStart=0;

                            qint32 lastCount=0;

                            for(qint32 ii=selectStart;ii<dataCount_;ii++){
                                
                                decode->m_rowList[key]->mutex.lock();
                                DecodeRowData data=row->data[ii];
                                decode->m_rowList[key]->mutex.unlock();
                                
                                data.start*=multiply;
                                data.end*=multiply;
                                if(data.end>maxUnit){
                                    lastCount++;
                                    
                                    if(lastCount>10)
                                        break;
                                }
                                qint64 len=data.end-data.start;
                                if((len>0 && len>showConfig.m_pixelUnit)||len==0){
                                    qint64 drawStart=qMax(floor((data.start-startUnit)/showConfig.m_pixelUnit),-5.);
                                    qint64 drawEnd=floor(((data.end>maxUnit?maxUnit:data.end)-startUnit)/showConfig.m_pixelUnit);
                                    if((drawStart<0 && drawEnd<0) || (drawEnd==drawStart&&data.start!=data.end) || (drawStart>m_width && drawEnd>m_width))
                                        continue;
                                    isShow=true;
                                    DrawDecodeRow(&painter,drawStart,drawEnd,data,rowCount,fontWidth);
                                }
                            }
                            if(isShow){
                                painter.setPen(QPen(m_TextColor, 1));
                                painter.drawText(1, (m_decodeRowInterval+m_decodeRowHeight)*rowCount+16+m_decodeRowInterval,key+":"+row->desc);
                                rowCount++;
                            }
                        }
                    }
                }
            }else
                errorString=QObject::tr("放大后查看细节","解码放大");
        }
        if(!errorString.isEmpty()){
            painter.setPen(QPen(m_TextColor, 1));
            qint32 fontWidth=painter.fontMetrics().width(errorString);
            bool isDraw=false;
            errorString=errorString.replace(",","，");
            if(fontWidth>m_width&&errorString.contains("，")){
                QStringList ls=errorString.split("，");
                if(ls.length()==2){
                    qint32 fontWidth1=painter.fontMetrics().width(ls[1]);
                    fontWidth=painter.fontMetrics().width(ls[0]);
                    painter.drawText(m_width/2-fontWidth/2, 26, ls[0]);
                    painter.drawText(m_width/2-fontWidth1/2, 45, ls[1]);
                    if(fontWidth<fontWidth1)
                        fontWidth=fontWidth1;
                    isDraw=true;
                }
            }
            if(!isDraw)
                painter.drawText(m_width/2-fontWidth/2, 37, errorString);
            painter.drawLine(1,31,m_width/2-fontWidth/2-7,31);
            painter.drawLine(m_width/2+fontWidth/2+5,31,m_width-1,31);
        }
    }
    qint32 height=(m_decodeRowInterval+m_decodeRowHeight)*rowCount+5;
    if(height>61)
        m_decodeHeight=height;
    else
        m_decodeHeight=61;
}

void DoubleBuffering::DrawDecodeRow(QPainter *painter, qint32 start, qint32 end, DecodeRowData data, qint32 rowIndex, qint32 rowNameWidth)
{
    qint32 drawLen=end-start;
    if(drawLen>=0)
    {
        qint32 top=(m_decodeRowInterval+m_decodeRowHeight)*rowIndex+m_decodeRowInterval;
        qint32 offset=4;
        QBrush fillBrush=QBrush(m_decodeTypeColor[data.type%16]);
        painter->setPen(QPen(m_DataColor, 0));
        if(drawLen<10)
            offset=(drawLen-2)/2;
        if(offset<0)
            offset=0;
        if(data.start==data.end){
            start=start-((m_decodeRowHeight-1)/2);
            painter->setPen(QPen(Qt::transparent, 0));
            painter->setBrush(fillBrush);
            painter->drawEllipse(start,top,m_decodeRowHeight-1,m_decodeRowHeight-1);
            offset=1;
            end=start+m_decodeRowHeight-1;
        }else{
            qint32 median=top+m_decodeRowHeight/2;
            qint32 bottom=top+m_decodeRowHeight;
            QPainterPath path;
            path.moveTo(start+offset,top);
            path.lineTo(end-offset,top);
            path.lineTo(end,median);
            path.lineTo(end-offset,bottom);
            path.lineTo(start+offset,bottom);
            path.lineTo(start,median);
            painter->fillPath(path, fillBrush);
        }
        start+=offset;
        if(start<rowNameWidth)
            start=rowNameWidth;
        end-=offset;
        if(end>m_width)
            end=m_width;
        DrawDecodeText(painter,start,end,top+16,data);
    }
}

void DoubleBuffering::DrawDecodeText(QPainter *painter, qint32 start, qint32 end, qint32 y, DecodeRowData data)
{
    painter->setPen(QPen(Qt::black, 1));
    QFontMetrics fm = painter->fontMetrics();
    qint32 fontWidth=fm.width(data.longText);
    qint32 drawWidth=end-start;
    QString drawText=data.longText;
    if(drawWidth>0){
        if(drawWidth<fontWidth)
        {
            fontWidth=fm.width(data.text);
            drawText=data.text;
            if(drawWidth<fontWidth||fontWidth==0)
            {
                if(drawText.isEmpty())
                    drawText=data.longText;
                if(drawWidth>12&&drawText.size()>4)
                {
                    QString source=data.text;
                    if(source.isEmpty())
                        source=data.longText;
                    QString str=QString(source[0]);
                    qint32 index=1;
                    while(fm.width(str+"...")<drawWidth){
                        str+=source[index];
                        index++;
                    }
                    drawText=str.left(str.size()-2)+"...";
                    fontWidth=fm.width(drawText);
                }else
                    return;
            }
        }
        painter->drawText(start+drawWidth/2-fontWidth/2,y,drawText);
    }
}

void DoubleBuffering::DrawLogic(QPainter &painter)
{
    
    SessionConfig* config=m_session->m_config;
    SessionShowConfig showConfig=config->m_showConfig;
    qint32 multiply=m_session->m_segment->GetMultiply();
    qint64 maxUnit=config->m_maxUnit;
    qint64 startUnit=config->m_showStartUnit;
    {
        qint64 temp=showConfig.m_pixelUnit*(m_width+5)+startUnit+multiply;
        if(temp>maxUnit)
            temp=maxUnit;
        maxUnit=temp;
    }
    Segment* segment=m_session->m_segment;
    DrawLogicBasics(painter);
    if(segment->isData[m_channelID] && multiply>0)
    {
        double pixelUnit=showConfig.m_pixelUnit;
        qint32 offsetUnit=startUnit-(startUnit/multiply*multiply);
        double offset=offsetUnit/pixelUnit;
        qint64 llStartUnitMultiply=startUnit/multiply;
        qint64 ullChunkStart=llStartUnitMultiply,ullChunkEnd=maxUnit/multiply+multiply*5;
        qreal maxShow=(qreal)maxUnit/multiply;
        quint64 maxSample=segment->GetMaxSample(m_channelID);
        qreal currentWidth=0;
        bool sampleRecode;
        bool isEndLine=false;
        do{
            sampleRecode=segment->GetSample(ullChunkStart, m_channelID);
            DataEnd next=segment->GetDataEnd(ullChunkStart,m_channelID,false);
            qreal drawWidth=currentWidth;
            if(next.position!=(quint64)ullChunkStart && (quint64)ullChunkStart<maxSample && next.position>0)
            {
                if(next.position>=maxShow)
                {
                    next.position=maxShow;
                    isEndLine=true;
                }
                if(next.isEnd)
                    isEndLine=true;
                if((next.position-ullChunkStart)*multiply>pixelUnit)
                {
                    if(pixelUnit<1)
                        drawWidth=(next.position-llStartUnitMultiply)*100*multiply/(pixelUnit*100);
                    else
                        drawWidth=(next.position-llStartUnitMultiply)*multiply/pixelUnit;
                    if(drawWidth-offset>m_width)
                        drawWidth=m_width+offset+5;
                }
                DrawChannelLine(&painter, currentWidth-offset, drawWidth-offset, sampleRecode,isEndLine);
                if(drawWidth<currentWidth)
                    currentWidth++;
                else{
                    if(drawWidth==qFloor(drawWidth))
                        currentWidth=drawWidth+1;
                    else
                        currentWidth=qCeil(drawWidth);
                }
                ullChunkStart=(currentWidth*pixelUnit+startUnit-offsetUnit)/multiply;
            }else break;
        }while(ullChunkStart<ullChunkEnd && currentWidth-offset<m_width && !isEndLine && !m_isExit);
    }
    if(!m_decodeChannelDesc.isEmpty())
    {
        painter.setPen(QPen(m_TextColor, 1));
        QFont font = painter.font();
        font.setPixelSize(12);
        painter.setFont(font);
        painter.drawText(6,m_height/2+4,m_decodeChannelDesc);
    }
}

void DoubleBuffering::DrawLogicBasics(QPainter &painter)
{
    qint32 temp=m_height/2;
    painter.setPen(QPen(m_DashLineColor, 1, Qt::DashLine));
    painter.drawLine(QLineF(0, temp, m_width, temp));
    QFont font = painter.font();
    font.setPixelSize(9);
    painter.setFont(font);
    painter.setPen(QPen(m_TextColor, 1));
    painter.drawText(2,11,"H");
    painter.drawText(2,m_height-3,"L");
    painter.setPen(QPen(m_DataColor, 1));
    painter.fillPath(m_trianglePath, QBrush(m_DataColor));
}

QPixmap DoubleBuffering::getPixmap()
{
    if(m_pPixmap!=nullptr)
        return m_pPixmap->copy();
    else
        return QPixmap();
}

bool DoubleBuffering::isPixmap()
{
    return m_pPixmap!=nullptr;
}

