#include "usb_control.h"
#include <QFile>

#define TO_MCU_EP    0x02
#define TO_PC_EP    0x81

USBControl::USBControl()
{
    _m_logic=new LogicAnalyzer();
}

USBControl::~USBControl() {
    if(_m_logic!=nullptr){
        _m_logic->flag.init = 0;
        _m_logic->flag.exit = 1;
        if(_m_logic->usb_loop!=nullptr)
        {
            _m_logic->usb_loop->terminate();
            _m_logic->usb_loop->wait();
            delete _m_logic->usb_loop;
            _m_logic->usb_loop=nullptr;
        }

        delete _m_logic;
        _m_logic=nullptr;
    }
}

bool USBControl::IsInit() {
    return _m_isInit;
}

QList<FindData> USBControl::Find() {
    return logic_analyzer_find();
}

void  USBControl::Find_Free(libusb_device* dev) {
    libusb_unref_device(dev);
}

bool USBControl::Init(libusb_device* dev, libusb_context* context, qint32 channelCount, int port) {
    if (_m_isInit)
        return true;
    DeInit();
    m_ChannelCount=channelCount;
    //初始化通讯
    _m_logic->port = port;
    _m_logic->dev = dev;
    _m_logic->context = context;
    _m_logic->flag.exit=0;
    _m_logic->to_mcu_ep = TO_MCU_EP;
    _m_logic->to_pc_ep = TO_PC_EP;
    if(_m_logic->context==nullptr){
        m_lastError=logic_analyzer_init(_m_logic);
        if (m_lastError!=LIBUSB_SUCCESS)
            goto Fail;
    }
    m_lastError=logic_analyzer_open(_m_logic);
    if (m_lastError==LIBUSB_SUCCESS)
    {
        _m_logic->usb_loop=new usb_loop_thread(_m_logic);
        _m_logic->usb_loop->start(QThread::HighestPriority);
    }else
        goto Fail;

    _m_logic->flag.init=1;
    //初始化写入结构体
    struct libusb_transfer* p;
    for (int i = 0; i < WRITE_TRANSFER_LENGTH; i++) {
        p = libusb_alloc_transfer(0);
        if (p == NULL)
            goto Fail;
        _m_dataToDevice.transfer[i] = p;
    }

    //初始化读取结构体
    _m_dataToPC.singleBuffer=new quint8[2048];
    _m_dataToPC.len = READ_TRANSFER_BUFFER_SIZE;//32K缓存
    for (int i = 0; i < READ_TRANSFER_LENGTH; i++) {
        p = libusb_alloc_transfer(0);
        if (p == NULL)
            goto Fail;
        _m_dataToPC.transfer[i] = p;
        _m_dataToPC.buf[i] = new quint8[_m_dataToPC.len];
    }
    m_port=port;
    _m_isInit = true;
    return true;
Fail:
    DeInit();
    return false;
}

void USBControl::DeInit() {
    if(_m_logic!=nullptr){
        _m_logic->flag.init = 0;
        _m_logic->flag.exit = 1;
        if(_m_logic->usb_loop!=nullptr)
        {
            _m_logic->usb_loop->wait();
            delete _m_logic->usb_loop;
            _m_logic->usb_loop=nullptr;
        }
    }
}

static void LIBUSB_CALL transfer_write_cb(struct libusb_transfer *transfer)
{
    DataToDevice* m = (DataToDevice*)transfer->user_data;
    if (transfer->status != LIBUSB_TRANSFER_COMPLETED) {
        m->flag.error = transfer->status;
        qDebug("TestToDevice transfer error :%d\r\n", transfer->status);
        return;
    }
    if (transfer->length != transfer->actual_length) {
        qDebug("TestToDevice transfer len :%u\r\n", transfer->actual_length);
        return;
    }
}

