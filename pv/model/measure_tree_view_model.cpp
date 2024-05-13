#include "measure_tree_view_model.h"

measureTreeViewModel::measureTreeViewModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleName[NameRole]="name";
    m_roleName[MeasureIDRole]="measureID";
    m_roleName[ChannelIDRole]="channelID";
    m_roleName[StartRole]="start";
    m_roleName[EndRole]="end";
    m_roleName[StartTextRole]="startText";
    m_roleName[StartTextDetailRole]="startTextDetail";
    m_roleName[EndTextRole]="endText";
    m_roleName[EndTextDetailRole]="endTextDetail";
    m_roleName[TimeRole]="time";
    m_roleName[TimeDetailRole]="timeDetail";
    m_roleName[MinFreqRole]="minFreq";
    m_roleName[MinFreqDetailRole]="minFreqDetail";
    m_roleName[MaxFreqRole]="maxFreq";
    m_roleName[MaxFreqDetailRole]="maxFreqDetail";
    m_roleName[RisingRole]="rising";
    m_roleName[FallingRole]="falling";
    m_roleName[NoteRole]="note";
    m_roleName[DataColorRole]="dataColor";
    m_roleName[IsSelectRole]="isSelect";
    m_roleName[IsAutoOffsetRole]="isAutoOffset";
    m_roleName[IsCalcRole]="isCalc";
    m_roleName[OffsetRole]="offset";
}

void measureTreeViewModel::init(QString sessionID)
{
    m_session=DataService::getInstance()->getSession(sessionID);
    if(m_session)
    {
        m_measure=&m_session->m_measure;
        refresh();
    }
}

void measureTreeViewModel::append()
{
    beginInsertRows(QModelIndex(), m_count, m_count);
    m_count++;
    endInsertRows();
    refresh();
}

void measureTreeViewModel::remove(int measureID)
{
    qint32 count=m_measure->count();
    for(qint32 j=0;j<count;j++){
        MeasureData data = m_measure->at(j);
        if(data.id==measureID){
            beginRemoveRows(QModelIndex(), j, j);
            m_measure->remove(j);
            m_count--;
            endRemoveRows();
            break;
        }
    }
    if(m_measure->count()==0)
        m_session->m_measureIndex=0;
}

void measureTreeViewModel::refresh()
{
    QModelIndex start_index = createIndex(0, 0);
    QModelIndex end_index = createIndex((m_measure->size() - 1), 0);
    emit dataChanged(start_index, end_index);
}

void measureTreeViewModel::refresh(int measureID)
{
    qint32 count=m_measure->count();
    for(qint32 j=0;j<count;j++){
        MeasureData data = m_measure->at(j);
        if(data.id==measureID){
            QModelIndex start_index = createIndex(j, 0);
            QModelIndex end_index = createIndex(j, 0);
            emit dataChanged(start_index, end_index);
            break;
        }
    }
}

QVariantMap measureTreeViewModel::get(int measureID) {
    QHashIterator<int, QByteArray> i(roleNames());
    QVariantMap res;
    qint32 count=m_measure->count();
    for(qint32 j=0;j<count;j++){
        MeasureData data = m_measure->at(j);
        if(data.id==measureID){
            QModelIndex idx = index(j, 0);
            while (i.hasNext()) {
                i.next();
                res[i.value()] = idx.data(i.key());
            }
            break;
        }
    }
    return res;
}

void measureTreeViewModel::set(int measureID, QString key, QVariant value)
{
    QHashIterator<int, QByteArray> i(roleNames());
    qint32 count=m_measure->count();
    for(qint32 j=0;j<count;j++){
        MeasureData data = m_measure->at(j);
        if(data.id==measureID){
            while (i.hasNext()) {
                i.next();
                if(i.value()==key){
                    switch (i.key()) {
                    case IsAutoOffsetRole:
                        (*m_measure)[j].isAutoOffset=value.toBool();
                        break;
                    case DataColorRole:
                        (*m_measure)[j].dataColor=value.toString();
                        refreshAt(j);
                        break;
                    case NoteRole:
                        (*m_measure)[j].note=value.toString();
                        refreshAt(j);
                        break;
                    }
                }
            }
            break;
        }
    }
}

void measureTreeViewModel::setSelect(int measureID)
{
    qint32 count=m_measure->count();
    for(qint32 j=0;j<count;j++)
        (*m_measure)[j].isSelect=(m_measure->at(j).id==measureID);
    refresh();
}

qint32 measureTreeViewModel::getSelectID()
{
    qint32 count=m_measure->count();
    for(qint32 j=0;j<count;j++)
    {
        if(m_measure->at(j).isSelect)
            return m_measure->at(j).id;
    }
    return -1;
}

int measureTreeViewModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || m_measure==nullptr)
        return 0;
    return m_measure->size();
}

QHash<int, QByteArray> measureTreeViewModel::roleNames() const
{
    return m_roleName;
}

QVariant measureTreeViewModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row >= m_measure->count())
        return QVariant("");

    MeasureData data = m_measure->at(row);

    switch (role) {
    case NameRole:
        return data.name;
    case MeasureIDRole:
        return data.id;
    case ChannelIDRole:
        return data.channelID;
    case StartRole:
        return data.start;
    case EndRole:
        return data.end;
    case StartTextRole:
        return data.str.start;
    case StartTextDetailRole:
        return data.str.startDetail;
    case EndTextRole:
        return data.str.end;
    case EndTextDetailRole:
        return data.str.endDetail;
    case TimeRole:
        return data.str.time;
    case TimeDetailRole:
        return data.str.timeDetail;
    case MinFreqRole:
        return data.str.minFreq;
    case MinFreqDetailRole:
        return data.str.minFreqDetail;
    case MaxFreqRole:
        return data.str.maxFreq;
    case MaxFreqDetailRole:
        return data.str.maxFreqDetail;
    case RisingRole:
        return data.rising;
    case FallingRole:
        return data.falling;
    case NoteRole:
        return data.note;
    case DataColorRole:
        return data.dataColor;
    case IsSelectRole:
        return data.isSelect;
    case IsAutoOffsetRole:
        return data.isAutoOffset;
    case IsCalcRole:
        return data.isCalc;
    case OffsetRole:
        return data.offset;
    default:
        return QVariant("");
    }
}

void measureTreeViewModel::refreshAt(int row)
{
    QModelIndex start_index = createIndex(row, 0);
    QModelIndex end_index = createIndex(row, 0);
    emit dataChanged(start_index, end_index);
}
