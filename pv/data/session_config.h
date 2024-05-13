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
    QList<SessionShowConfig> m_showList;//显示列表
    SessionShowConfig m_showConfig;//显示属性
    quint32 m_selectShowIndex;//当前显示索引
    qint64 m_maxUnit=1000000000;//最多拥有多少个ns，也就是显示的最大范围
    qint64 m_showStartUnit=0;//界面显示起始位置(ns)
    qint32 m_width=0;//绘制宽度
    bool m_isCustom=true;//是否最大显示
    bool m_isData=false;//是否有数据
    bool m_isShowIndex=false;//是否显示序号
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
#endif // SESSION_CONFIG_H
