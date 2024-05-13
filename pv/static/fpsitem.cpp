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
