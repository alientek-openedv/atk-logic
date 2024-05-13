#include "decode_table_model.h"

DecodeTableModel::DecodeTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_roleName[Qt::DisplayRole]="display";
    m_roleName[TextRole]="showText";
    m_roleName[StartTextRole]="startText";
    m_roleName[StartRole]="start";
    m_roleName[EndRole]="end";
    m_roleName[TimeTextRole]="timeText";
}

DecodeTableModel::~DecodeTableModel()
{
    stopAll(false);
}

void DecodeTableModel::init(QString sessionID)
{
    m_session=DataService::getInstance()->getSession(sessionID);
    if(m_session)
        m_decodeList=&m_session->m_decodeList;
    m_decode=nullptr;
    m_orderList=nullptr;
    resetSort();
}

QJsonArray DecodeTableModel::initShow(QString decodeID)
{
    stopAll(true);
    m_decode=nullptr;
    m_showRowList.clear();
    m_rowType.clear();
    m_sortPositionList.clear();
    m_set.bufferFirst=0;
    QJsonArray array;
    for(auto &i : (*m_decodeList)){
        if(!i)
            continue;
        if(i->getDecodeID()==decodeID)
        {
            QStringList ls=i->m_rowList.keys();
            for(auto &j : ls){
                i->m_rowList[j]->mutex.lock();
                for(qint32 k=0;k<i->m_rowList[j]->row.count();k++)
                    array.append(i->m_rowList[j]->row[k].id);
                i->m_rowList[j]->mutex.unlock();
            }
            m_decode=i;
            m_orderList=&i->m_orderList;
            m_isComplete=!m_decode->isRun();
            for(auto i:m_decode->m_decodeIndex)
                m_rowType.append(QMap<QString,QJsonObject>());
            break;
        }
    }
    return array;
}

void DecodeTableModel::appendShow(QString rowID)
{
    setStop(false);
    if(!m_showRowList.contains(rowID))
    {
        m_showRowList.append(rowID);
        QStringList ls=m_decode->m_rowList.keys();
        for(const QString &i : ls){
            m_decode->m_rowList[i]->mutex.lock();
            for(qint32 ii=0;ii<m_decode->m_rowList[i]->row.count();ii++){
                if(m_decode->m_rowList[i]->row[ii].id==rowID)
                {
                    qint32 index=m_decode->m_decodeIndex.indexOf(i);
                    if(index>=0&&index<m_rowType.count())
                        m_rowType[index].insert(m_decode->m_rowList[i]->row[ii].rowType);
                    m_decode->m_rowList[i]->mutex.unlock();
                    bool isAdd=true;
                    if(!m_sortPositionList.isEmpty()){
                        for(auto &k:m_sortPositionList){
                            if(k.name==i && k.showIndex==ii)
                            {
                                isAdd=false;
                                break;
                            }
                        }
                    }
                    if(isAdd)
                        m_sortPositionList.append(SortShow(i,ii));
                    resetSort();
                    return;//这里会强制返回，下面不要放代码
                }
            }
            m_decode->m_rowList[i]->mutex.unlock();
        }
    }
}

void DecodeTableModel::removeShow(QString rowID)
{
    if(m_showRowList.indexOf(rowID)!=-1)
    {
        m_showRowList.removeAt(m_showRowList.indexOf(rowID));
        QStringList ls=m_decode->m_rowList.keys();
        for(const QString &i : ls){
            m_decode->m_rowList[i]->mutex.lock();
            for(qint32 ii=0;ii<m_decode->m_rowList[i]->row.count();ii++){
                if(m_decode->m_rowList[i]->row[ii].id==rowID)
                {
                    qint32 index=m_decode->m_decodeIndex.indexOf(i);
                    if(index>=0&&index<m_rowType.count())
                        m_rowType[index].remove(rowID);
                    m_decode->m_rowList[i]->mutex.unlock();
                    for(qint32 j=0;j<m_sortPositionList.count();j++){
                        if(m_sortPositionList[j].name==i && m_sortPositionList[j].showIndex==ii){
                            m_sortPositionList.removeAt(j);
                            break;
                        }
                    }
                    resetSort();
                    return;//这里会强制返回，下面不要放代码
                }
            }
            m_decode->m_rowList[i]->mutex.unlock();
        }
    }
}

