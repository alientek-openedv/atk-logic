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

#include "session_error.h"

SessionError::SessionError(QObject *parent)
    : QObject{parent}, isClose(false) {}

const QString &SessionError::getError_msg() const
{
    return error_msg;
}

void SessionError::setError_msg(const QString &newError_msg)
{
    if (error_msg == newError_msg)
        return;
    if(!newError_msg.isEmpty())
        LogHelp::write(newError_msg);
    error_msg = newError_msg;
    isClose = false;
    emit error_msgChanged();
}

void SessionError::setError_msg(const QString &newError_msg, bool waitClose_=false)
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

void SessionError::waitClose()
{
    while(!isClose){
        QApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

bool SessionError::getIsClose() const
{
    return isClose;
}

void SessionError::setIsClose(bool newIsClose)
{
    if (isClose == newIsClose)
        return;
    isClose = newIsClose;
    emit isCloseChanged();
}
