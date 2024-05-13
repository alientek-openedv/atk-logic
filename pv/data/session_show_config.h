#ifndef SESSION_SHOW_CONFIG_H
#define SESSION_SHOW_CONFIG_H

#include <QString>
#include "../static/util.h"

class SessionShowConfig
{
public:
    SessionShowConfig(double scalePixel,QString showUint,qint32 multiply=0);
    SessionShowConfig();

public:
    double m_scalePixel;//每个刻度占像素点数量
    double m_unitMultiply;//显示单位与ns倍率
    qint64 m_multiply;//显示单位实际增减倍率
    double m_pixelUnit;//每个像素点的进制，指每个像素点拥有多少个标准单位
    QString m_showUint;//显示单位
};

#endif // SESSION_SHOW_CONFIG_H