bool USBControl::Write(quint8* data, qint32 len) {
    if (!_m_isInit||m_NoDevice)
        return false;
    int offset=8;//0x00偏移
    len += 7 + offset;//加上指令头、尾、CRC32 7字节
    quint8* pBuffer = new quint8[len];
    memset(pBuffer, 0, len);
    len -= 7 + offset;//计算完成后还原
    quint32 CRC32 = gCRC32(data, len);
    memset(pBuffer + offset, 0x0A, 1);//指令头
    memcpy(pBuffer + offset + 1, data, len);//数据
    memset(pBuffer + offset + 1 + len, 0x0B, 1);//指令尾
    memcpy(pBuffer + offset + 2 + len, &CRC32, sizeof(CRC32));//CRC32
//    showHex(pBuffer, len + offset + 7);
    bool ret=SendToDevice(pBuffer, len + offset + 7);
    delete[] pBuffer;
    return ret;
}

static void LIBUSB_CALL transfer_read_cb(struct libusb_transfer* transfer)
{
    DataToPC* m = (DataToPC*)transfer->user_data;
    //如果数据填不满缓存区，还是会报超时，这里判定还是有数据
    //    if(transfer->actual_length>0)
    //        qDebug()<<"transfer->actual_length:"<<transfer->actual_length;
    if(transfer->status == LIBUSB_TRANSFER_TIMED_OUT && transfer->actual_length >0 && transfer->length>0)
        goto transferPush;
    if (transfer->status != LIBUSB_TRANSFER_COMPLETED && transfer->status != LIBUSB_TRANSFER_TIMED_OUT) {
        qDebug()<<"read_error:"<<transfer->status;
        m->flag.error=transfer->status;
        m->flag.count++;
        return;
    }
    if(transfer->length>0 && transfer->actual_length >0)
        goto transferPush;
    m->flag.count++;
    return;
transferPush:
    m->flag.dataCount++;
    DataToPC_OK ok;
    ok.transfer=transfer;
    ok.length=transfer->actual_length;
    if(ok.length%2048!=0)
        ok.length=ok.length/2048*2048;
    ok.buffer=new quint8[ok.length];
    logic_analyzer_convert_to_pc(transfer->buffer, ok.buffer, ok.length);
    m->mutex.lock();
    m->ok.push_back(ok);
    m->mutex.unlock();
    m->flag.count++;
}

QString USBControl::ReadStr() {
    QString ret;
    Data_* data = new Data_();
    if (Read(data))
        ret=QString::fromLocal8Bit((char*)data->buf, data->len);
    if(data->buf!=nullptr)
        delete[] data->buf;
    delete data;
    return ret;
}

