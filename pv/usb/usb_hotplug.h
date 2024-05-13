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
