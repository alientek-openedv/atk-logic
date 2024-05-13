#include "analysis.h"

int analysis_check_data_header(quint8* pData){
    if (*pData == 0x0a) //包头检测
    {
        char c = *(pData + 1);
        if (c > 0 && c < 7)//判定指令范围
            return c;
    }
    return -1;
}

bool analysis_check_data_bottom(quint8* pData,qint64 llDataLength) {

    quint16 len=*((quint16*)(pData + 2)) + 4;
    if(len<llDataLength){
        if (*(pData + len) == 0x00 && *(pData + len + 1) == 0x0b)//包尾检测
            return true;
    }
    return false;
}

bool analysis_get_data(quint8* pData, Data_* data) {
    pData += 2;
    quint16 len = *((quint16*)pData);
    pData += 2;
    //判定获取的长度不得超出数据长度
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
    //循环指令所有数据
    while (m_index < m_len) {
        //判断是否协议头并且获取指令代码
        ret.order = analysis_check_data_header(m_pData);
        if(ret.order!=-1)
        {
            if(analysis_check_data_bottom(m_pData, m_len-m_index)){
                //获取数据
                Data_* data2 = new Data_();
                data2->len = m_len-m_index-1;
                if (analysis_get_data(m_pData, data2))
                {
                    ret.pData=data2->buf;
                    ret.ullLen=data2->len;

                    //跳过已经获取的数据
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
