#include "shared_memory_helper.h"

SharedMemoryHelper::SharedMemoryHelper()
{
    m_shared.setKey(m_key);
    m_isAttach=m_shared.attach();
    if(!m_isAttach){
        m_isAttach=m_shared.create(m_sharedSize);
        if(m_isAttach){
            m_shared.lock();
            void* pData=m_shared.data();
            memset(pData,0,m_sharedSize);
            m_shared.unlock();
        }
    }
}

void SharedMemoryHelper::increase(qint32 position)
{
    if(!m_isAttach || position+1>=m_sharedSize)
        return;
    m_shared.lock();
    quint8* pData=(quint8*)m_shared.data();
    *(pData+position)=*(pData+position)+1;
    m_shared.unlock();
}

void SharedMemoryHelper::decrement(qint32 position)
{
    if(!m_isAttach || position+1>=m_sharedSize)
        return;
    m_shared.lock();
    quint8* pData=(quint8*)m_shared.data();
    if(*(pData+position)>0)
        *(pData+position)=*(pData+position)-1;
    m_shared.unlock();
}

void SharedMemoryHelper::appendConnectDevice(qint32 port)
{
    quint8 num=getU8Data(DEVICE_COUNT);
    QList<quint8> listData;
    if(num>0)
        listData=getDataList(DEVICE_DATA,num);
    num++;
    if(!listData.contains(port))
        listData.append(port);
    setData(DEVICE_COUNT,num);
    setData(DEVICE_DATA,0,m_sharedSize-DEVICE_DATA-1);
    setData(DEVICE_DATA,listData);
}

void SharedMemoryHelper::removeConnectDevice(qint32 port)
{
    quint8 num=getU8Data(DEVICE_COUNT);
    QList<quint8> listData;
    if(num>0){
        listData=getDataList(DEVICE_DATA,num);
        num--;
    }
    listData.removeOne(port);
    setData(DEVICE_COUNT,num);
    setData(DEVICE_DATA,0,m_sharedSize-DEVICE_DATA-1);
    setData(DEVICE_DATA,listData);
}

bool SharedMemoryHelper::setFileDirCrean(bool set)
{
    if(!m_isAttach)
        return false;
    bool reft=true;
    m_shared.lock();
    quint8* pData=(quint8*)m_shared.data();
    if(pData[FILE_DIR_CREAN] && set)
        reft=false;
    else
        pData[FILE_DIR_CREAN]=set;
    m_shared.unlock();
    return reft;
}

bool SharedMemoryHelper::resetAllData()
{
    if(!m_isAttach)
        return false;
    m_shared.lock();
    void* pData=m_shared.data();
    memset(pData,0,m_sharedSize);
    m_shared.unlock();
    return true;
}

bool SharedMemoryHelper::isAttach()
{
    return m_isAttach;
}

bool SharedMemoryHelper::isUpdate()
{
    return getU8Data(UPDATE_NOW);
}

QList<quint8> SharedMemoryHelper::getConnectDevice()
{
    return getDataList(DEVICE_DATA, getU8Data(DEVICE_COUNT));
}

void SharedMemoryHelper::setData(qint32 position, quint8 val, qint32 len)
{
    if(!m_isAttach || position+1>=m_sharedSize)
        return;
    if(len>0 && position+len<m_sharedSize){
        m_shared.lock();
        quint8* pData=(quint8*)m_shared.data();
        memset(pData+position,val,len);
        m_shared.unlock();
    }
}

void SharedMemoryHelper::setData(qint32 position, quint8 data)
{
    if(!m_isAttach || position+1>=m_sharedSize)
        return;
    m_shared.lock();
    quint8* pData=(quint8*)m_shared.data();
    *(pData+position)=data;
    m_shared.unlock();
}

void SharedMemoryHelper::setData(qint32 position, QList<quint8> listData)
{
    if(!m_isAttach || position+1>=m_sharedSize)
        return;
    if(listData.count()>0 && position+listData.count()<m_sharedSize){
        m_shared.lock();
        quint8* pData=(quint8*)m_shared.data();
        for(qint32 i=0;i<listData.count();i++)
            *(pData+position+i)=listData[i];
        m_shared.unlock();
    }
}

quint8 SharedMemoryHelper::getU8Data(qint32 position)
{
    quint8 ret=0;
    if(!m_isAttach || position+1>=m_sharedSize)
        return ret;
    m_shared.lock();
    quint8* pData=(quint8*)m_shared.data();
    ret=*(pData+position);
    m_shared.unlock();
    return ret;
}

QList<quint8> SharedMemoryHelper::getDataList(qint32 position, qint32 len)
{
    QList<quint8> ret;
    if(!m_isAttach || position+1>=m_sharedSize)
        return ret;
    if(len>0 && position+len<m_sharedSize){
        m_shared.lock();
        quint8* pData=(quint8*)m_shared.data();
        for(qint32 i=0;i<len;i++)
            ret.append(*(pData+position+i));
        m_shared.unlock();
    }
    return ret;
}
