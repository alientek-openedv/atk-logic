#ifndef THREAD_READ_H
#define THREAD_READ_H

class ThreadWork;
#include <QObject>
#include "thread_work.h"

class ThreadRead : public QObject
{
    Q_OBJECT
public:
    explicit ThreadRead(QObject *parent = nullptr);
    ~ThreadRead();
    void stop();
    void start(USBControl* usb, qint32 maxTransferSize, ThreadWork* parent);
    bool isRun();

private:
    QThread m_thread;
    ThreadWork* m_parent;
    bool m_run=false;
    bool m_runState=false;
};

#endif // THREAD_READ_H
