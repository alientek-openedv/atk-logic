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

#include "shortcut_listener.h"
#include "qdebug.h"


ShortcutListener::ShortcutListener(QObject *parent)
    : QObject{parent}
{

}

bool ShortcutListener::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);
    switch (event->type()) {
    case QEvent::FocusOut:
        emit focusOut();
        break;
    case QEvent::KeyPress:
        emit keyPress(static_cast<QKeyEvent*>(event)->key());
        break;
    case QEvent::KeyRelease:
        emit keyRelease(static_cast<QKeyEvent*>(event)->key());
        break;
    }
    return false;
}

void ShortcutListener::init(QObject *object)
{
    if (!object)
        return;
    m_parent=object;
}

bool ShortcutListener::Enable() const
{
    return m_Enable;
}

void ShortcutListener::setEnable(bool newEnable)
{
    if (m_Enable == newEnable)
        return;
    m_Enable = newEnable;
    if(m_Enable)
        m_parent->installEventFilter(this);
    else
        m_parent->removeEventFilter(this);
}
