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

#pragma once


#include "libusb.h"
#include "qmutex.h"
#include <QList>
#include <QThread>
#include <QObject>
#include "../static/util.h"

#define WRITE_TRANSFER_LENGTH 4
#define READ_TRANSFER_LENGTH 40
#define READ_TRANSFER_BUFFER_SIZE 16384 

class usb_loop_thread;

struct Data_ {
    quint8* buf = nullptr;
    quint32 len = 0;
};

struct ToDeviceFlag{
    qint32 dataCount = 0;
    qint32 count = 0;
    quint8 error = 1;
};

struct FindData {
    libusb_context* context=nullptr;
    libusb_device* dev;
    int port;
};

struct LogicAnalyzerFlag{
    quint8 init : 1;
    quint8 exit : 1;
};

struct LogicAnalyzer{
    libusb_context* context=nullptr;
    libusb_device* dev;
    libusb_device_handle* device_handle=nullptr;
    usb_loop_thread* usb_loop=nullptr;
    quint32 port;
    quint8 to_mcu_ep;
    quint8 to_pc_ep;
    volatile LogicAnalyzerFlag flag;
};

struct DataToDevice {
    qint32 bufIndex = 0;
    quint8* buf[WRITE_TRANSFER_LENGTH];
    struct libusb_transfer* transfer[WRITE_TRANSFER_LENGTH];
    ToDeviceFlag flag;
    DataToDevice() {
        for (int i = 0; i < WRITE_TRANSFER_LENGTH; i++)
        {
            buf[i] = nullptr;
            transfer[i] = nullptr;
        }
    }
    ~DataToDevice() {
        for (int i = 0; i < WRITE_TRANSFER_LENGTH; i++) {
            if (transfer[i] != nullptr) {
                libusb_free_transfer(transfer[i]);
                transfer[i] = nullptr;
            }
            if (buf[i] != nullptr) {
                delete[] buf[i];
                buf[i] = nullptr;
            }
        }
    }
};

struct DataToPC_OK{
    libusb_transfer* transfer;
    quint8* buffer;
    qint64 length;
};

struct DataToPC {
    mutable QMutex mutex;
    QList<DataToPC_OK> ok;
    quint8* singleBuffer=nullptr;
    quint8* buf[READ_TRANSFER_LENGTH];
    qint64 len;
    struct libusb_transfer* transfer[READ_TRANSFER_LENGTH];
    ToDeviceFlag flag;
    DataToPC() {
        for (int i = 0; i < READ_TRANSFER_LENGTH; i++)
        {
            buf[i] = nullptr;
            transfer[i] = nullptr;
        }
    }
    ~DataToPC() {
        for (int i = 0; i < READ_TRANSFER_LENGTH; i++) {
            if (transfer[i] != nullptr) {
                libusb_free_transfer(transfer[i]);
                transfer[i] = nullptr;
            }
            if (buf[i] != nullptr) {
                delete[] buf[i];
                buf[i] = nullptr;
            }
        }
        if(singleBuffer!=nullptr)
            delete[] singleBuffer;
    }
};
int logic_analyzer_init(LogicAnalyzer* m);
void logic_analyzer_deinit(LogicAnalyzer* m);
int logic_analyzer_open(LogicAnalyzer* m);
void logic_analyzer_close(LogicAnalyzer* m);
QList<FindData> logic_analyzer_find();
void logic_analyzer_convert_to_pc(const void* s, void* d, unsigned len);
void logic_analyzer_convert_to_device(const void* s, void* d, unsigned len);
bool logic_analyzer_reopen(LogicAnalyzer *m);

class usb_loop_thread : public QThread
{
    Q_OBJECT
public:
    usb_loop_thread(LogicAnalyzer* m);

protected:
    void run();

private:
    LogicAnalyzer* m_logicAnalyzer;
};
