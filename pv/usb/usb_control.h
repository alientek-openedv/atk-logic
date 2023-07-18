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



#ifndef USB_CONTROL_H
#define USB_CONTROL_H

#pragma once
#include <QList>
#include <QString>
#include <QMutex>
#include "usb_base.h"
#include "../static/util.h"
#include <pv/data/analysis.h>
#include <pv/static/log_help.h>


typedef void *HANDLE;

#define MCU_ENTER_KEY "ATK-LOGIC-ANALYZER-MCU-V1"
#define FPGA_ENTER_KEY "ATK-LOGIC-ANALYZER-FPGA-V1"
#define BOOTLOADER_ENTER_KEY "ATK-LOGIC-ANALYZER"
#define SET_VERSION_ENTER_KEY "ATK-LOGIC-ANALYZER-CONFIG"
void printf_buf(const uint16_t *p, unsigned len);
struct AnalysisData;
class DeviceAPI;
class Analysis;
class USBControl
{
public:
    USBControl();
    ~USBControl();

    bool IsInit();

    bool Init(libusb_device* dev, libusb_context* context, qint32 channelCount, int port);

    QList<FindData> static Find();

    void static Find_Free(libusb_device* dev);

    void DeInit();

    QString ReadStr();
    bool Read(Data_* pData, int iCount = -1, qint32 timeOut=150, qint32 maxTransferSize=READ_TRANSFER_LENGTH);
    bool ReadSynchronous(Data_* pData, qint32 len=512);
    bool ReadSingle(Data_* pData);

    int GetPort();

    bool SetResetState(qint8 state);
    bool GetResetState();
    bool GetDeviceData(bool isWaitReply=true);
    bool GetMCUVersion();
    bool EnterBootloader();
    bool EnterHardwareUpdate(bool isMCU);
    bool SendUpdateData(const uint8_t *data, uint16_t len, bool isMCU, bool isLast=false);
    bool RestartMCU();
    bool WaitHardwareUpdate(quint8 code);

    bool ParameterSetting(quint8* data, quint32 len);
    bool SimpleTrigger(quint8* data, quint32 len);
    bool PWM(quint8* data, quint32 len);
    bool CloseAllPWM();
    bool Stop();
    bool Register(quint8* data, quint32 len);
    bool Exit();

public:
    int m_ThreadState = 0;
    QString m_SessionID;
    qint32 m_ChannelCount = 0;
    qint32 m_mcuVersion = 0;
    qint32 m_fpgaVersion = 0;
    bool m_busy=false;
    bool m_NoDevice=false;
    qint32 m_lastError=0;
    
private:
    bool Write(quint8* data, qint32 len);
    bool Write(quint8 code, quint8 data[], qint32 len, bool isWaitReply=false);
    bool SendToDevice(quint8* data, qint64 len);
    bool SendToMCU(quint8 *data, qint64 len);
    
private:
    QMutex m_lock;
    bool _m_isInit = false;
    LogicAnalyzer* _m_logic=nullptr;
    DataToDevice _m_dataToDevice;
    DataToPC _m_dataToPC;
    int m_port=-1;
    HANDLE hThread;
};

#endif 
