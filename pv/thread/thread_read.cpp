#include "thread_read.h"

ThreadRead::ThreadRead(QObject *parent)
    : QObject{parent}
{
    m_thread.start();
    this->moveToThread(&m_thread);
}

ThreadRead::~ThreadRead()
{
    stop();
    m_thread.requestInterruption();
    m_thread.quit();
    m_thread.wait();
    disconnect();
}

void ThreadRead::stop()
{
    m_run=false;
}

void ThreadRead::start(USBControl *usb, qint32 maxTransferSize, ThreadWork* parent)
{
    m_runState=true;
    m_parent=parent;
    m_run=true;
    while(m_run){
        Data_* data = new Data_();
        if(usb->Read(data, 1, 1200, maxTransferSize))
            m_parent->onDataIn(data);
        else{
            if (data->buf != nullptr)
                delete[] data->buf;
            delete data;
            if(usb->m_NoDevice)
                m_run=false;
        }
        if(usb->m_ThreadState==2)
            break;
    }
    m_runState=false;
}

bool ThreadRead::isRun()
{
    return m_runState;
}
