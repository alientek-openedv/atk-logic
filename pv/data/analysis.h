#ifndef ANALYSIS_H
#define ANALYSIS_H


#pragma once

#include "pv/usb/usb_control.h"
#include "pv/static/log_help.h"

struct Data_;
class USBControl;

/// <summary>
/// 检查指令数据头部是否符合数据格式并获取数据格式类型
/// </summary>
/// <param name="pData">数据指针</param>
/// <returns>符合返回数据格式类型，否则返回-1</returns>
int analysis_check_data_header(quint8* pData);

/// <summary>
/// 检查指令数据尾部是否符合数据格式
/// </summary>
/// <param name="pData">数据指针</param>
/// <param name="llDataLength">数据总长</param>
/// <returns>符合返回true</returns>
bool analysis_check_data_bottom(quint8* pData,qint64 llDataLength);

/// <summary>
/// 获取指令数据
/// </summary>
/// <param name="pData">数据指针</param>
/// <param name="data">返回数据</param>
/// <returns>是否获取成功</returns>
bool analysis_get_data(uint8_t* pData, Data_* data);


struct AnalysisData
{
    qint32 order;
    quint8* pData;
    qint64 ullLen;
};

//该类不会执行删除内存操作，需要析构该类后再删除指针
class Analysis{
public:
    Analysis(quint8* pData,qint64 ullLen);

    AnalysisData getNextData();

    Data_* getLastData();



private:
    quint8* m_pData;
    qint64 m_len;
    qint64 m_index;
    quint8* m_plastData;
    qint64 m_lastIndex;
};

#endif // ANALYSIS_H