bool USBControl::Read(Data_* pData, qint32 iCount,qint32 timeOut, qint32 maxTransferSize) {
    pData->buf=nullptr;
    pData->len=0;
    if (!_m_isInit||m_NoDevice)
        return false;
    _m_dataToPC.flag.error = 0;
    _m_dataToPC.flag.count = 0;
    _m_dataToPC.flag.dataCount = 0;
    _m_dataToPC.ok.clear();
    if(maxTransferSize<1)
        maxTransferSize=1;
    else if(maxTransferSize>READ_TRANSFER_LENGTH)
        maxTransferSize=READ_TRANSFER_LENGTH;
    iCount*=maxTransferSize;
    int sendCount_ = 0,acceptCount_=0;
    try {
        for (int i = 0; i < maxTransferSize; i++) {
            memset(_m_dataToPC.buf[i], 0, _m_dataToPC.len);
            libusb_fill_bulk_transfer(_m_dataToPC.transfer[i],
                                      _m_logic->device_handle,
                                      _m_logic->to_pc_ep,
                                      _m_dataToPC.buf[i],
                                      _m_dataToPC.len,
                                      transfer_read_cb,
                                      &_m_dataToPC,
                                      timeOut);
            sendCount_++;
            const int res = libusb_submit_transfer(_m_dataToPC.transfer[i]);
            if (res != LIBUSB_SUCCESS) {
                LogHelp::write("读取USB数据失败！错误代码："+QString::number(res));
                throw "error";
                //qDebug("TestToPC submit error:%d\r\n", res);
            }
        }
    } catch (std::exception e) {
        _m_dataToPC.flag.error = true;
        qDebug()<<"读取USB数据失败！！！错误原因："<<e.what();
        return false;
    } catch(...){
        _m_dataToPC.flag.error = true;
        qDebug()<<"读取USB数据失败！！！";
        return false;
    }
    QList<DataToPC_OK> lsBufferOK;
    quint64 bufferlen=0;
    do {
        if(_m_dataToPC.flag.error||m_NoDevice)
            break;
        _m_dataToPC.mutex.lock();
        if (_m_dataToPC.ok.size() == 0)
        {
            _m_dataToPC.mutex.unlock();
            continue;
        }
        acceptCount_++;
        DataToPC_OK ok=_m_dataToPC.ok.front();
        _m_dataToPC.ok.erase(_m_dataToPC.ok.begin());
        _m_dataToPC.mutex.unlock();
        if(ok.length>0){
            lsBufferOK.push_back(ok);
            bufferlen+=ok.length;
        }

        //        QString s;
        //        s=s.asprintf("%#lx", ok.transfer);
        //        if(s=="0xbaadf00d" || s.toLongLong(nullptr,16)<10000)
        //        {
        //            qDebug()<<"continue:"<<s;
        //            errorCount++;
        //            if(errorCount==4)
        //            {
        //                qDebug()<<"error_continue_break";
        //                break;
        //            }
        //            continue;
        //        }
        if(sendCount_<iCount||(iCount<=0&&sendCount_<200))
        {
            if(ok.length<=0)
                break;
            sendCount_++;
            try{
                const int res = libusb_submit_transfer(ok.transfer);
                if (res != LIBUSB_SUCCESS)
                {
                    LogHelp::write("读取USB数据失败！！错误代码："+QString::number(res));
                    throw "error";
                }
            } catch (std::exception e) {
                _m_dataToPC.flag.error = true;
                qDebug()<<"读取失败！！！错误原因："<<e.what();
                break;
            } catch(...){
                _m_dataToPC.flag.error = true;
                qDebug()<<"读取失败！！！";
                break;
            }
        }
    } while (_m_dataToPC.flag.count != sendCount_ || (_m_dataToPC.flag.count == sendCount_ && acceptCount_ != _m_dataToPC.flag.dataCount));
    if (lsBufferOK.size()>0 && bufferlen>0) {
        pData->buf = new quint8[bufferlen];
        pData->len = 0;
        memset(pData->buf, 0, bufferlen);
        while (lsBufferOK.size() != 0)
        {
            DataToPC_OK data = lsBufferOK.front();
            lsBufferOK.erase(lsBufferOK.begin());
            if(data.length>0)
                memcpy(pData->buf + pData->len, data.buffer, data.length);
            pData->len += data.length;
            delete[] data.buffer;
        }
    }
    if(_m_dataToPC.flag.error)
    {
        if(_m_dataToPC.flag.error==LIBUSB_TRANSFER_NO_DEVICE)
            m_NoDevice=true;
        LogHelp::write("ReadError");
    }
    return pData->len != 0;
}

bool USBControl::ReadSynchronous(Data_ *pData, qint32 len)
{
    pData->len=len;
    pData->buf=new quint8[len];
    if (!_m_isInit||m_NoDevice)
        return false;
    int actual_length=0;
    int res=libusb_bulk_transfer(_m_logic->device_handle,
                                 _m_logic->to_pc_ep,
                                 pData->buf,
                                 pData->len,
                                 &actual_length,
                                 50);
    pData->len=actual_length;
    if(pData->len==0){
        delete[] pData->buf;
        pData->buf=nullptr;
    }
    if(pData->len>=2048){
        if(pData->len%2048!=0)
            pData->len=pData->len/2048*2048;
        quint8* tmp=new quint8[pData->len];
        logic_analyzer_convert_to_pc(pData->buf, tmp, pData->len);
        delete[] pData->buf;
        pData->buf=tmp;
    }
    return res == LIBUSB_SUCCESS || (res == LIBUSB_ERROR_TIMEOUT && pData->len!=0);
}