void DecodeTableModel::search(const QString &text)
{
    m_sortDataMutex.lock();
    m_isSearch=true;
    m_searchText=text.toLower();
    m_sortDataMutex.unlock();
}

void DecodeTableModel::exitSearch()
{
    m_sortDataMutex.lock();
    m_isSearch=false;
    m_searchData.clear();
    m_sortDataMutex.unlock();
}

void DecodeTableModel::stopAll(bool isRemove)
{
    m_isStop=true;
    m_runStateMutex.lock();
    m_isNext=false;
    if(m_isRun==1)
        m_isRun=2;
    m_runStateMutex.unlock();
    while(true){
        m_runStateMutex.lock();
        qint8 temp=m_isRun;
        m_runStateMutex.unlock();
        if(temp==0)
            break;
        QApplication::processEvents(QEventLoop::AllEvents, 50);
        QThread::msleep(50);
    }
    if(isRemove)
        m_orderList=nullptr;
    resetSort();
}

double DecodeTableModel::showData()
{
    qint64 start=0;
    if(m_session->m_segment!=nullptr && m_session->m_segment->GetMultiply()>0)
        start=m_session->m_config->m_showStartUnit/m_session->m_segment->GetMultiply();
    qint64 index=findShowDataFirstData(start);
    return findShowDataFirstData(start)/(double)m_sortCount;
}

qint64 DecodeTableModel::findShowDataFirstData(qint64 start)
{
    //二分法查找首个数据
    m_sortDataMutex.lock();
    QList<DecodeRowData>* data=m_isSearch?&m_searchData:&m_sortData;
    qint32 selectStart=-1;
    qint32 left=0,right=data->count();
    while(left < right)
    {
        int mid=(left+right)/2;
        if(data->at(mid).start<=start && data->at(mid).end>=start){
            selectStart=mid;
            break;
        }
        else if(data->at(mid).start > start){
            right = mid; // 中间的值大于目标值，向左收缩区间
        }
        else if(data->at(mid).end < start){
            left = mid+1;// 中间的值小于目标值，向右收缩区间
        }
        if(right==left)
            selectStart=left-(left==0?0:1);
    }
    m_sortDataMutex.unlock();
    return selectStart;
}

int cmpData(DecodeRowData&e1,DecodeRowData&e2){
    if(e1.start<e2.start)
        return 1;
    else
        return 0;
}

void DecodeTableModel::refresh()
{
    if((m_decode==nullptr && m_showRowList.count()<1) || m_isStop)
        return;
    m_runStateMutex.lock();
    if(m_isRun!=2){
        if(m_isRun==1){
            m_isNext=true;
            m_runStateMutex.unlock();
        }
        else{
            m_isRun=1;
            m_runStateMutex.unlock();
            QtConcurrent::run(this, &DecodeTableModel::sortData);
        }
    }else
        m_runStateMutex.unlock();
    //qSort(m_data.begin(), m_data.end(), cmpData);
}

void DecodeTableModel::setLineHeight(qint32 lineHeight)
{
    m_set.lineHeight=lineHeight;
}

QVariantMap DecodeTableModel::get(int rowIndex) {
    QVariantMap res;
    for(qint32 i=0;i<m_column;i++){
        QModelIndex idx = index(rowIndex, i);
        res[QString::number(i)] = idx.data(Qt::DisplayRole);
    }
    res["size"]=m_column;
    return res;
}

qint32 DecodeTableModel::refreshBuffer(qint32 y)
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

qint32 DecodeTableModel::getShowCount()
{
    return m_sortCount;
}

void DecodeTableModel::setComplete(bool isComplete)
{
    m_isComplete=isComplete;
    emit isExportChanged();
}

qint32 DecodeTableModel::getShowFirst()
{
    return m_set.bufferFirst;
}

