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
    double m_scalePixel;
    double m_unitMultiply;
    qint64 m_multiply;
    double m_pixelUnit;
    QString m_showUint;
};

#endif 
