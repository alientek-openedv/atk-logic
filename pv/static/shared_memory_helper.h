#ifndef SHAREDMEMORYHELPER_H
#define SHAREDMEMORYHELPER_H

#include "qsharedmemory.h"

enum DataPosition{
    SAVE_FILE=0,
    LOAD_FILE=1,
    FILE_DIR_CREAN=2,
    UPDATE_NOW=511,
    DEVICE_COUNT=512,
    DEVICE_DATA=513//DEVICE_DATA往后区域都不能修改
};

class SharedMemoryHelper
{
public:
    SharedMemoryHelper();
    quint8 getU8Data(qint32 position);
    void increase(qint32 position);
    void decrement(qint32 position);
    void appendConnectDevice(qint32 port);
    void removeConnectDevice(qint32 port);
    bool setFileDirCrean(bool set);
    bool resetAllData();
    bool isAttach();
    bool isUpdate();
    QList<quint8> getConnectDevice();

private:
    void setData(qint32 position, quint8 val, qint32 len);
    void setData(qint32 position, quint8 data);
    void setData(qint32 position, QList<quint8> listData);
    QList<quint8> getDataList(qint32 position, qint32 len);

private:
    QString m_key="atk_logic_shared_memory_block";
    int m_sharedSize=1024;
    QSharedMemory m_shared;
    bool m_isAttach=false;
};

#endif // SHAREDMEMORYHELPER_H
