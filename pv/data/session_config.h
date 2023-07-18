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

#ifndef SESSION_CONFIG_H
#define SESSION_CONFIG_H

#include "qlist.h"
#include "session_show_config.h"
#include "qdebug.h"

class SessionConfig
{
public:
    SessionConfig();
    SessionConfig& operator=(const SessionConfig &config);
    friend bool operator!=(const SessionConfig &st, const SessionConfig &st2);

private:
    void appendConfig(QString showUint);

public:
    QList<SessionShowConfig> m_showList;
    SessionShowConfig m_showConfig;
    quint32 m_selectShowIndex;
    qint64 m_maxUnit=1000000000;
    qint64 m_showStartUnit=0;
    qint32 m_width=0;
    bool m_isCustom=true;
    bool m_isData=false;
    bool m_isShowIndex=false;
};

bool inline operator!=(const SessionConfig &st, const SessionConfig &st2)
{
    bool ret=false;
    if(st.m_showConfig.m_pixelUnit!=st2.m_showConfig.m_pixelUnit)
        ret=true;
    else if(st.m_selectShowIndex!=st2.m_selectShowIndex)
        ret=true;
    else if(st.m_maxUnit!=st2.m_maxUnit)
        ret=true;
    else if(st.m_showStartUnit!=st2.m_showStartUnit)
        ret=true;
    return ret;
}
#endif 
