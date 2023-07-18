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

#include "decode_list_model.h"

DecodeListModel::DecodeListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleName[NameRole]="name";
    m_roleName[NoteRole]="note";
    m_roleName[IDRole]="id";
    m_decodeService=DataService::getInstance()->getDecodeServer();
    m_decoderList=m_decodeService->getDecodeList();
    refresh();
}

void DecodeListModel::refresh()
{
    if (m_decoderList==nullptr)
        return;
    setCount(0);
    if(m_decodeService==nullptr && m_decoderList->count()<1)
        return;
    m_decoderMutex.lock();
    m_decoderShowList.clear();
    for(auto &i : *m_decoderList){
        if(m_isSearch)
        {
            if(QString(i->name).toLower().contains(m_searchText)||QString(i->id).toLower().contains(m_searchText))
                m_decoderShowList.append(i);
        }else
            m_decoderShowList.append(i);
    }
    qint32 tmp=m_decoderShowList.count();
    m_decoderMutex.unlock();
    setCount(tmp);
    if(m_count>0)
    {
        QModelIndex start_index = createIndex(0, 0);
        QModelIndex end_index = createIndex(m_count-1, 0);
        emit dataChanged(start_index, end_index);
    }
}

void DecodeListModel::search(QString text)
{
    m_isSearch=true;
    m_searchText=text.toLower();
}

void DecodeListModel::exitSearch()
{
    m_isSearch=false;
}

bool DecodeListModel::containName(QString name)
{
    for(auto &d : *m_decoderList){
        if(QString(d->name)==name)
            return true;
    }
    return false;
}

void DecodeListModel::setCount(qint32 count)
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

int DecodeListModel::rowCount(const QModelIndex &parent) const
{
    return m_count;
}

QHash<int, QByteArray> DecodeListModel::roleNames() const
{
    return m_roleName;
}

QVariant DecodeListModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    m_decoderMutex.lock();
    if(row < 0 || row >= m_decoderShowList.count()){
        m_decoderMutex.unlock();
        return QVariant("");
    }
    atk_decoder* data = m_decoderShowList[row];
    m_decoderMutex.unlock();
    switch (role) {
    case NameRole:
        return data->name;
    case IDRole:
        return data->id;
    case NoteRole:
        return m_decodeService->getDecodeDoc(data->id);
    default:
        return QVariant("");
    }
}

