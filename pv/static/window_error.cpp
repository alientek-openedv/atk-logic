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

#include "window_error.h"

WindowError::WindowError(QObject *parent) : QObject{parent} {}

const QString &WindowError::getError_msg() const
{
    return error_msg;
}

void WindowError::setError_msg(const QString &newError_msg)
{
    if (error_msg == newError_msg)
        return;
    if(!newError_msg.isEmpty())
        LogHelp::write(newError_msg);
    error_msg = newError_msg;
    isClose = false;
    emit error_msgChanged();
}

void WindowError::setError_msg(const QString &newError_msg, bool waitClose_)
{
    if (error_msg == newError_msg)
        return;
    if(!newError_msg.isEmpty())
        LogHelp::write(newError_msg);
    error_msg = newError_msg;
    isClose = false;
    emit error_msgChanged();
    if(waitClose_)
        waitClose();
}

void WindowError::waitClose()
{
    while(!isClose){
        QApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

bool WindowError::getIsClose() const
{
    return isClose;
}

void WindowError::setIsClose(bool newIsClose)
{
    if (isClose == newIsClose)
        return;
    isClose = newIsClose;
    emit isCloseChanged();
}
