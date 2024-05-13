#include "session_config.h"

SessionConfig::SessionConfig()
{
    appendConfig("s");
    appendConfig("ms");
    appendConfig("μs");
    appendConfig("ns");
    m_selectShowIndex=30;
    m_showConfig=m_showList[m_selectShowIndex];
}

SessionConfig& SessionConfig::operator=(const SessionConfig &config)
{
    m_showConfig.m_pixelUnit=config.m_showConfig.m_pixelUnit;
    m_selectShowIndex=config.m_selectShowIndex;
    m_maxUnit=config.m_maxUnit;
    m_showStartUnit=config.m_showStartUnit;
    return *this;
}

void SessionConfig::appendConfig(QString showUint)
{
    int loop[]={48,68,98,135,193,273,384};
    if(showUint=="s"){
        int loop2[]={100,10,0,-10};
        double lastPixelUnit=0;
        for(quint32 i=0;i<sizeof(loop2)/sizeof(loop2[0]);i++)
        {
            for(quint32 ii=0;ii<sizeof(loop)/sizeof(loop[0]);ii++)
            {
                SessionShowConfig config;
                config=SessionShowConfig(loop[ii],showUint,loop2[i]);
                if(config.m_pixelUnit>0&&lastPixelUnit!=config.m_pixelUnit)
                {
                    lastPixelUnit=config.m_pixelUnit;
                    m_showList.append(config);
                }
            }
        }
    }else{
        int loop2[]={10,0,-10};
        double lastPixelUnit=0;
        for(quint32 i=0;i<sizeof(loop2)/sizeof(loop2[0]);i++)
        {
            for(quint32 ii=0;ii<sizeof(loop)/sizeof(loop[0]);ii++)
            {
                SessionShowConfig config;
                if(showUint!="ns")
                    config=SessionShowConfig(loop[ii],showUint,loop2[i]);
                else if(i==0 || (i==1&&ii<2))
                    config=SessionShowConfig(loop[ii],showUint,loop2[i]);
                if(config.m_pixelUnit>0&&lastPixelUnit!=config.m_pixelUnit)
                {
                    lastPixelUnit=config.m_pixelUnit;
                    m_showList.append(config);
                }
            }
        }
    }

}
