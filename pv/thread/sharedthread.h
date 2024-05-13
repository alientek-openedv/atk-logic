#ifndef SHAREDTHREAD_H
#define SHAREDTHREAD_H

#include <QObject>
#include <QThread>
#include <pv/utils/qtlockedfile/qtlockedfile.h>

class SharedThread : public QObject
{
    Q_OBJECT
public:
    explicit SharedThread(QObject *parent = nullptr);
    ~SharedThread();
    void init(QString path);

    void lock();
    void unlock();

    bool m_isLock=false;

private:
    QThread m_thread;
    QtLockedFile m_lockFile;
};

#endif // SHAREDTHREAD_H
