#include "search_table_model.h"

SearchTableModel::SearchTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_roleName[Qt::DisplayRole]="display";
    m_roleName[StartRole]="start";
    m_roleName[EndRole]="end";
    m_roleName[StartTextRole]="startText";
    m_roleName[TimeTextRole]="timeText";
}

SearchTableModel::~SearchTableModel()
{
    stopAll();
}

void SearchTableModel::init(QString sessionID)
{
    m_session=DataService::getInstance()->getSession(sessionID);
}

void SearchTableModel::stopAll()
{
    m_sortDataMutex.lock();
    if(m_state==1||m_state==2)
        m_state=3;
    m_sortDataMutex.unlock();
    while(true){
        if(m_sortDataMutex.try_lock()){
            qint8 temp=m_state;
            m_sortDataMutex.unlock();
            if(temp==0)
                break;
        }
        QApplication::processEvents(QEventLoop::AllEvents, 50);
    }
    resetSort();
}

void SearchTableModel::resetSort()
{
    m_sortDataMutex.lock();
    m_sortData.clear();
    m_data.clear();
    m_start=0;
    m_end=0;
    m_searchText="";
    m_sortDataMutex.unlock();
    setCount(0);
}

void SearchTableModel::refresh()
{
    if(m_count>0)
    {
        QModelIndex start_index = createIndex(0, 0);
        QModelIndex end_index = createIndex(m_count-1, m_column-1);
        emit dataChanged(start_index, end_index);
    }
}

void SearchTableModel::setLineHeight(qint32 lineHeight)
{
    m_set.lineHeight=lineHeight;
}

qint32 SearchTableModel::refreshBuffer(qint32 y)
{
    qint32 topLine=20;
    qint32 bottomLine=m_set.bufferSize-20;
    qint32 current=y/m_set.lineHeight;
    qint32 middle=m_set.bufferSize/2;
    qint32 offset=0;
    qint32 offsetCurrent=middle-current;
    qint32 recode=m_set.bufferFirst;
    if(current<=topLine){
        //缓冲区往顶部偏移
        if(m_set.bufferFirst!=0){
            m_set.bufferFirst-=offsetCurrent;
            if(m_set.bufferFirst<0){
                offset=recode*m_set.lineHeight;
                m_set.bufferFirst=0;
            }else
                offset=offsetCurrent*m_set.lineHeight;
            refreshBuffer();
        }
    }else if(current>=bottomLine){
        //缓冲区往底部偏移
        if(m_set.bufferFirst+m_set.bufferSize<m_sortCount){
            m_set.bufferFirst-=offsetCurrent;
            offset=offsetCurrent*m_set.lineHeight;
            refreshBuffer();
        }else
        {
            offset=(recode-(m_sortCount-m_set.bufferSize))*m_set.lineHeight;
            m_set.bufferFirst=m_sortCount-m_set.bufferSize;
            if(offset!=0)
                refreshBuffer();
        }
    }
    return offset;
}

qint32 SearchTableModel::getShowCount()
{
    return m_sortCount;
}

qint32 SearchTableModel::getShowFirst()
{
    return m_set.bufferFirst;
}

qint32 SearchTableModel::scrollPosition(double position)
{
    qint32 count=m_sortCount;
    qint32 middle=m_set.bufferSize/2;
    qint32 current=position*count;
    double offset=(position*count-current)*m_set.lineHeight;
    m_set.bufferFirst=current-middle;
    if(m_set.bufferFirst<0)
        m_set.bufferFirst=0;
    else if(m_set.bufferFirst+m_set.bufferSize>count)
        m_set.bufferFirst=count-m_set.bufferSize;
    if(m_set.bufferFirst<0)
        m_set.bufferFirst=0;
    refreshBuffer();
    return (current-m_set.bufferFirst)*m_set.lineHeight+offset;
}

void SearchTableModel::refreshCount()
{
    m_sortDataMutex.lock();
    m_sortCount=m_sortData.count();
    m_sortDataMutex.unlock();
    refreshBuffer();
}

