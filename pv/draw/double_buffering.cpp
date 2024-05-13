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
    m_blockDash.clear();
    m_blockDash<<1<<1;
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
}

void DoubleBuffering::refReshBuffer(QPainter *painter)
{
    if(m_width<1||m_height<1||m_isExit)
        return;

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
    //区分是否解码器
    if(m_isDecode)
        DrawDecode(*painter);
    else
        DrawLogic(*painter);
    m_session->m_segment->lessenCite();
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
    //获取显示参数
    SessionConfig* config=m_session->m_config;
    SessionShowConfig showConfig=config->m_showConfig;
    qint32 multiply=m_session->m_segment->GetMultiply();
    qint64 maxUnit=config->m_maxUnit;
    qint64 startUnit=config->m_showStartUnit;
    double threshold=showConfig.m_pixelUnit*4;
    {
        qint64 temp=showConfig.m_pixelUnit*(m_width+5)+startUnit+3;
        if(temp>maxUnit)
            temp=maxUnit;
        maxUnit=temp;
    }
    if(multiply<=0)
        multiply=1;
    //初始化
    qint32 rowCount=0;
    QVector<qint32> showList;
    QString errorString="";
    if(m_session->m_isShowDecode){
        DecodeController* decode=m_session->getDecode(m_decodeID);
        if(!decode || decode->m_isDelete)
            return;
        QFont font = painter.font();
        font.setFamily("OPPOSans");
        if(decode->m_height==1){
            m_topOffset=4;
            font.setPixelSize(12);
        }else{
            m_topOffset=6;
            font.setPixelSize(16);
        }
        painter.setFont(font);
        painter.setPen(QPen(m_TextColor, 0));
        if(!decode)
            return;
        if(decode->m_isReportedError)
            errorString=QObject::tr("数据解码出错，请检查协议参数");
        if(errorString.isEmpty()){
            m_decodeRowHeight=m_decodeRowInitHeight*decode->m_height;
            for(QString &key : decode->m_rowList.keys()){
                decode->m_rowListMutex.lock();
                if(decode->m_isDelete){
                    decode->m_rowListMutex.unlock();
                    return;
                }
                decode->m_rowList[key]->mutex.lock();
                qint32 count_=decode->m_rowList[key]->row.count();
                decode->m_rowList[key]->mutex.unlock();
                for(qint32 i=0;i<count_;i++){
                    decode->m_rowList[key]->mutex.lock();
                    DecodeRow* row=&decode->m_rowList[key]->row[i];
                    qint32 dataCount_=row->data.length();
                    decode->m_rowList[key]->mutex.unlock();
                    //计算主要颜色
                    if(row->colorState==0 && dataCount_>0){
                        decode->m_rowList[key]->mutex.lock();
                        DecodeRowData data=row->data[0];
                        decode->m_rowList[key]->mutex.unlock();
                        row->color=m_decodeTypeColor[data.type%16];
                        row->colorState=1;
                    }else if(row->colorState==1 && dataCount_>10){
                        QHash<qint32, qint32> count;
                        for(qint32 i=0;i<10;i++){
                            decode->m_rowList[key]->mutex.lock();
                            DecodeRowData data=row->data[i];
                            decode->m_rowList[key]->mutex.unlock();
                            if(count.contains(data.type))
                                count[data.type]++;
                            else
                                count[data.type]=1;
                        }
                        qint32 max=-1;
                        QHash<qint32,qint32>::const_iterator it = count.constBegin();
                        while (it != count.constEnd()) {
                            if(max==-1)
                                max=it.key();
                            else if(it.value()>count[max])
                                max=it.key();
                            ++it;
                        }
                        row->color=m_decodeTypeColor[max%16];
                        row->colorState=2;
                    }
                    if(row->isShow && dataCount_>0)
                    {
                        if(m_isExit)
                            return;
                        QPen blockFillPen(Qt::transparent, 1, Qt::DotLine);
                        blockFillPen.setDashPattern(m_blockDash);
                        blockFillPen.setColor(row->color);
                        bool isShow=false;
                        //二分法查找数据区间
                        qint32 left_=0,right_=0;
                        decode->findDecodeRowDataNode(decode->m_rowList[key],i,left_,right_,startUnit/multiply);
                        //二分法查找首个数据
                        qint32 selectStart=decode->findDecodeRowFirstData(decode->m_rowList[key],i,left_,right_,startUnit/multiply);
                        selectStart-=10;
                        if(selectStart<0)
                            selectStart=0;
                        qint32 blockStartWidth=-1, blockLen=-1;
                        qint32 lastCount=0;
                        QList<qint32> drawWidthList;
                        auto showRowName = [&](){
                            if(!showList.contains(rowCount)){
                                showList.append(rowCount);
                                QFontMetrics fm = painter.fontMetrics();
                                qint32 startWidth=m_width;
                                if(drawWidthList.count()>0){
                                    qSort(drawWidthList);
                                    startWidth=drawWidthList[0];
                                }
                                if(startWidth>fm.horizontalAdvance(key+":"+row->desc))
                                    DrawDecodeRowName(&painter,rowCount,key+":"+row->desc);
                            }
                        };

                        for(qint32 ii=selectStart;ii<dataCount_;ii++){
                            //获取数据
                            decode->m_rowList[key]->mutex.lock();
                            DecodeRowData data=row->data[ii];
                            decode->m_rowList[key]->mutex.unlock();

                            //单位转换
                            data.start*=multiply;
                            data.end*=multiply;
                            if(data.end>maxUnit){
                                lastCount++;
                                //绘制结束位置后10个数据
                                if(lastCount>10)
                                    break;
                            }
                            if(blockStartWidth!=-1){
                                qint32 tmp=(qint32)floor((data.end-startUnit)/showConfig.m_pixelUnit);
                                if(tmp!=blockStartWidth+blockLen && tmp!=blockStartWidth+blockLen+1){
                                    if(blockStartWidth+blockLen<0)
                                        blockStartWidth=-1;
                                    else{
                                        drawWidthList.append(blockStartWidth);
                                        isShow=true;
                                        DrawDecodeFill(&painter,blockStartWidth,blockStartWidth+blockLen,rowCount,blockFillPen);
                                        blockStartWidth=-1;
                                    }
                                }
                            }
                            qint64 len=data.end-data.start;
                            if(len<threshold&&len>0){
                                qint32 len=qCeil((data.end-data.start)/showConfig.m_pixelUnit);
                                if(blockStartWidth<0){
                                    blockStartWidth=floor((data.start-startUnit)/showConfig.m_pixelUnit);
                                    blockLen=len;
                                    if(blockStartWidth>m_width)
                                        blockStartWidth=-1;
                                }else
                                    blockLen+=len;
                                if(blockStartWidth+blockLen>m_width+5){
                                    drawWidthList.append(blockStartWidth);
                                    isShow=true;
                                    DrawDecodeFill(&painter,blockStartWidth,blockStartWidth+blockLen,rowCount,blockFillPen);
                                    blockStartWidth=-1;
                                    break;
                                }
                                qint64 next=((blockStartWidth+blockLen+1)*showConfig.m_pixelUnit+startUnit)/multiply;
                                qint32 tmp=decode->findDecodeRowFirstData(decode->m_rowList[key],i,left_,right_,next);
                                if(tmp<ii){
                                    decode->findDecodeRowDataNode(decode->m_rowList[key],i,left_,right_,next);
                                    tmp=decode->findDecodeRowFirstData(decode->m_rowList[key],i,left_,right_,next);
                                }
                                if(tmp>ii)
                                    ii=tmp-1;
                            }else{
                                if(blockStartWidth!=-1){
                                    if(len==0)
                                        continue;
                                    if(blockStartWidth+blockLen<0)
                                        blockStartWidth=-1;
                                    else{
                                        drawWidthList.append(blockStartWidth);
                                        isShow=true;
                                        DrawDecodeFill(&painter,blockStartWidth,blockStartWidth+blockLen,rowCount,blockFillPen);
                                        blockStartWidth=-1;
                                    }
                                }
                                qint64 drawStart=qMax(floor((data.start-startUnit)/showConfig.m_pixelUnit),-5.);
                                qint64 drawEnd=floor(((data.end>maxUnit?maxUnit:data.end)-startUnit)/showConfig.m_pixelUnit);
                                if((drawStart<0 && drawEnd<0) || (drawStart>m_width && drawEnd>m_width))
                                    continue;
                                drawWidthList.append(drawStart);
                                isShow=true;
                                qint32 ellipseWidth=m_decodeRowHeight;
                                if(data.start==data.end){
                                    if(ii+1<dataCount_){
                                        decode->m_rowList[key]->mutex.lock();
                                        DecodeRowData data2=row->data[ii+1];
                                        decode->m_rowList[key]->mutex.unlock();
                                        data2.start*=multiply;
                                        if(data2.start>data.end){
                                            qint64 drawStart_=qMax(floor((data2.start-startUnit)/showConfig.m_pixelUnit),-5.);
                                            if(drawStart_-drawStart<ellipseWidth/2)
                                                ellipseWidth=qMax((drawStart_-drawStart)*2,0ll);
                                        }
                                    }
                                    if(ii-1>=0){
                                        decode->m_rowList[key]->mutex.lock();
                                        DecodeRowData data2=row->data[ii-1];
                                        decode->m_rowList[key]->mutex.unlock();
                                        data2.end*=multiply;
                                        if(data2.end<data.start){
                                            qint64 drawEnd_=qMax(floor((data2.end-startUnit)/showConfig.m_pixelUnit),-5.);
                                            if(drawEnd-drawEnd_<ellipseWidth/2)
                                                ellipseWidth=qMax((drawEnd-drawEnd_)*2,0ll);
                                        }
                                    }
                                }
                                DrawDecodeRow(&painter,drawStart,drawEnd,data,rowCount,ellipseWidth,blockFillPen);
                            }
                        }
                        if(blockStartWidth!=-1){
                            if(blockStartWidth+blockLen<0)
                                blockStartWidth=-1;
                            else{
                                drawWidthList.append(blockStartWidth);
                                isShow=true;
                                DrawDecodeFill(&painter,blockStartWidth,blockStartWidth+blockLen,rowCount,blockFillPen);
                            }
                        }
                        if(isShow||decode->m_isLockRow){
                            showRowName();
                            rowCount++;
                        }
                    }
                }
                decode->m_rowListMutex.unlock();
            }
        }
        if(!errorString.isEmpty()){
            painter.setPen(QPen(m_TextColor, 1));
            qint32 fontWidth=painter.fontMetrics().horizontalAdvance(errorString);
            bool isDraw=false;
            errorString=errorString.replace(",","，");
            if(fontWidth>m_width&&errorString.contains("，")){
                QStringList ls=errorString.split("，");
                if(ls.length()==2){
                    qint32 fontWidth1=painter.fontMetrics().horizontalAdvance(ls[1]);
                    fontWidth=painter.fontMetrics().horizontalAdvance(ls[0]);
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

void DoubleBuffering::DrawDecodeFill(QPainter *painter, qint32 startWidth, qint32 endWidth, qint32 rowIndex, QPen& blockFillPen)
{
    qint32 top=(m_decodeRowInterval+m_decodeRowHeight)*rowIndex+m_decodeRowInterval;
    QPen p=painter->pen();
    painter->setPen(QPen(blockFillPen.color(), 0, Qt::SolidLine));
    painter->drawRect(QRect(startWidth,top,endWidth-startWidth,m_decodeRowHeight));
    painter->setPen(blockFillPen);
    for(qint32 i=1;i<m_decodeRowHeight;i++)
        painter->drawLine(QLineF(startWidth+(i%2==0?1:0), top+i, endWidth, top+i));
    painter->setPen(p);
}

void DoubleBuffering::DrawDecodeRow(QPainter *painter, qint32 start, qint32 end, DecodeRowData data, qint32 rowIndex, qint32 ellipseWidth, QPen& blockFillPen)
{
    qint32 rStart=start,rEnd=end;
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
            if(ellipseWidth<4)
                return;
            start=start-ellipseWidth/2;
            painter->setPen(QPen(Qt::transparent, 0));
            painter->setBrush(fillBrush);
            painter->drawEllipse(start,top,ellipseWidth,m_decodeRowHeight-1);
            offset=0;
            end=start+ellipseWidth;
            if(ellipseWidth<8)
                return;
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
        end-=offset;
        if(end>m_width)
            end=m_width;
        if(!DrawDecodeText(painter,start,end,top+m_topOffset,data)){
            painter->setBrush(QBrush(Qt::transparent));
            painter->setCompositionMode(QPainter::CompositionMode_Clear);
            painter->fillRect(QRect(rStart,top,rEnd-rStart,m_decodeRowHeight),QBrush(Qt::transparent));
            painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
            DrawDecodeFill(painter,rStart,rEnd,rowIndex,blockFillPen);
        }
    }
}

void DoubleBuffering::DrawDecodeRowName(QPainter *painter, qint32 rowCount, QString text)
{
    painter->setPen(QPen(m_TextColor, 1));
    painter->drawText(1, (m_decodeRowInterval+m_decodeRowHeight)*rowCount+m_topOffset+m_decodeRowInterval+m_decodeRowHeight/2,text);
}

bool DoubleBuffering::DrawDecodeText(QPainter *painter, qint32 start, qint32 end, qint32 y, DecodeRowData data)
{
    painter->setPen(QColor::fromRgb(0,0,0));
    QFontMetrics fm = painter->fontMetrics();
    qint32 fontWidth=fm.horizontalAdvance(data.longText);
    qint32 drawWidth=end-start;
    QString drawText=data.longText;
    y+=m_decodeRowHeight/2;
    if(drawWidth>0){
        bool isDrawRect=false;
        QString color;
        if(drawText.length()>3 && drawText.indexOf("#")!=-1){
            QRegExp regExp("#?([a-fA-F0-9]{6}|[a-fA-F0-9]{3})");
            if(regExp.indexIn(drawText, 0)!=-1){
                color="#"+regExp.cap(1);
                start+=14;
                drawWidth=end-start;
                if(drawWidth<0)
                    return false;
                isDrawRect=true;
            }
        }
        if(drawWidth<fontWidth)
        {
            if(data.start==data.end){
                if(!data.text.isEmpty()){
                    drawText=data.text;
                    fontWidth=fm.horizontalAdvance(drawText);
                }
                if(drawWidth<fontWidth){
                    drawText="..";
                    fontWidth=fm.horizontalAdvance(drawText);
                    y-=2;
                }
            }else{
                drawText=data.text;
                fontWidth=fm.horizontalAdvance(drawText);
                if(drawWidth<fontWidth||fontWidth==0)
                {
                    if(drawText.isEmpty())
                        drawText=data.longText;
                    if(drawWidth>12&&drawText.size()>1)
                    {
                        QString source=data.text;
                        if(source.isEmpty())
                            source=data.longText;
                        QString str="";
                        qint32 index=0;
                        while(true){
                            if(fm.horizontalAdvance(str+source[index]+"...")>drawWidth||index>=source.size())
                                break;
                            str+=source[index];
                            index++;
                        }
                        drawText=str+"...";
                        fontWidth=fm.horizontalAdvance(drawText);
                    }else{
                        return false;
                    }
                }
            }
        }
        painter->drawText(start+drawWidth/2-fontWidth/2,y,drawText);
        if(isDrawRect)
            painter->fillRect(QRect(start+drawWidth/2-fontWidth/2-12, y-m_topOffset-5, 10, 10), QBrush(QColor(color)));
    }
    return true;
}

void DoubleBuffering::DrawLogic(QPainter &painter)
{
    //获取显示参数
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

    //绘制基本要素
    DrawLogicBasics(painter);

    //没有数据则跳过绘制
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
                DrawChannelLine(&painter, currentWidth-offset, drawWidth-offset, sampleRecode, isEndLine);
                if(currentWidth==drawWidth)
                    DrawChannelLine(&painter, currentWidth, currentWidth, false, false);
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

    //绘制通道注释名
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
    //    绘制对齐标尺的竖线
    //    painter->setPen(QPen(m_LineColor, 1));
    //    double drawPosition=0;
    //    qint32 drawEnd=width()+10;
    //    double office=qRound(showConfig.m_pixelUnit*showConfig.m_scalePixel);
    //    if(office<1)
    //        office=1;
    //    double unitOffice=config->m_showStartUnit/office;
    //    unitOffice=(unitOffice-ceil(unitOffice))*office/showConfig.m_pixelUnit;
    //    drawPosition=-unitOffice;
    //    while(drawPosition<drawEnd){
    //        painter->drawLine(QLineF(drawPosition, 0, drawPosition, m_height));
    //        drawPosition+=showConfig.m_scalePixel;
    //    }

    //绘制中线
    qint32 temp=m_height/2;
    painter.setPen(QPen(m_DashLineColor, 1, Qt::DashLine));
    painter.drawLine(QLineF(0, temp, m_width, temp));
    //绘制HL文本
    QFont font = painter.font();
    font.setPixelSize(9);
    painter.setFont(font);
    painter.setPen(QPen(m_TextColor, 1));
    painter.drawText(2,11,"H");
    painter.drawText(2,m_height-3,"L");

    //绘制三角标
    painter.setPen(QPen(m_DataColor, 1));
    painter.fillPath(m_trianglePath, QBrush(m_DataColor));
}

