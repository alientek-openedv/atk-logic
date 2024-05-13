#include "usb_hotplug.h"

USBHotplug::USBHotplug(QObject *parent)
    : QObject{parent}
{
    m_thread.start();
    this->moveToThread(&m_thread);
    m_dataService=DataService::getInstance();
}

USBHotplug::~USBHotplug()
{
    if(m_run)
        m_run=false;
    m_thread.requestInterruption();
    m_thread.quit();
    m_thread.wait();
    disconnect();
}

void USBHotplug::startHotplug()
{
    LogHelp::write(QString("注册USB Hotplug"));
    m_run=true;
    libusb_context *context;
    int ret = libusb_init(&context);
    if (ret != 0) {
        m_dataService->getWindowError()->setError_msg(QObject::tr("初始化USB监听失败。"),true);
        return;
    }

    if (!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
        m_dataService->getWindowError()->setError_msg(QObject::tr("该系统不支持该USB监听功能。"),true);
        libusb_exit(context);
        return;
    }

    libusb_hotplug_callback_handle hp[2];
    ret = libusb_hotplug_register_callback(context, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, LIBUSB_HOTPLUG_NO_FLAGS, VENDOR_ID,
                                           PRODUCT_ID, LIBUSB_HOTPLUG_MATCH_ANY, hotplug_callback, this, &hp[0]);
    if (LIBUSB_SUCCESS != ret) {
        m_dataService->getWindowError()->setError_msg(QObject::tr("注册USB接入监听回调函数失败，错误代码:%1，错误文本:%2").arg(QString::number(ret),libusb_error_name(ret)),true);
        libusb_exit(context);
        return;
    }

    ret = libusb_hotplug_register_callback(context, LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, LIBUSB_HOTPLUG_NO_FLAGS, VENDOR_ID,
                                           PRODUCT_ID, LIBUSB_HOTPLUG_MATCH_ANY, hotplug_callback, this, &hp[1]);
    if (LIBUSB_SUCCESS != ret) {
        m_dataService->getWindowError()->setError_msg(QObject::tr("注册USB断开监听回调函数失败，错误代码:%1，错误文本:%2").arg(QString::number(ret),libusb_error_name(ret)),true);
        libusb_exit(context);
        return;
    }

    while (m_run) {
        timeval tv = {1, 0};
        ret = libusb_handle_events_timeout(context, &tv);
        if (ret < 0)
            LogHelp::write(QString("usb监听event事件失败。"));
    }

    libusb_exit(context);
}


int LIBUSB_CALL hotplug_callback(libusb_context* ctx, libusb_device* dev, libusb_hotplug_event event, void* user_data)
{
    Q_UNUSED(ctx);
    USBHotplug* root=(USBHotplug*)user_data;
    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED)
        emit root->DevicePlugIN();
    else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT)
        emit root->DevicePlugOUT();
    return 0;
}