bool SearchTableModel::search(const QString &text, qint64 start, qint64 end)
{
    qint32 state;
    m_sortDataMutex.lock();
    state=m_state;
    m_sortDataMutex.unlock();
    if(m_session && state==0)
        m_segment=m_session->m_segment;
    if(m_segment)
    {
        qint32 multiply=m_segment->GetMultiply();
        if(multiply<1 || text.size()!=16)
            return false;
        if(start==-2)
            start=0;
        if(end==-2)
            end=0;
        if(start==-1||start>m_session->m_config->m_maxUnit)
            start=m_session->m_config->m_maxUnit;
        if(end==-1||end>m_session->m_config->m_maxUnit)
            end=m_session->m_config->m_maxUnit;
        start=start/multiply+(start%multiply!=0?1:0);
        end/=multiply;
        if(start>end){
            start^=end;
            end^=start;
            start^=end;
        }
        if(text.count('X')==16){
            setCount(0);
            m_sortDataMutex.lock();
            m_searchText="";
            if(m_state!=0){
                m_state=3;
                m_sortDataMutex.unlock();
            }else{
                m_sortData.clear();
                m_sortCount=0;
                m_set.bufferFirst=0;
                m_sortDataMutex.unlock();
                refreshBuffer();
            }
            return true;
        }else if(m_state!=3 && m_segment){
            bool isChanged=false;
            m_sortDataMutex.lock();
            if(m_start!=start){
                m_start=start;
                isChanged=true;
            }
            if(m_end!=end){
                m_end=end;
                isChanged=true;
            }
            if(m_searchText!=text){
                m_searchText=text;
                isChanged=true;
            }
            if(isChanged){
                if(m_state==0)
                {
                    m_state=1;
                    QtConcurrent::run(this, &SearchTableModel::searchData);
                }
                else
                    m_state=2;
            }
            m_sortDataMutex.unlock();
            return isChanged;
        }
    }
    return false;
}

int SearchTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_count;
}

int SearchTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_column;
}

QHash<int, QByteArray> SearchTableModel::roleNames() const
{
    return m_roleName;
}

QVariant SearchTableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row >= m_count)
        return QVariant("");
    m_sortDataMutex.lock();
    SearchData rowData=m_data[row];
    m_sortDataMutex.unlock();
    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case 0:
            return m_set.bufferFirst+row+1;
        case 1:
            return nsToShowStr((double)rowData.start);
        case 2:
            return nsToShowStr((double)(rowData.end-rowData.start+1));
        }
        break;
    case StartRole:
        return rowData.start;
    case EndRole:
        return rowData.end;
    case StartTextRole:
        return nsToShowStr((double)rowData.start);
    case TimeTextRole:
        return nsToShowStr((double)(rowData.end-rowData.start+1));
    }
    return QVariant("");
}

void SearchTableModel::setCount(qint32 count)
{
    m_countMutex.lock();
    if(count!=m_count){
        qint32 temp=m_count;
        if(count>temp){
            if(temp<=0)
                temp=1;
            beginInsertRows(QModelIndex(),temp-1,count-1);
            m_count=count;
            endInsertRows();
        }else{
            beginRemoveRows(QModelIndex(),0,temp-count-1);
            m_count=count;
            endRemoveRows();
        }
    }
    m_countMutex.unlock();
}

