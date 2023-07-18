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

#ifndef ANALYSIS_H
#define ANALYSIS_H


#pragma once

#include "pv/usb/usb_control.h"
#include "pv/static/log_help.h"

struct Data_;
class USBControl;

int analysis_check_data_header(quint8* pData);

bool analysis_check_data_bottom(quint8* pData,qint64 llDataLength);

bool analysis_get_data(uint8_t* pData, Data_* data);


struct AnalysisData
{
    qint32 order;
    quint8* pData;
    qint64 ullLen;
};

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
