#include "usb_base.h"
#include "qdebug.h"

int logic_analyzer_init(LogicAnalyzer* m)
{
    libusb_set_debug(m->context, LIBUSB_LOG_LEVEL_WARNING);
    int ret=libusb_init(&m->context);
    if (ret != LIBUSB_SUCCESS)
        return ret;
    else
        m->flag.init = true;
    return ret;
}

void logic_analyzer_deinit(LogicAnalyzer* m)
{
    if (!m->flag.init)
        return;
    m->flag.exit = true;
    logic_analyzer_close(m);
    libusb_unref_device(m->dev);
    libusb_exit(m->context);
}

QList<FindData> logic_analyzer_find() {
    QList<FindData> find_data;
    libusb_device** devs;
    libusb_context* ctx = NULL;
    if(libusb_init(&ctx) != 0)
        return find_data;

    qint32 cnt = libusb_get_device_list(ctx, &devs);
    if (cnt < 0)
    {
        libusb_exit(ctx);
        return find_data;
    }

    for (int i = 0; i < cnt; i++) {
        libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(devs[i], &desc);
        if (r < 0)
            continue;
        if (desc.idVendor == VENDOR_ID && desc.idProduct == PRODUCT_ID) {
            FindData temp;
            temp.dev = devs[i];
            temp.context = ctx;
            temp.port = libusb_get_port_number(devs[i]);
            libusb_ref_device(devs[i]);
            find_data.push_back(temp);
        }
    }
    libusb_free_device_list(devs, 1);
    return find_data;
}

int logic_analyzer_open(LogicAnalyzer* m)
{
    int ret=0;
    if (m->device_handle == NULL) {
        ret=libusb_open(m->dev, &m->device_handle);
        if (ret != LIBUSB_SUCCESS)
            return ret;
        if (m->device_handle == NULL)
            return -1;
        libusb_set_auto_detach_kernel_driver(m->device_handle, 1);
        ret=libusb_claim_interface(m->device_handle, 0);
        if (ret != LIBUSB_SUCCESS)
            logic_analyzer_close(m);
        return ret;
    }
    return ret;
}

bool logic_analyzer_reopen(LogicAnalyzer *m)
{
    if(m->device_handle == NULL)
        return logic_analyzer_open(m);

    //关闭设备，重新打开
    logic_analyzer_close(m);

    return logic_analyzer_open(m);
}

void logic_analyzer_close(LogicAnalyzer* m)
{
    if (m->device_handle != nullptr) {
        libusb_release_interface(m->device_handle,0);
        libusb_close(m->device_handle);
        m->device_handle = nullptr;
    }
}


void logic_analyzer_convert_to_pc(const void* s, void* d, unsigned len)
{
    const quint8* src = (const quint8*)s;
    uint16_t* dst = (uint16_t*)d;
    const uint16_t* src0 = (const uint16_t*)(&src[0]);
    const uint16_t* src1 = (const uint16_t*)(&src[512]);
    const uint16_t* src2 = (const uint16_t*)(&src[512 * 2]);
    const uint16_t* src3 = (const uint16_t*)(&src[512 * 3]);
    for (unsigned i = 0; i < len; i += 2048) {
        for (unsigned j = 0, k = 0; j < 256; j++, k += 4) {
            dst[k] = src0[j];
            dst[k + 1] = src1[j];
            dst[k + 2] = src2[j];
            dst[k + 3] = src3[j];
        }
        dst += 1024;
        src0 += 1024;
        src1 += 1024;
        src2 += 1024;
        src3 += 1024;
    }
}

void logic_analyzer_convert_to_device(const void* s, void* d, unsigned len)
{
    const uint16_t* src = (const uint16_t*)s;
    quint8* dst = (quint8*)d;
    uint16_t* dst0 = (uint16_t*)(&dst[0]);
    uint16_t* dst1 = (uint16_t*)(&dst[512]);
    uint16_t* dst2 = (uint16_t*)(&dst[512 * 2]);
    uint16_t* dst3 = (uint16_t*)(&dst[512 * 3]);
    for (unsigned i = 0; i < len; i += 2048) {
        for (unsigned j = 0, k = 0; j < 256; j++, k += 4) {
            dst0[j] = src[k];
            dst1[j] = src[k + 1];
            dst2[j] = src[k + 2];
            dst3[j] = src[k + 3];
        }
        src += 1024;
        dst0 += 1024;
        dst1 += 1024;
        dst2 += 1024;
        dst3 += 1024;
    }
}

usb_loop_thread::usb_loop_thread(LogicAnalyzer* m) : m_logicAnalyzer(m){}

void usb_loop_thread::run()
{
    if (!m_logicAnalyzer->flag.init)
        return;
    while (m_logicAnalyzer->flag.exit==0)
    {
        timeval tv;
        tv.tv_sec = tv.tv_usec = 1;
        libusb_handle_events_timeout_completed(m_logicAnalyzer->context, &tv, NULL);
    }
}
