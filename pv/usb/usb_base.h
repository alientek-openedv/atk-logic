/* Copyright(c),广州星翼电子科技有限公司
* All rights reserved.*
* 文件名称：usbBase.h
* 摘    要：usb协议基础层
* 当前版本：1.0
* 作    者：J(俊杰)
* 完成日期：2022年7月14日
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
#define READ_TRANSFER_BUFFER_SIZE 16384 //16K

class usb_loop_thread;

struct Data_ {
    quint8* buf = nullptr;
    quint32 len = 0;
};//读取的数据

struct ToDeviceFlag{
    qint32 dataCount = 0;
    qint32 count = 0;
    quint8 error = 1;
};

struct FindData {
    libusb_context* context=nullptr;//usb设备
    libusb_device* dev;
    int port;
};//寻找的数据结构体

struct LogicAnalyzerFlag{
    quint8 init : 1;//初始化标志,1:表示初始化成功，0:表示初始化识别
    quint8 exit : 1;//退出事件循环标志,1:表示需要退出事件循环，0:表示继续事件循环
};

struct LogicAnalyzer{
    libusb_context* context=nullptr;//usb设备
    libusb_device* dev;//usb设备
    libusb_device_handle* device_handle=nullptr;//设备
    usb_loop_thread* usb_loop=nullptr;
    quint32 port;//端口
    quint8 to_mcu_ep;//发送数据到mcu的端点
    quint8 to_pc_ep;//接收数据的端点
    volatile LogicAnalyzerFlag flag;//标志
};//逻辑分析仪驱动相关参数

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
};//测试发送数据到mcu的相关参数

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
};//从mcu接收数据测试的相关参数

/******************************************************************************
 * 函数: logic_analyzer_init
 * 描述: 初始化LogicAnalyzer
 * 输入: m: 逻辑分析仪驱动相关参数
 * 返回: 成功返回true,否则false
 * ****************************************************************************/
int logic_analyzer_init(LogicAnalyzer* m);

/******************************************************************************
 * 函数: logic_analyzer_deinit
 * 描述: 释放LogicAnalyzer的资源
 * 输入: LogicAnalyzer: 逻辑分析仪驱动相关结构体
 * ****************************************************************************/
void logic_analyzer_deinit(LogicAnalyzer* m);

/******************************************************************************
 * 函数: logic_analyzer_open
 * 描述: 打开usb设备
 * 输入: m: 逻辑分析仪驱动相关结构体
 * ****************************************************************************/
int logic_analyzer_open(LogicAnalyzer* m);

/******************************************************************************
 * 函数: logic_analyzer_close
 * 描述: 关闭usb设备
 * 输入: LogicAnalyzer: 逻辑分析仪驱动相关结构体
 * ****************************************************************************/
void logic_analyzer_close(LogicAnalyzer* m);

/******************************************************************************
 * 函数: logic_analyzer_find
 * 描述: 寻找新的设备
 * ****************************************************************************/
QList<FindData> logic_analyzer_find();

/******************************************************************************
 * 函数: logic_analyzer_convert_to_pc
 * 描述: 发送数据给mcu前的转换
 * 输入: s: 指向转换前数据缓存的指针
 * 输出: d: 指向转换后数据缓存的指针
 * 输入: len: 数据长度，必须是2048的整数倍，而且不能为0
 * ****************************************************************************/
void logic_analyzer_convert_to_pc(const void* s, void* d, unsigned len);

/******************************************************************************
 * 函数: logic_analyzer_convert_to_device
 * 描述: 从mcu接收数据后的转换
 * 输入: s: 指向转换前数据缓存的指针
 * 输出: d: 指向转换后数据缓存的指针
 * 输入: len: 数据长度，必须是2048的整数倍，而且不能为0
 * ****************************************************************************/
void logic_analyzer_convert_to_device(const void* s, void* d, unsigned len);

/******************************************************************************
 * 函数: logic_analyzer_open
 * 描述: 重新打开设备打开usb设备
 * 输入: LogicAnalyzer: 逻辑分析仪驱动相关结构体
 * ****************************************************************************/
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