qint64 DecodeTableModel::getFirstStart(double position)
{
    if(m_count>0){
        qint32 current=position*m_sortCount;
        return m_data[current-m_set.bufferFirst].start*m_decode->m_multiply;
    }
    return -1;
}

qint32 DecodeTableModel::scrollPosition(double position, double height)
{
    qint32 middle=m_set.bufferSize/2;
    qint32 current=position*m_sortCount;
    m_set.bufferFirst=current-middle;
    if(m_set.bufferFirst<0)
        m_set.bufferFirst=0;
    else if(m_set.bufferFirst+m_set.bufferSize>m_sortCount)
        m_set.bufferFirst=m_sortCount-m_set.bufferSize;
    if(m_set.bufferFirst<0)
        m_set.bufferFirst=0;
    refreshBuffer();
    current-=m_set.bufferFirst;
    if(current+height/m_set.lineHeight>m_sortCount-m_set.bufferFirst)
        current=m_sortCount-m_set.bufferFirst-height/m_set.lineHeight;
    if(current<0)
        current=0;
    return current*m_set.lineHeight;
}

void DecodeTableModel::refreshBuffer()
{
    m_sortDataMutex.lock();
    qint32 showCount=qMin(m_sortCount,m_set.bufferFirst+m_set.bufferSize);
    QList<DecodeRowData>* data=m_isSearch?&m_searchData:&m_sortData;
    m_data=data->mid(m_set.bufferFirst,showCount-m_set.bufferFirst);
    showCount=m_data.count();
    setCount(showCount);
    m_sortDataMutex.unlock();
    if(showCount>0)
    {
        QModelIndex start_index = createIndex(0, 0);
        QModelIndex end_index = createIndex(showCount-1, m_column-1);
        emit dataChanged(start_index, end_index);
    }
    emit modelDataChanged();
}

void DecodeTableModel::sortData()
{
    if(m_isStop)
        return;
    m_runStateMutex.lock();
    qint32 runState=m_isRun;
    m_runStateMutex.unlock();
    qint32 count;
    if(m_decode==nullptr)
        goto end;
    m_decode->m_orderListMutex.lock();
    if(m_orderList==nullptr||runState!=1){
        m_decode->m_orderListMutex.unlock();
        goto end;
    }
    count=m_orderList->count()-(m_isComplete?0:1);
    m_decode->m_orderListMutex.unlock();
    {
        qint32 firstData=0;
        if(m_sortPositionList.count()!=0)
        {
            firstData=m_sortPositionList[0].dataPosition;
            for(const auto &i:m_sortPositionList){
                if(i.dataPosition>firstData)
                    firstData=i.dataPosition;
            }
        }
        for(qint32 i=firstData;i<count;i++){
            if(m_orderList==nullptr||m_decode==nullptr)
                goto end;
            m_decode->m_orderListMutex.lock();
            DataOrder data=m_orderList->at(i);
            m_decode->m_orderListMutex.unlock();
            qint32 count=m_sortPositionList.count();
            for(qint32 ii=0;ii<count;ii++){
                m_runStateMutex.lock();
                runState=m_isRun;
                m_runStateMutex.unlock();
                if(runState==2)
                    goto end;
                if(data.name==m_sortPositionList[ii].name){
                    if(m_sortPositionList[ii].showIndex==data.rowIndex){
                        m_decode->m_rowList[data.name]->mutex.lock();
                        m_sortDataMutex.lock();
                        for(qint32 j=0;j<data.len;j++){
                            m_sortData+=m_decode->m_rowList[data.name]->row[data.rowIndex].data[m_sortPositionList[ii].rowPosition];
                            m_sortPositionList[ii].rowPosition++;
                        }
                        m_sortDataMutex.unlock();
                        m_decode->m_rowList[data.name]->mutex.unlock();
                        m_sortPositionList[ii].dataPosition=i+1;
                    }
                }
            }
        }
        m_sortDataMutex.lock();
        if(!m_isSearch)
            m_sortCount=m_sortData.count();
        else{
            m_searchData.clear();
            QList<QString> needStr;
            QList<QString> notStr;
            QList<QString> split;
            if(m_searchText.indexOf("&&")!=-1)
                split=m_searchText.split("&&");
            else
                split.append(m_searchText);
            for(auto &i:split){
                if(i.startsWith("!!")&&i.length()>2)
                    notStr.append(i.right(i.length()-2));
                else
                    needStr.append(i);
            }
            QString str;
            bool isAppend;
            for(auto &i:m_sortData){
                str=i.longText.toLower();
                isAppend=true;
                for(int j=0;j<needStr.size();j++){
                    if(!str.contains(needStr[j])){
                        isAppend=false;
                        break;
                    }
                }
                if(isAppend){
                    for(int j=0;j<notStr.size();j++){
                        if(str.contains(notStr[j])){
                            isAppend=false;
                            break;
                        }
                    }
                }
                if(isAppend)
                    m_searchData.append(i);
            }
            m_sortCount=m_searchData.count();
        }
        m_sortDataMutex.unlock();
        if(m_set.bufferFirst+m_set.bufferSize>m_sortCount)
            m_set.bufferFirst=m_sortCount-m_set.bufferSize;
        if(m_set.bufferFirst<0)
            m_set.bufferFirst=0;
        refreshBuffer();
    }
end:
    m_runStateMutex.lock();
    if(m_isNext)
    {
        m_isNext=false;
        m_runStateMutex.unlock();
        QtConcurrent::run(this, &DecodeTableModel::sortData);
    }else{
        m_isRun=0;
        m_runStateMutex.unlock();
    }
}