bool USBControl::ReadSingle(Data_ *pData)
{
    pData->buf=nullptr;
    pData->len=0;
    if (!_m_isInit||m_NoDevice)
        return false;
    _m_dataToPC.flag.error = 0;
    _m_dataToPC.flag.count = 0;
    _m_dataToPC.flag.dataCount = 0;
    _m_dataToPC.ok.clear();
    qint32 iCount=-1;
    int sendCount_ = 0,acceptCount_=0;
    try {
        libusb_fill_bulk_transfer(_m_dataToPC.transfer[0],
                _m_logic->device_handle,
                _m_logic->to_pc_ep,
                _m_dataToPC.singleBuffer,
                2048,
                transfer_read_cb,
                &_m_dataToPC,
                50);
        sendCount_++;
        const int res = libusb_submit_transfer(_m_dataToPC.transfer[0]);
        if (res != LIBUSB_SUCCESS) {
            LogHelp::write("读取USB数据失败！错误代码："+QString::number(res));
            throw "error";
            //qDebug("TestToPC submit error:%d\r\n", res);
        }
    } catch (...) {
        _m_dataToPC.flag.error = true;
        qDebug()<<"读取USB数据失败！！！";
        return false;
    }
    QList<DataToPC_OK> lsBufferOK;
    quint64 bufferlen=0;
    do {
        if(_m_dataToPC.flag.error||m_NoDevice)
            break;
        _m_dataToPC.mutex.lock();
        if (_m_dataToPC.ok.size() == 0)
        {
            _m_dataToPC.mutex.unlock();
            continue;
        }
        acceptCount_++;
        DataToPC_OK ok=_m_dataToPC.ok.front();
        _m_dataToPC.ok.erase(_m_dataToPC.ok.begin());
        _m_dataToPC.mutex.unlock();
        if(ok.length>0){
            lsBufferOK.push_back(ok);
            bufferlen+=ok.length;
        }
        if(sendCount_<iCount||iCount<=0)
        {
            if(ok.length<=0)
                break;
            sendCount_++;
            try{
                const int res = libusb_submit_transfer(ok.transfer);
                if (res != LIBUSB_SUCCESS)
                {
                    LogHelp::write("读取USB数据失败！！错误代码："+QString::number(res));
                    throw "error";
                }
            } catch (...) {
                _m_dataToPC.flag.error = true;
                qDebug()<<"读取失败！！！";
                break;
            }
        }
    } while (_m_dataToPC.flag.count != sendCount_ || (_m_dataToPC.flag.count == sendCount_ && acceptCount_ != _m_dataToPC.flag.dataCount));
    if (lsBufferOK.size()>0 && bufferlen>0) {
        pData->buf = new quint8[bufferlen];
        pData->len = 0;
        memset(pData->buf, 0, bufferlen);
        while (lsBufferOK.size() != 0)
        {
            DataToPC_OK data = lsBufferOK.front();
            lsBufferOK.erase(lsBufferOK.begin());
            if(data.length>0)
                memcpy(pData->buf + pData->len, data.buffer, data.length);
            pData->len += data.length;
            delete[] data.buffer;
        }
    }
    if(_m_dataToPC.flag.error)
    {
        if(_m_dataToPC.flag.error==LIBUSB_TRANSFER_NO_DEVICE)
            m_NoDevice=true;
        LogHelp::write("ReadError");
    }
    return pData->len != 0;
}

int USBControl::GetPort()
{
    return m_port;
}

bool USBControl::SetResetState(qint8 state)
{
    quint8 buf[512];
    buf[0]=0x0a;
    buf[1]=0x87;
    buf[2]=state;
    return SendToMCU(buf, LEN(buf));
}

bool USBControl::GetResetState()
{
    quint8 buf[512];
    buf[0]=0x0a;
    buf[1]=0x88;
    return SendToMCU(buf, LEN(buf));
}

bool USBControl::Write(quint8 code, quint8* data, qint32 len, bool isWaitReply) {
    len += 2;
    quint8* newCode = new quint8[len];
    memset(newCode, 0, len);
    *newCode = code;
    *(newCode + 1) = len-1;
    if(len - 2>0)
        memcpy(newCode + 2, data, len - 2);
    m_lock.lock();
    bool _init=Write(newCode, len);
    //qDebug()<<"sendcode:"<<code<<_init;
    if(isWaitReply)
    {
        //        while (true) {
        //            if(_init&&WaitReply(code))
        //                break;
        //            _init=Write(newCode, len);
        //            //qDebug()<<"sendcode:"<<code<<_init;
        //        }
    }
    delete[] newCode;
    m_lock.unlock();
    return _init;
}

