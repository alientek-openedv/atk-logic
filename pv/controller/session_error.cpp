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