bool DecodeTableModel::isExport() const
{
    return m_count!=0&&m_isComplete;
}

void DecodeTableModel::resetSort()
{
    for(auto &i:m_sortPositionList)
    {
        i.dataPosition=0;
        i.rowPosition=0;
    }
    m_sortDataMutex.lock();
    m_searchData.clear();
    m_sortData.clear();
    m_data.clear();
    setCount(0);
    m_sortDataMutex.unlock();
}

void DecodeTableModel::saveTableReady(QJSValue val)
{
    m_session->m_tableData=m_isSearch?&m_searchData:&m_sortData;
    m_session->m_tableRowType=&m_rowType;
    m_session->m_tableMultiply=m_decode->m_multiply;
    m_session->m_tableSaveIndex.clear();
    for(qint32 i=0;i<val.property("length").toUInt();i++)
        m_session->m_tableSaveIndex.append(val.property(i).toInt());
}

bool DecodeTableModel::isStop()
{
    return m_isStop;
}

void DecodeTableModel::setStop(bool stop)
{
    m_isStop=stop;
}

int DecodeTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_count;
}

int DecodeTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_column;
}

QHash<int, QByteArray> DecodeTableModel::roleNames() const
{
    return m_roleName;
}

QVariant DecodeTableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    m_sortDataMutex.lock();
    if(row < 0 || row >= m_count){
        m_sortDataMutex.unlock();
        return QVariant("");
    }
    DecodeRowData rowData=m_data[row];
    m_sortDataMutex.unlock();
    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case 0:
            return nsToShowStr((double)rowData.start*m_decode->m_multiply);
        case 1:
            return nsToShowStr((double)((rowData.end-rowData.start)*m_decode->m_multiply));
        case 2:
        {
            for (auto i : m_rowType[rowData.decodeIndex])
            {
                if(i["list"].toArray().contains((int)rowData.type))
                    return i["desc"].toString();
            }
        }
            break;
        case 3:
            return rowData.longText;
        }
        break;
    case TextRole:
        return rowData.longText;
    case StartTextRole:
        return nsToShowStr((double)rowData.start*m_decode->m_multiply);
    case StartRole:
        return rowData.start*m_decode->m_multiply;
    case EndRole:
        return rowData.end*m_decode->m_multiply;
    case TimeTextRole:
        return nsToShowStr((double)((rowData.end-rowData.start)*m_decode->m_multiply));
    default:
        return QVariant("");
    }
    return QVariant("");
}

void DecodeTableModel::setCount(qint32 count)
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
    emit isExportChanged();
}
