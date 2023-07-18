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