bool USBControl::SendToMCU(quint8 *data, qint64 len)
{
    if (!_m_isInit||m_NoDevice)
        return false;
    if (_m_dataToDevice.buf[_m_dataToDevice.bufIndex] != nullptr)
        delete[] _m_dataToDevice.buf[_m_dataToDevice.bufIndex];
    _m_dataToDevice.buf[_m_dataToDevice.bufIndex] = new quint8[len];
    libusb_fill_bulk_transfer(_m_dataToDevice.transfer[_m_dataToDevice.bufIndex],
            _m_logic->device_handle,
            _m_logic->to_mcu_ep,
            _m_dataToDevice.buf[_m_dataToDevice.bufIndex],
            len,
            transfer_write_cb,
            &_m_dataToDevice,
            100);
    memcpy(_m_dataToDevice.buf[_m_dataToDevice.bufIndex],data,len);
    const int res = libusb_submit_transfer(_m_dataToDevice.transfer[_m_dataToDevice.bufIndex]);
    _m_dataToDevice.bufIndex++;
    if (_m_dataToDevice.bufIndex == WRITE_TRANSFER_LENGTH)
        _m_dataToDevice.bufIndex = 0;
    _m_dataToDevice.flag.error = res != LIBUSB_SUCCESS;
    return res == LIBUSB_SUCCESS;;
}

void printf_buf(const uint16_t *p, unsigned len)
{
    for(unsigned i = 0; i < len; i++)
        qDebug("%d,\r\n", p[i] & 0x01);
}

bool USBControl::SendToDevice(quint8 *data, qint64 len)
{
    if (!_m_isInit||m_NoDevice)
        return false;
    qint64 ullNewLen = len % 2048 != 0 ? (len / 2048 + 1) * 2048 : len;
    quint8* pBuffer = new quint8[ullNewLen];
    int res;
    try {
        if (_m_dataToDevice.buf[_m_dataToDevice.bufIndex] != nullptr)
            delete[] _m_dataToDevice.buf[_m_dataToDevice.bufIndex];
        _m_dataToDevice.buf[_m_dataToDevice.bufIndex] = new quint8[ullNewLen];
        memset(pBuffer, 0, ullNewLen);
        memcpy(pBuffer, data, len);//数据
        libusb_fill_bulk_transfer(_m_dataToDevice.transfer[_m_dataToDevice.bufIndex],
                _m_logic->device_handle,
                _m_logic->to_mcu_ep,
                _m_dataToDevice.buf[_m_dataToDevice.bufIndex],
                ullNewLen,
                transfer_write_cb,
                &_m_dataToDevice,
                100);
        //    printf_buf((uint16_t*)pBuffer,1024);
        logic_analyzer_convert_to_device(pBuffer, _m_dataToDevice.buf[_m_dataToDevice.bufIndex], ullNewLen);
        //    QFile file("a.txt");
        //    if(file.open(QIODevice::WriteOnly  |QIODevice::Append)){
        //        file.write("\r\n\r\n\r\naa[]={\r\n");
        //        for(int i =0; i < 2048; i++){
        //            if(i % 16 == 0)
        //                file.write("\r\n");
        //            file.write(QString().sprintf("0x%.2x,", _m_dataToDevice.buf[_m_dataToDevice.bufIndex][i]).toUtf8());
        //        }
        //        file.close();
        //    }
        res = libusb_submit_transfer(_m_dataToDevice.transfer[_m_dataToDevice.bufIndex]);
        _m_dataToDevice.bufIndex++;
        if (_m_dataToDevice.bufIndex == WRITE_TRANSFER_LENGTH)
            _m_dataToDevice.bufIndex = 0;
        _m_dataToDevice.flag.error = res != LIBUSB_SUCCESS;
    } catch (...) {
        _m_dataToDevice.flag.error = true;
        res=LIBUSB_ERROR_IO;
        qDebug()<<"发送异常！！";
    }
    delete[] pBuffer;
    return res == LIBUSB_SUCCESS;
}

bool USBControl::WaitHardwareUpdate(quint8 code)
{
    Data_ pData;
    int count=0;
    while(true)
    {
        if(ReadSynchronous(&pData)&&pData.len>0)
        {
            if(pData.buf[0]==0x0a && pData.buf[1]==code &&pData.buf[2]==0x01)
            {
                delete[] pData.buf;
                return true;
            }
            return false;
        }
        delete[] pData.buf;
        QThread::msleep(50);
        count++;
        if(count>5)
            break;
    }
    return false;
}

