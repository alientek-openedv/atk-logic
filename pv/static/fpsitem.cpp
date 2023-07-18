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

#include "fpsitem.h"

FpsItem::FpsItem(QQuickItem *parent)
: QQuickItem(parent)
{
    m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, [this]{ m_fps = m_frameCount; m_frameCount = 0; emit fpsChanged(); });
        connect(this, &QQuickItem::windowChanged, this, [this]{
            if (window())
                connect(window(), &QQuickWindow::afterRendering, this
                        , [this]{ m_frameCount++; }, Qt::DirectConnection);
        });
        m_timer->start(1000);
}

int FpsItem::fps() const
{
    return m_fps;
}
