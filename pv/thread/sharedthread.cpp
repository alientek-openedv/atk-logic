#include "sharedthread.h"

SharedThread::SharedThread(QObject *parent)
    : QObject{parent}
{
    m_thread.start();
    this->moveToThread(&m_thread);
}

SharedThread::~SharedThread()
{
    unlock();
}

void SharedThread::init(QString path)
{
    m_lockFile.setFileName(path+"/lock");
}

void SharedThread::lock()
{
    if(m_lockFile.open(QIODevice::WriteOnly)){
        if(m_lockFile.lock(QtLockedFile::LockMode::WriteLock),true){
            m_isLock=true;
        }else
            m_lockFile.close();
    }
}

void SharedThread::unlock()
{
    if(m_isLock){
        m_lockFile.unlock();
        m_lockFile.close();
    }
}