void SearchTableModel::searchData()
{
    qint32 state=1;
    QString search;
    qint64 start,end;
    qint32 multiply=m_segment->GetMultiply();
    bool isAppend=false;
    qint64 t1=GetTickCount_();
    if(multiply<=0)
        return;
    quint64 maxSample=m_session->m_config->m_maxUnit/multiply;
    m_session->m_segment->appendCite();
    while(state==1){
        m_sortCount=0;
        refreshBuffer();
        m_sortDataMutex.lock();
        start=m_start;
        end=m_end;
        search=reverseString(m_searchText);
        m_sortData.clear();
        //初始化每次循环搜索的变量
        QVector<qint8> edgeList=getSearchEdge(search);//边沿搜索通道索引
        QVector<qint8> levelList=getSearchLevel(search);//电平搜索通道索引
        QVector<bool> channelLevel;//当前位置电平
        QVector<DataEnd> positionList;//通道边沿结果集
        bool isReset=true;
        //判断搜索通道是否拥有数据
        if(!isData(search))
        {
            m_sortDataMutex.unlock();
            state=3;
            break;
        }
        m_sortDataMutex.unlock();
        if(start==end){
            m_sortDataMutex.lock();
            m_state=0;
            m_sortDataMutex.unlock();
            break;
        }

        //是否边沿搜索
        qint32 completeCount=0;
        if(edgeList.isEmpty()){
            //电平搜索
            //初始化索引
            for(qint32 i=0;i<levelList.count();i++)
                channelLevel.append(m_segment->GetSample(start,levelList[i]));
            while(state==1 && start<=end){
                //搜索边沿结果
                if(isReset){
                    isReset=false;
                    positionList.clear();
                    completeCount=0;
                    for(qint32 i=0;i<levelList.count();i++)
                    {
                        DataEnd temp=m_segment->GetDataEnd(start,levelList[i],false);
                        if(temp.position<=0 || temp.position>=maxSample || temp.position>=end)
                        {
                            //到底了
                            completeCount++;
                            if(temp.position<=0)
                                temp.position=0;
                            else
                                temp.position=maxSample;
                            if(temp.position>=end)
                                temp.position=end;
                        }
                        positionList.append(temp);
                    }
                }
                //取得最小搜索结果
                DataEnd minPosition=positionList[0];
                QList<qint32> minPositionIndex;
                minPositionIndex.append(0);
                bool appendState=true;
                for(qint32 i=1;i<positionList.count();i++){
                    if(positionList[i].position<minPosition.position){
                        minPosition=positionList[i];
                        minPositionIndex.clear();
                        minPositionIndex.append(i);
                    }else if(positionList[i].position==minPosition.position)
                        minPositionIndex.append(i);
                }

                //判断电平是否符合搜索条件
                for(qint32 i=0;i<levelList.size();i++){
                    qint32 index=levelList[i];
                    switch(search[index].toUpper().toLatin1()){
                    case '0'://低电平
                        if(m_segment->GetSample(start,index))
                            appendState=false;
                        break;
                    case '1'://高电平
                        if(!m_segment->GetSample(start,index))
                            appendState=false;
                        break;
                    }
                }
                //判断是否全部搜索通道到底
                if(completeCount==levelList.count()&&m_state==1)
                    m_state=0;
                m_sortDataMutex.lock();
                if(appendState)
                    //符合条件，添加显示
                    m_sortData.append(SearchData(start*multiply, minPosition.position*multiply-1));
                {
                    completeCount=0;
                    for(qint32 i=0;i<minPositionIndex.count();i++){
                        DataEnd temp=m_segment->GetDataEnd(minPosition.position,levelList[minPositionIndex[i]],false);
                        if(temp.position<=0 || temp.position>=maxSample || temp.position>=end)
                        {
                            //到底了
                            completeCount++;
                            if(temp.position<=0)
                                temp.position=0;
                            else
                                temp.position=maxSample;
                            if(temp.position>=end)
                                temp.position=end;
                        }
                        positionList[minPositionIndex[i]]=temp;
                    }
                }
                start=minPosition.position;
                state=m_state;
                m_sortDataMutex.unlock();
                if(appendState)
                    isAppend=true;
                if(isAppend){
                    qint64 t2=GetTickCount_();
                    if(t2-t1>500){
                        t1=t2;
                        refreshCount();
                    }
                }
            }
        }else{
            //边沿搜索
            while(state==1 && start<=end){
                //搜索边沿结果
                if(isReset){
                    isReset=false;
                    positionList.clear();
                    completeCount=0;
                    for(qint32 i=0;i<edgeList.count();i++)
                    {
                        DataEnd temp=m_segment->GetDataEnd(start,edgeList[i],false);
                        if(temp.position<=0 || temp.position>=maxSample || temp.position>=end)
                        {
                            //到底了
                            completeCount++;
                            if(temp.position<=0)
                                temp.position=0;
                            else
                                temp.position=maxSample;
                            if(temp.position>=end)
                                temp.position=end;
                        }
                        positionList.append(temp);
                    }
                }
                //判断所有边沿位置是否一样，否则位置跳到最大搜索结果处继续搜索
                DataEnd maxPosition=positionList[0];
                QList<qint32> maxPositionIndex;
                maxPositionIndex.append(0);
                bool appendState=true;
                //判断所有边沿位置
                for(qint32 i=1;i<positionList.count();i++){
                    if(maxPosition.position!=positionList[i].position)
                        appendState=false;
                    if(positionList[i].position>maxPosition.position){
                        maxPosition=positionList[i];
                        maxPositionIndex.clear();
                        maxPositionIndex.append(i);
                    }else if(positionList[i].position==maxPosition.position)
                        maxPositionIndex.append(i);
                }
                if(appendState){
                    //判断边沿是否符合搜索条件
                    for(qint32 i=0;i<edgeList.count();i++){
                        qint32 index=edgeList[i];
                        bool first=m_segment->GetSample(positionList[i].position-1,index);
                        bool second=m_segment->GetSample(positionList[i].position,index);
                        switch(search[index].toUpper().toLatin1()){
                        case 'R'://上升沿
                            if(!(!first && second))
                                appendState=false;
                            break;
                        case 'F'://下降沿
                            if(!(first && !second))
                                appendState=false;
                            break;
                        case 'C'://所有沿
                            if(first == second)
                                appendState=false;
                            break;
                        }
                        if(!appendState)
                            break;
                    }
                }
                if(appendState){
                    //判断电平是否符合搜索条件
                    for(qint32 i=0;i<levelList.size();i++){
                        qint32 index=levelList[i];
                        switch(search[index].toUpper().toLatin1()){
                        case '0'://低电平
                            if(m_segment->GetSample(maxPosition.position,index))
                                appendState=false;
                            break;
                        case '1'://高电平
                            if(!m_segment->GetSample(maxPosition.position,index))
                                appendState=false;
                            break;
                        }
                    }
                }
                start=maxPosition.position;
                //判断是否全部搜索通道到底
                if(completeCount==edgeList.count()&&m_state==1)
                    m_state=0;
                m_sortDataMutex.lock();
                if(appendState){
                    //符合条件，添加显示
                    qint64 temp=start*multiply;
                    m_sortData.append(SearchData(temp, temp));
                }
                {
                    completeCount=0;
                    for(qint32 i=0;i<maxPositionIndex.count();i++){
                        DataEnd temp=m_segment->GetDataEnd(start,edgeList[maxPositionIndex[i]],false);
                        if(temp.position<=0 || temp.position>=maxSample || temp.position>=end)
                        {
                            //到底了
                            completeCount++;
                            if(temp.position<=0)
                                temp.position=0;
                            else
                                temp.position=maxSample;
                            if(temp.position>=end)
                                temp.position=end;
                        }
                        positionList[maxPositionIndex[i]]=temp;
                    }
                }
                state=m_state;
                m_sortDataMutex.unlock();
                if(appendState)
                    emit refreshCountTimer(true);
            }
        }
        //更新搜索参数重置搜索
        if(state==2){
            m_sortDataMutex.lock();
            m_state=1;
            state=1;
            isReset=true;
            m_sortDataMutex.unlock();
        }
    }
    m_session->m_segment->lessenCite();
    emit refreshCountTimer(false);
    //停止搜索
    m_sortDataMutex.lock();
    if(state==3)
        m_sortData.clear();
    m_sortCount=m_sortData.count();
    if(m_set.bufferFirst+m_set.bufferSize>m_sortCount)
        m_set.bufferFirst=m_sortCount-m_set.bufferSize;
    if(m_set.bufferFirst<0)
        m_set.bufferFirst=0;
    m_state=0;
    m_sortDataMutex.unlock();
    refreshBuffer();
}