//bool USBControl::WaitReply(quint8 code)
//{
//    qint32 count=0;
//    qint32 sleepNum=50;
//    while (true) {
//        if(m_lastData!=nullptr)
//        {
//            if(m_lastData->buf)
//                delete[] m_lastData->buf;
//            memset(m_lastData,0,sizeof(Data_));
//        }else
//            m_lastData=new Data_();

//        if(Read(m_lastData,1))
//        {
//            //qDebug()<<m_lastData->len<<m_lastData->buf;
//            if(m_lastData->len>0&&m_lastData->buf){
//                Analysis analusis(m_lastData->buf,m_lastData->len);
//                AnalysisData data=analusis.getNextData();
//                while(data.order!=-1){
//                    if(data.order==4){
//                        //qDebug()<<"waitcode:"<<code<<"ok";
//                        if(*(data.pData+2)==code)
//                        {
//                            delete data.pData;
//                            return true;
//                        }
//                    }
//                    delete data.pData;
//                    data=analusis.getNextData();
//                }
//            }
//        }
//        QThread::msleep(50);
//        count+=sleepNum;
//        if(count>=200)
//            break;
//    }
//    return false;
//}

bool USBControl::GetDeviceData(bool isWaitReply) {
    return Write(0x10, NULL, 0, isWaitReply);
}

bool USBControl::GetMCUVersion()
{
    quint8 buf[512];
    buf[0]=0x0a;
    buf[1]=0x81;
    return SendToMCU(buf, LEN(buf));
}

bool USBControl::EnterBootloader()
{
    quint8 buf[512];
    buf[0]=0x0a;
    buf[1]=0x80;
    memcpy(&buf[2], BOOTLOADER_ENTER_KEY, sizeof(BOOTLOADER_ENTER_KEY));
    return SendToMCU(buf, LEN(buf));
}

bool USBControl::EnterHardwareUpdate(bool isMCU)
{
    quint8 buf[512];
    buf[0]=0x0a;
    if(isMCU){
        buf[1]=0x82;
        memcpy(&buf[2], MCU_ENTER_KEY, sizeof(MCU_ENTER_KEY));
    }else{
        buf[1]=0x85;
        memcpy(&buf[2], FPGA_ENTER_KEY, sizeof(FPGA_ENTER_KEY));
    }
    if(SendToMCU(buf, LEN(buf)))
        return WaitHardwareUpdate(buf[1]);
    return false;
}

bool USBControl::SendUpdateData(const uint8_t *data, uint16_t len, bool isMCU, bool isLast)
{
    uint8_t buf[512];
    buf[0] = 0x0a;
    buf[1] = isMCU?0x83:0x86;
    buf[2] = len & 0xff;
    buf[3] = (len >> 8) & 0xff;
    memcpy(&buf[4], data, len);
    if(SendToMCU(buf, LEN(buf)))
    {
        if(isLast)
            QThread::msleep(isMCU?1000:5000);
        return WaitHardwareUpdate(buf[1]);
    }
    return false;
}

bool USBControl::RestartMCU()
{
    quint8 buf[512];
    buf[0]=0x0a;
    buf[1]=0x84;
    return SendToMCU(buf, LEN(buf));
}

bool USBControl::ParameterSetting(quint8* data, quint32 len) {
    return Write(0x11, data, len);
}

bool USBControl::SimpleTrigger(quint8* data, quint32 len) {
    return Write(0x12, data, len);
}

bool USBControl::PWM(quint8 *data, quint32 len)
{
    return Write(0x17, data, len);
}

bool USBControl::CloseAllPWM()
{
    QByteArray dataBytes;
    dataBytes.append(0x20);
    if(PWM((quint8*)dataBytes.data(),dataBytes.size())){
        QByteArray dataBytes2;
        dataBytes2.append(0x10);
        if(PWM((quint8*)dataBytes2.data(),dataBytes2.size()))
            return true;
    }
    return false;
}

bool USBControl::Stop() {
    return Write(0x15, NULL, 0, false);
}

bool USBControl::Exit()
{
    return Write(0x18, NULL, 0, false);
}

