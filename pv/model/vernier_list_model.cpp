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

#include "vernier_list_model.h"

VernierListModel::VernierListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleName[NameRole]="name";
    m_roleName[VisibleRole]="visible_";
    m_roleName[VernierIDRole]="vernierID";
    m_roleName[DataColorRole]="dataColor";
    m_roleName[PositionRole]="position";
    m_roleName[PositionStrRole]="positionStr";
    m_roleName[NoteRole]="note";
    m_roleName[ShowTextRole]="showText";
    m_roleName[CostRole]="cost";
}

void VernierListModel::init(QString sessionID)
{
    m_session=DataService::getInstance()->getSession(sessionID);
    if(m_session)
    {
        m_vernier=&m_session->m_vernier;
        refresh();
    }
}

void VernierListModel::append()
{
    beginInsertRows(QModelIndex(), m_count, m_count);
    m_count++;
    endInsertRows();
    emit countChanged();
}

void VernierListModel::remove(int vernierID)
{
    if(vernierID>0){
        qint32 count=m_vernier->count();
        for(qint32 j=0;j<count;j++){
            VernierData data = m_vernier->at(j);
            if(data.id==vernierID){
                qint32 dataCount=data.busyList.count();
                if(dataCount>0){
                    QString str=QObject::tr("欲删除标签被占用：");
                    for(qint32 i=0;i<dataCount;i++)
                        str+="\n    "+data.busyList[i].str;
                    m_session->showErrorMsg(str,false);
                }else{
                    count--;
                    if(data.isSelect)
                        emit closePopup();
                    beginRemoveRows(QModelIndex(), j, j);
                    m_vernier->remove(j);
                    m_count--;
                    endRemoveRows();
                    emit countChanged();
                }
                break;
            }
        }
        if(count==1)
            m_session->m_vernierIndex=1;
    }else
    {
        set(0,"visible_",false);
        set(0,"note","");
        set(0,"dataColor","#26A3D9");
        emit countChanged();
    }
}

void VernierListModel::removeAll(bool isForce)
{
    bool isError=false;
    qint32 count=m_vernier->count(),j=0;
    if(count>1){
        while(true){
            if(j==m_vernier->count())
                break;
            if(m_vernier->at(j).id==0)
                j++;
            else{
                if(m_vernier->at(j).busyList.count()==0||isForce){
                    beginRemoveRows(QModelIndex(), j, j);
                    m_vernier->remove(j);
                    m_count--;
                    endRemoveRows();
                }
                else{
                    isError=true;
                    j++;
                }
            }
        }
    }
    if(!isError)
        m_session->m_vernierIndex=1;
    emit countChanged();
    emit closePopup();
    set(0,"visible_",false);
    set(0,"note","");
    set(0,"dataColor","#26A3D9");
    if(isError)
        m_session->showErrorMsg(QObject::tr("部分标签被占用，详细占用信息请逐个删除查看。"),false);
}

void VernierListModel::refresh()
{
    QModelIndex start_index = createIndex(0, 0);
    QModelIndex end_index = createIndex((m_vernier->size() - 1), 0);
    emit dataChanged(start_index, end_index);
}

void VernierListModel::refresh(int vernierID)
{
    qint32 count=m_vernier->count();
    for(qint32 j=0;j<count;j++){
        VernierData data = m_vernier->at(j);
        if(data.id==vernierID){
            QModelIndex start_index = createIndex(j, 0);
            QModelIndex end_index = createIndex(j, 0);
            emit dataChanged(start_index, end_index);
            break;
        }
    }
}

void VernierListModel::appendBusy(int vernierID, QString busyID, qint32 flag, QString str)
{
    qint32 count=m_vernier->count();
    for(qint32 j=0;j<count;j++){
        VernierData data = m_vernier->at(j);
        if(data.id==vernierID&&data.id!=0){
            VernierDataBusy tmp;
            tmp.id=busyID;
            tmp.flag=flag;
            tmp.str=str;
            (*m_vernier)[j].busyList.append(tmp);
            break;
        }
    }
}

void VernierListModel::removeBusy(int vernierID, QString busyID, qint32 flag)
{
    qint32 count=m_vernier->count();
    for(qint32 j=0;j<count;j++){
        VernierData data = m_vernier->at(j);
        if(data.id==vernierID&&data.id!=0){
            qint32 count2=data.busyList.count();
            for(qint32 i=0;i<count2;i++){
                VernierDataBusy tmp = data.busyList[i];
                if(tmp.id==busyID&&tmp.flag==flag){
                    (*m_vernier)[j].busyList.removeAt(i);
                    break;
                }
            }
            break;
        }
    }
}

QVariantMap VernierListModel::getAt(int vernierID) {
    QHashIterator<int, QByteArray> i(roleNames());
    QVariantMap res;
    qint32 count=m_vernier->count();
    for(qint32 j=0;j<count;j++){
        VernierData data = m_vernier->at(j);
        if(data.id==vernierID){
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

QVariantMap VernierListModel::get(int index)
{
    if(m_vernier && index<m_vernier->count() && index>=0)
        return getAt(m_vernier->at(index).id);
    return QVariantMap();
}

void VernierListModel::set(int vernierID, QString key, QVariant value)
{
    QHashIterator<int, QByteArray> i(roleNames());
    qint32 count=m_vernier->count();
    for(qint32 j=0;j<count;j++){
        VernierData data = m_vernier->at(j);
        if(data.id==vernierID){
            while (i.hasNext()) {
                i.next();
                if(i.value()==key){
                    switch (i.key()) {
                    case NoteRole:
                        (*m_vernier)[j].note=value.toString();
                        break;
                    case DataColorRole:
                        (*m_vernier)[j].dataColor=value.toString();
                        refreshAt(j);
                        break;
                    case PositionRole:
                        (*m_vernier)[j].position=value.toLongLong();
                        refreshAt(j);
                        break;
                    case PositionStrRole:
                        (*m_vernier)[j].positionStr=value.toString();
                        refreshAt(j);
                        break;
                    case VisibleRole:
                        (*m_vernier)[j].visible=value.toBool();
                        refreshAt(j);
                        break;
                    }
                }
            }
            break;
        }
    }
}

int VernierListModel::count()
{
    if (m_vernier==nullptr)
        return 0;
    return m_count;
}

void VernierListModel::refreshAt(int row)
{
    QModelIndex start_index = createIndex(row, 0);
    QModelIndex end_index = createIndex(row, 0);
    emit dataChanged(start_index, end_index);
}

int VernierListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || m_vernier==nullptr)
        return 0;
    return m_count;
}

QHash<int, QByteArray> VernierListModel::roleNames() const
{
    return m_roleName;
}

QVariant VernierListModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row >= m_vernier->count())
        return QVariant("");

    VernierData data = m_vernier->at(row);
    switch (role) {
    case NameRole:
    case ShowTextRole:
        return data.name;
    case VisibleRole:
        return data.visible;
    case VernierIDRole:
        return data.id;
    case DataColorRole:
        return data.dataColor;
    case PositionRole:
    case CostRole:
        return data.position;
    case PositionStrRole:
        return data.positionStr;
    case NoteRole:
        return data.note;
    default:
        return QVariant("");
    }
}
