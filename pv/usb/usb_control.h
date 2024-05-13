/* Copyright(c),广州星翼电子科技有限公司
* All rights reserved.*
* 文件名称：usbControl.h
* 摘    要：usb协议控制层
* 当前版本：1.0
* 作    者：J(俊杰)
* 完成日期：2022年7月14日
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

    /// <summary>
    /// 读取数据
    /// </summary>
    /// <param name="pData">数据指针</param>
    /// <param name="iCount">读取次数，1次=512K，1以下则为无限读，直至没有数据</param>
    /// <param name="timeOut">超时时间</param>
    /// <param name="maxTransferSize">使用的缓冲区数</param>
    /// <returns></returns>
    bool Read(Data_* pData, int iCount = -1, qint32 timeOut=150, qint32 maxTransferSize=READ_TRANSFER_LENGTH);

    /// <summary>
    /// 同步读取数据并且不进行转换，仅用与MCU通讯
    /// </summary>
    /// <param name="pData">数据指针</param>
    /// <param name="len">读取长度，大于2048则开启自动转换</param>
    /// <returns></returns>
    bool ReadSynchronous(Data_* pData, qint32 len=512);

    /// <summary>
    /// 读取数据一次最低2048
    /// </summary>
    /// <param name="pData">数据指针</param>
    /// <returns></returns>
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
    //Data_* m_lastData=nullptr;
    int m_ThreadState = 0;//0=指令停止，1=正常，2=用户停止，3=异常断开
    QString m_SessionID;
    qint32 m_ChannelCount = 0;			/* 通道数 */
    qint32 m_mcuVersion = 0;
    qint32 m_fpgaVersion = 0;
    qint16 m_deviceVersion = 0;
    bool m_busy=false;
    bool m_NoDevice=false;
    qint32 m_lastError=0;
    //qint32 m_SamplingFrequency = 0;	/* 采样频率kHz */

private:
    bool Write(quint8* data, qint32 len);
    bool Write(quint8 code, quint8 data[], qint32 len, bool isWaitReply=false);
    bool SendToDevice(quint8* data, qint64 len);
    bool SendToMCU(quint8 *data, qint64 len);
    //bool WaitReply(quint8 code);

private:
    QMutex m_lock;
    bool _m_isInit = false;
    LogicAnalyzer* _m_logic=nullptr;
    DataToDevice _m_dataToDevice;
    DataToPC _m_dataToPC;
    int m_port=-1;
    HANDLE hThread;
};

#endif // USBCONTROL_H