void SearchTableModel::refreshBuffer()
{
    m_sortDataMutex.lock();
    qint32 showCount=qMin(m_sortCount,m_set.bufferFirst+m_set.bufferSize);
    QVector<SearchData>* data=&m_sortData;
    m_data=data->mid(m_set.bufferFirst,showCount-m_set.bufferFirst);
    showCount=m_data.count();
    m_sortDataMutex.unlock();
    setCount(showCount);
    if(m_count>0)
    {
        QModelIndex start_index = createIndex(0, 0);
        QModelIndex end_index = createIndex(m_count-1, m_column-1);
        emit dataChanged(start_index, end_index);
    }
    emit modelDataChanged();
}

QVector<qint8> SearchTableModel::getSearchEdge(const QString &searchText)
{
    QVector<qint8> list;
    for(qint32 i=0;i<searchText.size();i++)
        if(searchText[i]=='R' || searchText[i]=='F' || searchText[i]=='C')
            list.append(i);
    return list;
}

QVector<qint8> SearchTableModel::getSearchLevel(const QString &searchText)
{
    QVector<qint8> list;
    for(qint32 i=0;i<searchText.size();i++)
        if(searchText[i]=='0' || searchText[i]=='1')
            list.append(i);
    return list;
}

bool SearchTableModel::isData(const QString &str)
{
    for(qint32 i=0;i<str.size();i++){
        if(str[i]=='0' || str[i]=='1' || str[i]=='R' || str[i]=='F' || str[i]=='C'){
            if(!m_segment->isData[i])
                return false;
        }
    }
    return true;
}
