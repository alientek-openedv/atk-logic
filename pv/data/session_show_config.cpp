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

#include "session_show_config.h"
#include "qdebug.h"

SessionShowConfig::SessionShowConfig(double scalePixel,QString showUint,qint32 multiply)
    : m_showUint(showUint),m_multiply(multiply)
{
    if(showUint=="μs")
        m_unitMultiply=1000;
    else if(showUint=="ms")
        m_unitMultiply=1000000;
    else if(showUint=="s")
        m_unitMultiply=1000000000;
    else
        m_unitMultiply=1;
    if(multiply>0)
        m_unitMultiply*=multiply;
    else if(multiply<0)
        m_unitMultiply/=qAbs(multiply);
    m_pixelUnit=m_unitMultiply/scalePixel;
    if(m_pixelUnit>1){
        m_pixelUnit=qRound64(m_pixelUnit);
        m_scalePixel=m_unitMultiply/m_pixelUnit;
    }else{
        m_pixelUnit=QString::number(m_pixelUnit, 'f', 2).toDouble();
        m_scalePixel=m_unitMultiply/m_pixelUnit;
    }
}

SessionShowConfig::SessionShowConfig(){}
