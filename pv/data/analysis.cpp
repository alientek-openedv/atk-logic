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

#include "analysis.h"

int analysis_check_data_header(quint8* pData){
    if (*pData == 0x0a) 
    {
        char c = *(pData + 1);
        if (c > 0 && c < 7)
            return c;
    }
    return -1;
}

bool analysis_check_data_bottom(quint8* pData,qint64 llDataLength) {

    quint16 len=*((quint16*)(pData + 2)) + 4;
    if(len<llDataLength){
        if (*(pData + len) == 0x00 && *(pData + len + 1) == 0x0b)
            return true;
    }
    return false;
}

bool analysis_get_data(quint8* pData, Data_* data) {
    pData += 2;
    quint16 len = *((quint16*)pData);
    pData += 2;
    if(len<data->len&&len>0){
        data->buf = pData;
        data->len = len;
        return true;
    }
    return false;
}

Analysis::Analysis(quint8 *pData, qint64 ullLen)
    : m_pData(pData),m_plastData(pData),m_len(ullLen),m_index(0),m_lastIndex(0)
{}

AnalysisData Analysis::getNextData()
{
    AnalysisData ret;
    while (m_index < m_len) {
        ret.order = analysis_check_data_header(m_pData);
        if(ret.order!=-1)
        {
            if(analysis_check_data_bottom(m_pData, m_len-m_index)){
                Data_* data2 = new Data_();
                data2->len = m_len-m_index-1;
                if (analysis_get_data(m_pData, data2))
                {
                    ret.pData=data2->buf;
                    ret.ullLen=data2->len;
                    m_index += ret.ullLen + 5;
                    m_pData += ret.ullLen + 5;
                    m_plastData=m_pData+1;
                    m_lastIndex=m_index+1;
                    delete data2;
                    return ret;
                }else
                    LogHelp::write("analysis_get_data:error");
                delete data2;
            }
        }
        m_index++;
        m_pData++;
    }
    ret.order=-1;
    return ret;
}

Data_* Analysis::getLastData()
{
    if(m_lastIndex<m_len){
        Data_* data2 = new Data_();
        data2->len=m_len-m_lastIndex;
        if(data2->len>0)
        {
            data2->buf=new quint8[data2->len];
            memcpy(data2->buf,m_plastData,data2->len);
        }else
        {
            delete data2;
            data2=nullptr;
        }
        return data2;
    }else
        return nullptr;
}
