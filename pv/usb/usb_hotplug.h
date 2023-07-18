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

#ifndef USB_HOTPLUG_H
#define USB_HOTPLUG_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include "libusb.h"
#include <pv/static/log_help.h>
#include "pv/static/data_service.h"

int LIBUSB_CALL hotplug_callback(libusb_context* ctx, libusb_device* dev, libusb_hotplug_event event, void* user_data);

class USBHotplug : public QObject
{
    Q_OBJECT
public:
    explicit USBHotplug(QObject *parent = nullptr);
    ~USBHotplug();

    void startHotplug();

signals:
    void DevicePlugIN();
    void DevicePlugOUT();

private:
    QThread m_thread;
    DataService* m_dataService;
    bool m_run=false;
};

#endif // USB_HOTPLUG_H
