#include "frameless_window.h"

#ifdef Q_OS_LINUX
#include <X11/Xlib.h>
#include <QX11Info>
#endif

#ifdef Q_OS_WIN
enum class Style : DWORD
{
    windowed = WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
    aero_borderless = WS_POPUP | WS_THICKFRAME | WS_SIZEBOX | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
    basic_borderless = WS_POPUP | WS_THICKFRAME | WS_SIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX
};

static bool isCompositionEnabled()
{
    BOOL composition_enabled = FALSE;
    bool success = ::DwmIsCompositionEnabled(&composition_enabled) == S_OK;
    return composition_enabled && success;
}

static Style selectBorderLessStyle()
{
    auto version = QSysInfo::windowsVersion();
    return isCompositionEnabled() && version!=QSysInfo::WinVersion::WV_WINDOWS7 ? Style::aero_borderless : Style::basic_borderless;
}
static void setShadow(HWND handle, bool enabled)
{
    if (isCompositionEnabled())
    {
        static const MARGINS shadow_state[2] { { 0, 0, 0, 0 }, { 1, 1, 1, 1 } };
        ::DwmExtendFrameIntoClientArea(handle, &shadow_state[0]);
    }
}

class TaoFrameLessViewPrivate
{
public:
    bool m_firstRun = true;
    HMENU mMenuHandler = NULL;
    bool borderless = true;        // is the window currently borderless
    bool borderless_shadow = false; // should the window display a native aero shadow while borderless
    void setBorderLess(HWND handle, bool enabled)
    {
        auto newStyle = enabled ? selectBorderLessStyle() : Style::windowed;
        auto oldStyle = static_cast<Style>(::GetWindowLongPtrW(handle, GWL_STYLE));

        if (oldStyle != newStyle)
        {
            borderless = enabled;
            ::SetWindowLongPtrW(handle, GWL_STYLE, static_cast<LONG>(newStyle));

            // when switching between borderless and windowed, restore appropriate shadow state
            setShadow(handle, borderless_shadow && (newStyle != Style::windowed));

            // redraw frame
            //            ::SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
            //            ::ShowWindow(handle, SW_SHOW);
        }
    }

    void setBorderLessShadow(HWND handle, bool enabled)
    {
        if (borderless)
        {
            borderless_shadow = enabled;
            setShadow(handle, enabled);
        }
    }
};

#endif

FramelessWindow::FramelessWindow(QWindow *parent)
    : QQuickView (parent)

{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));

    m_windowVersion = QSysInfo::windowsVersion();

#ifdef Q_OS_WIN
    d=new TaoFrameLessViewPrivate();
#else
    setFlags(flags()|Qt::Window|Qt::FramelessWindowHint|Qt::WindowMinMaxButtonsHint);
#endif

    m_dataService = DataService::getInstance();
    m_shared=&m_dataService->m_shared;
    QString path=QCoreApplication::applicationDirPath()+"/update.exe";
    QFile file(path);
    if(file.exists()){
        QFile::setPermissions(path, QFile::ReadOther | QFile::WriteOther);
        file.remove();
    }

    cleanTempFile();

    LogHelp::write(QString("窗口创建"));

    setResizeMode(SizeRootObjectToView);
    setOpacity(0);

    setIcon(QIcon(":/app.ico"));
    connect(this, &QQuickView::widthChanged,[=](int arg){
        m_moveArea.setWidth(arg - m_borderWidth *2 - m_moveRightOfficeWidth-m_moveLeftOfficeWidth-1);
    });

    connect(this, &QQuickView::windowStateChanged, this, &FramelessWindow::onWindowStateChanged);
    connect(this, &FramelessWindow::sendSharedLock, &m_sharedThread, &SharedThread::lock);
    connect(this, &FramelessWindow::sendCheckDeviceCreanInfo, &m_connect, &ConnectDevice::CheckDeviceCreanInfo);
    connect(this, &FramelessWindow::sendCheckUpdate, &m_connect, &ConnectDevice::CheckUpdate);
    connect(this, &FramelessWindow::sendCheckHardwareUpdate, &m_connect, &ConnectDevice::CheckHardwareUpdate);
    connect(&m_connect, &ConnectDevice::SendDeviceCreanInfo, this, &FramelessWindow::receiveDeviceCreanInfo);
    connect(&m_connect, &ConnectDevice::SendConnectSchedule, this, &FramelessWindow::sendConnectSchedule);
    connect(&m_connect, &ConnectDevice::SendCheckUpdateDate, this, &FramelessWindow::sendCheckUpdateDate);
    connect(&m_connect, &ConnectDevice::SendHardwareCheckUpdateDate, this, &FramelessWindow::sendHardwareCheckUpdateDate);

    //居中显示
    setWidth(900);
    setHeight(650);

    moveToScreenCenter();

    //加载语言
    if(!m_zhTrans.load(QString(":/translations/zh_CN.qm")))
        LogHelp::write(QString("加载中文语言失败"));
    if(!m_enTrans.load(QString(":/translations/en_US.qm")))
        LogHelp::write(QString("加载英文语言失败"));
    //加载记录语言
    m_lengArr.append("zh_CN");
    m_lengArr.append("en_US");
}

void FramelessWindow::initSet(qint32 decode_init_code)
{
    //初始化函数，全局唯一启动时候创建
    m_dataService->setWindowError(qobject_cast <WindowError*>(m_dataService->getRoot()->findChild<QObject*>("windowError")));

#ifdef WIN_DEVICE_CHECK
    if(!registerDeviceNotification())
        m_dataService->getWindowError()->setError_msg(QObject::tr("注册USB事件失败"),true);
#endif
#ifdef MAC_DEVICE_CHECK
    m_usbHotplug = new USBHotplug();
    connect(this, &FramelessWindow::sendStartUSBHotplug, m_usbHotplug, &USBHotplug::startHotplug);
    connect(m_usbHotplug, &USBHotplug::DevicePlugIN, this, &FramelessWindow::onDevicePlugIN);
    connect(m_usbHotplug, &USBHotplug::DevicePlugOUT, this, &FramelessWindow::onDevicePlugOUT);
    emit sendStartUSBHotplug();
#endif

    //全局快捷键类
    //m_hotkey=new HotKey(this);


    if(decode_init_code==0)
        decode_init_code=m_dataService->getDecodeServer()->Init();

    switch (decode_init_code) {
    case 1:
        m_dataService->getWindowError()->setError_msg(QObject::tr("初始化协议解码库失败，无法使用协议解码库。"),false);
        break;
    case 2:
        m_dataService->getWindowError()->setError_msg(QObject::tr("获取解码库列表失败。"),false);
        break;
    case 3:
        m_dataService->getWindowError()->setError_msg(QObject::tr("解码库列表为空。"),false);
        break;
    case 0:
        setDecodeJson(m_dataService->getDecodeServer()->getDecodeJson());
    }

    USBServer* usbServer=m_dataService->getUSBServer();
    m_updateDownload=new ThreadDownload(usbServer, nullptr, m_rootDir);
    connect(&m_connect, &ConnectDevice::EnterBootloader, m_updateDownload, &ThreadDownload::onEnterBootloader);
    connect(m_updateDownload, &ThreadDownload::SendDownloadSchedule, this, &FramelessWindow::sendDownloadSchedule);
    connect(this, &FramelessWindow::sendDownload, m_updateDownload, &ThreadDownload::startDownload);
    connect(this, &FramelessWindow::sendStartDownloadFirmware, m_updateDownload, &ThreadDownload::startDownloadFirmware);
}

void FramelessWindow::initLanguage()
{
    //设置语言
    setLanguage(m_firstLanguage);
    setFirstLanguage(m_firstLanguage);
}

FramelessWindow::~FramelessWindow()
{
#ifdef MAC_DEVICE_CHECK
    if(m_usbHotplug!=nullptr)
        delete m_usbHotplug;
#endif
#ifdef Q_OS_WIN
    if (d->mMenuHandler != NULL)
        ::DestroyMenu(d->mMenuHandler);
#endif
    //    if(m_hotkey!=nullptr)
    //        delete m_hotkey;
}

QVariant FramelessWindow::createSession(QVariant name, QVariant channelCount, QVariant type, QVariant port, QString usbName, qint32 level)
{
    if(type.toInt()>0)
        LogHelp::write(QString("创建会话:%1,%2,%3").arg(name.toString()+(level==1?" Plus":""),QString::number(type.toInt()),QString::number(port.toInt())));
    QVariant ret;
    if(type==2){
        QFile f(port.toString());
        if(!f.exists())
        {
            m_dataService->getWindowError()->setError_msg(QObject::tr("文件不存在，无法载入。"),true);
            return ret;
        }
    }
    QObject* object=m_dataService->getRoot()->findChild<QObject*>("sessionTab");
    QMetaObject::invokeMethod(object, "createSession", Q_RETURN_ARG(QVariant, ret), Q_ARG(QVariant, name),
                              Q_ARG(QVariant, usbName), Q_ARG(QVariant, channelCount), Q_ARG(QVariant, type),
                              Q_ARG(QVariant, port), Q_ARG(QVariant, level));
    requestActivate();
    raise();
    return ret;
}

void FramelessWindow::setLanguage(QString lang)
{
    LogHelp::write(QString("设置语言:%1").arg(lang));
    if(m_lastTrans)
        m_dataService->m_app->removeTranslator(m_lastTrans);
    if(lang=="en_US")
        m_lastTrans=&m_enTrans;
    else
        m_lastTrans=&m_zhTrans;
    for(qint32 i=0;i<m_lengArr.length();i++){
        QFile f(m_rootDir+"/"+m_lengArr[i]);
        if(f.exists())
            f.remove();
    }
    m_language=lang;
    QFile f(m_rootDir+"/"+lang);
    f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    f.close();
    m_dataService->m_app->installTranslator(m_lastTrans);
    m_dataService->m_engine->retranslate();
    emit languageChanged();
}

void FramelessWindow::removeSession(QString sessionID)
{
    m_dataService->removeSession(sessionID);
}

void FramelessWindow::windowClose()
{
    LogHelp::write(QString("软件退出"));
    setVisible(false);
    USBServer* usbService=m_dataService->getUSBServer();
    QList<USBControl *> allDevice = usbService->GetAllDevice(true);
    for(auto &i : allDevice)
    {
        if(!i->m_NoDevice && i->IsInit()){
            i->SetResetState(1);
            QThread::msleep(30);
            i->CloseAllPWM();
            i->Exit();
            QThread::msleep(30);
            i->SetResetState(0);
            m_shared->removeConnectDevice(i->GetPort());
        }
    }
    //删除保存目录下的空目录
    cleanTempFile();
    deleteEmptyDir(m_rootDir+"/temp");

    LogHelp::close();
    QString exit_cmd = "taskkill /f /PID " + QString::number(QCoreApplication::applicationPid());
    qApp->quit();
    exit(0);
    system(exit_cmd.toStdString().c_str());
}

void FramelessWindow::activateWindow()
{
#ifdef Q_OS_WIN
    SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    SetWindowPos(HWND(this->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
#endif
    requestActivate();
    this->raise();
}

#ifdef WIN_DEVICE_CHECK
bool FramelessWindow::registerDeviceNotification()
{
    HDEVNOTIFY hDeviceNotify;
    GUID  guid_hiv = { 0xA5DCBF10L, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } };
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    HWND hWnd = (HWND)(this->winId()); //获取当前窗口句柄
    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = guid_hiv;
    hDeviceNotify = RegisterDeviceNotification (hWnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
    LogHelp::write(QString("注册window消息:%1").arg(hDeviceNotify != NULL));
    return (hDeviceNotify != NULL); //true 注册消息成功
}
#endif

#pragma region "窗口事件方法" {

void FramelessWindow::openFile(QString path)
{
    QThread::msleep(100);
    pathRepair(path);
    LogHelp::write(QString("打开文件:%1").arg(path));
    createSession(getStringMiddle(path,"/",".",path.lastIndexOf("/")),m_dataService->m_channelCount,(int)SessionType::File,path);
}

void FramelessWindow::checkUpdate(int version, bool isShowError)
{
    emit sendCheckUpdate(version, isShowError);
}

void FramelessWindow::checkHardwareUpdate()
{
    if(m_connectPort!=-1)
    {
        USBControl* usb=m_dataService->getUSBServer()->PortGetUSB(m_connectPort);
        emit sendCheckHardwareUpdate(usb->m_mcuVersion,usb->m_fpgaVersion,usb->m_deviceVersion);
    }
}

qint32 FramelessWindow::getConnectPort()
{
    return m_connectPort;
}

QRect FramelessWindow::getScreenQRect()
{
    return this->screen()->availableGeometry();
}

void FramelessWindow::startUpdate(QString fpgaURL, QString mcuURL)
{
    m_updateDownload->m_usb=m_dataService->getUSBServer()->PortGetUSB(m_connectPort);
    m_updateDownload->startUpdate(fpgaURL,mcuURL);
}

void FramelessWindow::startDownloadFirmware()
{
    emit sendStartDownloadFirmware();
}

void FramelessWindow::enterBootloader()
{
    if(m_dataService->getUSBServer()->m_errorPort!=-1)
    {
        m_updateDownload->m_usb=m_dataService->getUSBServer()->PortGetUSB(m_dataService->getUSBServer()->m_errorPort);
        m_updateDownload->startUpdate();
    }
}

QString FramelessWindow::getVersionStr()
{
    if(m_connectPort!=-1)
    {
        USBControl* usb=m_dataService->getUSBServer()->PortGetUSB(m_connectPort);
        return "MCU: "+QString::number(usb->m_mcuVersion)+"  FPGA: "+QString::number(usb->m_fpgaVersion)+
                "  HWV: "+QString::number(usb->m_deviceVersion);
    }
    return "";
}

bool FramelessWindow::isWindows()
{
#ifdef Q_OS_WIN
    return true;
#else
    return false;
#endif
}

void FramelessWindow::initMethod()
{
    //首次启动检测设备
    checkedLock();
    onDevicePlugIN();
    //获取命令行参数
    QStringList arguments = QCoreApplication::arguments();
    arguments.pop_front();
    for(auto &i : arguments){
        QFile file(i);
        if(file.exists()){
            m_firstOpenFile=true;
            openFile(i);
        }
    }
}

bool FramelessWindow::openDataResources()
{
    QString path=QCoreApplication::applicationDirPath()+"/ATK-Logic_"+m_language+".pdf";
    QFile file(path);

    if(file.exists()){
#ifdef Q_OS_WIN32
        return QDesktopServices::openUrl(QUrl::fromLocalFile(path));
#endif
#ifdef Q_OS_MACX
        QString cmd= QString("open ATK-Logic_V1.0.pdf");
        return system(cmd.toStdString().c_str())==0;
#endif
#ifdef Q_OS_LINUX
        QString cmd= QString("evince "+path);
        return system(cmd.toStdString().c_str())==0;
#endif

    }
    return false;
}

void FramelessWindow::showMinimized_()
{
#ifdef Q_OS_MACX
    setFlag(Qt::FramelessWindowHint,false);
    showMinimized();
#else
    showMinimized();
#endif
}

void FramelessWindow::setDecodeLogLevel(qint32 level)
{
    if(level>=0 && level<=5)
        atk_log_loglevel_set(level);
}

void FramelessWindow::reloadDecoder(qint32 level)
{
    qint32 ret=0;
    atk_decoder_exit();
    if (atk_decoder_init(QApplication::applicationDirPath().toStdString().c_str()) == ATK_OK){
        if(level>=0 && level<=5)
            atk_log_loglevel_set(level);
        if(atk_decoder_load_all()!=ATK_OK)
            ret=2;
        else
            ret=m_dataService->getDecodeServer()->Init();
    }else
        ret=1;

    switch (ret) {
    case 1:
        m_dataService->getWindowError()->setError_msg(QObject::tr("初始化协议解码库失败，无法使用协议解码库。"),false);
        break;
    case 2:
        m_dataService->getWindowError()->setError_msg(QObject::tr("获取解码库列表失败。"),false);
        break;
    case 3:
        m_dataService->getWindowError()->setError_msg(QObject::tr("解码库列表为空。"),false);
        break;
    case 0:
        setDecodeJson(m_dataService->getDecodeServer()->getDecodeJson());
    }
    emit sendReloadDecoder();
}

QVariantList FramelessWindow::getDecodeLogList()
{
    m_dataService->m_logLock.lock();
    QVariantList ls=m_dataService->m_log;
    m_dataService->m_log.clear();
    m_dataService->m_logLock.unlock();
    return ls;
}

QString FramelessWindow::pathRepair_(QString path)
{
    pathRepair(path);
    return path;
}

void FramelessWindow::updateApp()
{
#ifdef Q_OS_WIN
    QString path=QCoreApplication::applicationDirPath()+"/update.exe";
    QFile file(path);
    if(file.exists()){
        QFile::setPermissions(path, QFile::ReadOther | QFile::WriteOther);
        if(!file.remove())
            m_dataService->getWindowError()->setError_msg(QObject::tr("删除文件失败，请检查更新程序是否启动中。"),true);
    }
    bool isOK=false;
#ifdef _WIN64
    isOK=QFile::copy(":/resource/update/update_x64.exe",path);
#else
    isOK=QFile::copy(":/resource/update/update_x86.exe",path);
#endif
    if(isOK){
        QStringList arguments;
        arguments.append("ATK-Logic");
        isOK=QProcess::startDetached(path,arguments,QCoreApplication::applicationDirPath());
        if(isOK)
            windowClose();
    }
    if(!isOK)
        m_dataService->getWindowError()->setError_msg(QObject::tr("权限不足，请以管理员权限启动后再试。"),true);
#endif
}

void FramelessWindow::checkUpdateNow()
{
    SharedMemoryHelper* shared=&m_dataService->m_shared;
    if(shared->isUpdate())
        windowClose();
}

#pragma endregion}

#pragma region "窗口事件" {

void FramelessWindow::onDevicePlugIN()
{
    while(!m_deviceMutex.try_lock()){
        if(g_isUpdata==2){
            g_isUpdata=0;
            m_deviceMutex.unlock();
        }
        QApplication::processEvents(QEventLoop::AllEvents, 50);
    }
    bool isUnLock=true;
    if(m_connectPort==-1){
        USBServer* usbService=m_dataService->getUSBServer();
        int newPort = usbService->GetNewDevice(m_shared->getConnectDevice());
        if(newPort!=-1)
        {
            LogHelp::write(QString("设备接入端口:%1").arg(QString::number(newPort)));
            auto tmp=usbService->PortGetUSB(newPort);
            tmp->m_busy=true;
            emit sendCheckDeviceCreanInfo(tmp,newPort);
            isUnLock=false;
        }else if(usbService->m_lastError==LIBUSB_ERROR_ACCESS){
            usbService->m_lastError=0;
            m_dataService->getWindowError()->setError_msg(QObject::tr("读取设备权限不足，请提升程序权限再尝试。"),true);
        }
    }
    if(isUnLock)
        m_deviceMutex.unlock();
}

void FramelessWindow::onDevicePlugOUT()
{
    USBServer* usbService=m_dataService->getUSBServer();
    qint32 errorRecodePort=usbService->m_errorPort;
    int deleteDevice=usbService->GetDeleteDevice();
    //删除连接中设备
    if(deleteDevice>0)
    {
        USBControl* usb=usbService->PortGetUSB(deleteDevice);
        usb->m_NoDevice=true;
        while(usb->m_busy)
            QApplication::processEvents(QEventLoop::AllEvents, 100);
        m_connectPort=-1;
        return;
    }
    //删除不存在设备
    if(usbService->m_delete.size()){
        for (int i = 0; i < usbService->m_delete.size(); i++)
        {
            LogHelp::write(QString("断开设备:%1").arg(QString::number(usbService->IdGetPort(usbService->m_delete[i]))));
            USBControl* usb=usbService->IdGetUSB(usbService->m_delete[i]);
            usb->m_NoDevice=true;
            while(usb->m_busy)
                QApplication::processEvents(QEventLoop::AllEvents, 100);
            m_shared->removeConnectDevice(usb->GetPort());
            qint32 newPort=usbService->DeviceToFile(usbService->m_delete[i]);
            emit deviceToFile(usbService->m_delete[i], newPort);
        }
        usbService->m_delete.clear();
        m_connectPort=-1;
        emit sendHardwareCheckUpdateDate(3, "", QJsonObject());
    }else if(errorRecodePort!=-1&&usbService->m_errorPort==-1)
    {
        LogHelp::write(QString("断开连接异常设备:%1").arg(QString::number(errorRecodePort)));
        m_connectPort=-1;
        emit sendConnectSchedule(0,-1);
        emit sendHardwareCheckUpdateDate(3, "", QJsonObject());
    }
}

void FramelessWindow::receiveDeviceCreanInfo(QString name, QString usbName, qint32 port, qint32 mcuVersions,
                                             qint32 fpgaVersions, qint16 deviceVersion, qint32 level, qint8 state)
{
    switch(state){
    case 0:
    {
        m_shared->appendConnectDevice(port);
        m_connectPort=port;
        USBControl* usb=m_dataService->getUSBServer()->PortGetUSB(port);
        usb->m_mcuVersion=mcuVersions;
        usb->m_fpgaVersion=fpgaVersions;
        createSession(name,m_dataService->getUSBServer()->m_channelCount,1,port,usbName,level);
        checkHardwareUpdate();
        emit sendConnectSchedule(100,state);
    }
        break;
    case 1:
    case 2:
    case 6:
    case 7:
        if(m_dataService->getUSBServer()->m_errorPort!=-1)
            m_dataService->getUSBServer()->DeleteDevice(m_dataService->getUSBServer()->m_errorPort);
        if(m_dataService->getUSBServer()->GetAllDevice().count()!=0)
        {
            for(auto &i : m_dataService->getUSBServer()->GetAllDevice()){
                if(i->GetPort()==port)
                {
                    m_dataService->getUSBServer()->m_errorPort=port;
                    break;
                }
            }
        }
        emit sendHardwareCheckUpdateDate(4, "", QJsonObject());
        emit sendConnectSchedule(100,state);
        m_firstOpenFile=false;
        break;
    case 3:
        m_dataService->getUSBServer()->DeleteDevice(port);
        emit sendConnectSchedule(0,-1);
        m_firstOpenFile=false;
        break;
    }
    m_deviceMutex.unlock();
}

void FramelessWindow::checkedLock()
{
    if(m_sharedThread.m_isLock && m_shared->isAttach())
        m_shared->resetAllData();
}

void FramelessWindow::cleanTempFile()
{
    if(!m_shared->isAttach())
        return;
    QString path=m_rootDir+"/temp/";
    if(m_shared->getU8Data(SAVE_FILE)==0){
        QDir dir(path+"save/");
        if(dir.exists()){
            dir.removeRecursively();
            dir.rmpath(path+"save/");
        }
    }
    if(m_shared->getU8Data(LOAD_FILE)==0){
        QDir dir(path+"load/");
        if(dir.exists()){
            dir.removeRecursively();
            dir.rmpath(path+"save/");
        }
    }
}

#ifdef Q_OS_WIN
static long hitTest(RECT winrect, long x, long y, int borderWidth)
{
    // 鼠标区域位于窗体边框，进行缩放
    if ((x >= winrect.left) && (x < winrect.left + borderWidth) && (y >= winrect.top) && (y < winrect.top + borderWidth))
        return HTTOPLEFT;
    else if (x < winrect.right && x >= winrect.right - borderWidth && y >= winrect.top && y < winrect.top + borderWidth)
        return HTTOPRIGHT;
    else if (x >= winrect.left && x < winrect.left + borderWidth && y < winrect.bottom && y >= winrect.bottom - borderWidth)
        return HTBOTTOMLEFT;
    else if (x < winrect.right && x >= winrect.right - borderWidth && y < winrect.bottom && y >= winrect.bottom - borderWidth)
        return HTBOTTOMRIGHT;
    else if (x >= winrect.left && x < winrect.left + borderWidth)
        return HTLEFT;
    else if (x < winrect.right && x >= winrect.right - borderWidth)
        return HTRIGHT;
    else if (y >= winrect.top && y < winrect.top + borderWidth)
        return HTTOP;
    else if (y < winrect.bottom && y >= winrect.bottom - borderWidth)
        return HTBOTTOM;
    else
        return 0;
}
#endif

void FramelessWindow::moveToScreenCenter()
{
    auto geo = calcCenterGeo(screen()->availableGeometry(), size());
    if (minimumWidth() > geo.width() || minimumHeight() > geo.height())
        setMinimumSize(geo.size());
    setGeometry(geo);
    update();
}

QRect FramelessWindow::calcCenterGeo(const QRect& screenGeo, const QSize& normalSize)
{
    int w = normalSize.width();
    int h = normalSize.height();
    int x = screenGeo.x() + (screenGeo.width() - w) / 2;
    int y = screenGeo.y() + (screenGeo.height() - h) / 2;
    if (screenGeo.width() < w)
    {
        x = screenGeo.x();
        w = screenGeo.width();
    }
    if (screenGeo.height() < h)
    {
        y = screenGeo.y();
        h = screenGeo.height();
    }
    return { x, y, w, h };
}

bool FramelessWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    if (!result)
        return false;

#ifdef Q_OS_WIN
    MSG* msg = reinterpret_cast<MSG*>(message);
    unsigned int msgType = msg->message;
    switch(msg->message){
    case WM_DEVICECHANGE:
        if(msg->wParam == DBT_DEVICEARRIVAL)
            onDevicePlugIN();//设备接入
        if(msg->wParam == DBT_DEVICEREMOVECOMPLETE)
            onDevicePlugOUT();//设备拔出
        break;
    case WM_CLOSE:
        windowClose();
        break;
    case WM_NCCALCSIZE: {
        const auto mode = static_cast<BOOL>(msg->wParam);
        //        const auto clientRect = mode ? &(reinterpret_cast<LPNCCALCSIZE_PARAMS>(msg->lParam)->rgrc[0]) : reinterpret_cast<LPRECT>(msg->lParam);
        if (mode == TRUE && d->borderless)
        {
            *result = WVR_REDRAW;
            //            {
            //                if (clientRect->top != 0)
            //                {
            //                    clientRect->top += 0.1;
            //                }
            //            }
            return true;
        }
        *result = 0;
        return true;
    }
    case WM_SETFOCUS:
        if(m_windowVersion==QSysInfo::WinVersion::WV_WINDOWS7){
            setGeometry(x(),y(),width()+1,height());
            update();
            setGeometry(x(),y(),width()-1,height());
        }
        break;
    case WM_KILLFOCUS:
        if(m_windowVersion==QSysInfo::WinVersion::WV_WINDOWS7){
            setGeometry(x(),y(),width()+1,height());
            update();
            setGeometry(x(),y(),width()-1,height());
        }
        break;
    case WM_ACTIVATE:{
        MARGINS margins = { 1,1,1,1 };
        HRESULT hr = S_OK;
        hr = DwmExtendFrameIntoClientArea(msg->hwnd, &margins);
        *result = hr;
        return true;
    }
    case WM_NCACTIVATE:
        if (!isCompositionEnabled())
        {
            *result = 1;
            return true;
        }
        if(msg->lParam==0 && m_windowVersion==QSysInfo::WinVersion::WV_WINDOWS7)
            return true;
        break;
    case WM_NCHITTEST: {
        if(msg->wParam==0)
            msg->wParam=1;

        if (d->borderless)
        {
            RECT winrect;
            GetWindowRect(HWND(winId()), &winrect);

            long x = GET_X_LPARAM(msg->lParam);
            long y = GET_Y_LPARAM(msg->lParam);

            *result = 0;
            if (m_windowState != Qt::WindowMaximized && m_windowState != Qt::WindowMinimized)
            {
                *result = hitTest(winrect, x, y, m_borderWidth);
                if (0 != *result)
                    return true;
            }
            if (m_moveArea.contains(mapFromGlobal(QCursor().pos())))
            {
                *result = HTCAPTION;
                return true;
            }
            return false;
        }
        break;
    }
    case WM_DPICHANGED:
        break;
    }
#endif
    return QWindow::nativeEvent(eventType, message, result);
}

void FramelessWindow::onWindowStateChanged(Qt::WindowState windowState)
{
#ifdef Q_OS_MACX
    setFlag(Qt::FramelessWindowHint,true);
#endif

    bool isSet=true;
    if(windowState==Qt::WindowMaximized)
        m_windowMaxState=1;
    else if(m_windowMaxState==1||m_windowMaxState==2){
        if(windowState==Qt::WindowMinimized)
            m_windowMaxState=2;
        else if(m_windowMaxState==2&&windowState==Qt::WindowNoState){
            m_windowMaxState=0;
            isSet=false;
            showMaximized();
        }else
            m_windowMaxState=0;
    }else
        m_windowMaxState=0;
    if(isSet)
        setWindowState_(windowState);
}

QString FramelessWindow::rootDir() const
{
    return m_rootDir;
}

void FramelessWindow::setRootDir(const QString &newRootDir)
{
    if (m_rootDir == newRootDir)
        return;
    m_rootDir = newRootDir;
    m_dataService->m_tempDir=m_rootDir;
    m_sharedThread.init(m_rootDir);
    emit sendSharedLock();
    bool isSetLang=false;
    for(qint32 i=0;i<m_lengArr.length();i++){
        QFile f(m_rootDir+"/"+m_lengArr[i]);
        if(f.exists()){
            isSetLang=true;
            m_firstLanguage=m_lengArr[i];
            break;
        }
    }
    if(!isSetLang){
        //加载系统语言
        QLocale locale;
        if(locale.language() == QLocale::English)  //获取系统语言环境
            m_firstLanguage="en_US";
        else
            m_firstLanguage="zh_CN";
    }
    emit rootDirChanged();
}

bool FramelessWindow::isLinuxMemoryLimit() const
{
    return m_isLinuxMemoryLimit;
}

void FramelessWindow::setIsLinuxMemoryLimit(bool newIsLinuxMemoryLimit)
{
    if (m_isLinuxMemoryLimit == newIsLinuxMemoryLimit)
        return;
    m_isLinuxMemoryLimit = newIsLinuxMemoryLimit;
    g_isLinuxMemoryLimit=m_isLinuxMemoryLimit;
    emit isLinuxMemoryLimitChanged();
}

bool FramelessWindow::firstOpenFile() const
{
    return m_firstOpenFile;
}

void FramelessWindow::setFirstOpenFile(bool newFirstOpenFile)
{
    if (m_firstOpenFile == newFirstOpenFile)
        return;
    m_firstOpenFile = newFirstOpenFile;
    emit firstOpenFileChanged();
}

Qt::WindowState FramelessWindow::getWindowState() const
{
    return m_windowState;
}

void FramelessWindow::setWindowState_(Qt::WindowState newWindowState)
{
    if (m_windowState == newWindowState)
        return;
    m_windowState = newWindowState;
    emit windowState_Changed();
}

const QJsonObject &FramelessWindow::decodeJson() const
{
    return m_decodeJson;
}

void FramelessWindow::setDecodeJson(const QJsonObject &newDecodeJson)
{
    if (m_decodeJson == newDecodeJson)
        return;
    m_decodeJson = newDecodeJson;
    emit decodeJsonChanged();
}

const QString &FramelessWindow::firstLanguage() const
{
    return m_firstLanguage;
}

void FramelessWindow::setFirstLanguage(const QString &newFirstLanguage)
{
    m_firstLanguage = newFirstLanguage;
    emit firstLanguageChanged();
}

void FramelessWindow::showEvent(QShowEvent* e)
{
#ifdef Q_OS_WIN
    if (d->m_firstRun)
    {
        d->m_firstRun = false;
        // 第一次show的时候，设置无边框。不在构造函数中设置。取winId会触发QWindowsWindow::create,直接创建win32窗口,引起错乱(win7 或者虚拟机启动即黑屏)。
        d->setBorderLess((HWND)(winId()), d->borderless);
        {
            d->mMenuHandler = ::CreateMenu();
            ::SetMenu((HWND)winId(), d->mMenuHandler);
        }
        //        //win7下禁用aero
        //        auto version = QSysInfo::windowsVersion();
        //        if(version==QSysInfo::WinVersion::WV_WINDOWS7){
        //            DwmEnableComposition(DWM_EC_DISABLECOMPOSITION);
        //        }
    }
#endif
    QQuickView::showEvent(e);
}

void FramelessWindow::resizeEvent(QResizeEvent* e)
{
    QQuickView::resizeEvent(e);
}

#ifndef Q_OS_WIN

FramelessWindow::MouseArea FramelessWindow::getArea(const QPoint &pos)
{
    int x = pos.x();
    int y = pos.y();
    int w = width();
    int h = height();
    MouseArea area;
    if(m_windowState == Qt::WindowNoState)
    {
        if (x >= 0 && x <= m_borderWidth && y >= 0 && y <= m_borderWidth) {
            area = TopLeft;
        } else if (x > m_borderWidth && x < (w - m_borderWidth) && y >= 0 && y <= m_borderWidth) {
            area = Top;
        } else if (x >=(w - m_borderWidth) && x <= w && y >= 0 && y <= m_borderWidth) {
            area = TopRight;
        } else if (x >= 0 && x <= m_borderWidth && y > m_borderWidth && y < (h - m_borderWidth)) {
            area = Left;
        } else if (x >=(w - m_borderWidth) && x <= w && y > m_borderWidth && y < (h - m_borderWidth)) {
            area = Right;
        } else if (x >= 0 && x <= m_borderWidth && y >= (h - m_borderWidth) && y <= h) {
            area = BottomLeft;
        } else if (x > m_borderWidth && x < (w - m_borderWidth) && y >= (h - m_borderWidth) && y <= h) {
            area = Bottom;
        } else if (x >=(w - m_borderWidth) && x <= w && y >= (h - m_borderWidth) && y <= h) {
            area = BottomRight;
        }else if (m_moveArea.contains(x, y)) {
            area = Move;
        } else {
            area = Client;
        }
    }else{
        if (m_moveArea.contains(x, y)) {
            area = Move;
        } else {
            area = Client;
        }
    }
    return area;
}

void FramelessWindow::setCursorIcon()
{
    static bool unset = false;

    switch (m_currentArea) {
    case TopLeft:
    case BottomRight:
        if(!unset)
            m_cursorRecode=cursor();
        unset = true;
        setCursor(Qt::SizeFDiagCursor);
        break;
    case Top:
    case Bottom:
        if(!unset)
            m_cursorRecode=cursor();
        unset = true;
        setCursor(Qt::SizeVerCursor);
        break;
    case TopRight:
    case BottomLeft:
        if(!unset)
            m_cursorRecode=cursor();
        unset = true;
        setCursor(Qt::SizeBDiagCursor);
        break;
    case Left:
    case Right:
        if(!unset)
            m_cursorRecode=cursor();
        unset = true;
        setCursor(Qt::SizeHorCursor);
        break;
    case Move:
    default:
        if (unset) {
            unset = false;
            setCursor(m_cursorRecode);
        }
        break;
    }
}

void FramelessWindow::setgeometryfunc(const QSize &size, const QPoint &pos)
{
    QPoint temp_pos = m_oldPos;
    QSize temp_size = minimumSize();
    if (size.width() > minimumWidth()) {
        temp_pos.setX(pos.x());
        temp_size.setWidth(size.width());
    } else {
        if (pos.x()!=temp_pos.x())
            temp_pos.setX(m_oldPos.x()+m_oldSize.width()-minimumWidth());
    }
    if (size.height() > minimumHeight()) {
        temp_pos.setY(pos.y());
        temp_size.setHeight(size.height());
    } else {
        if(pos.y()!=temp_pos.y())
            temp_pos.setY(m_oldPos.y() + m_oldSize.height() - minimumHeight());
    }
    setGeometry(QRect(temp_pos, temp_size));
    update();
}

void FramelessWindow::setWindowGeometry(const QPoint &pos)
{
    QPoint offset = m_startPos - pos;
    if (offset.x() == 0 && offset.y() == 0)
        return;

    switch (m_currentArea) {
    case TopLeft:
        setgeometryfunc(m_oldSize + QSize(offset.x(), offset.y()), m_oldPos - offset);
        break;
    case Top:
        setgeometryfunc(m_oldSize + QSize(0, offset.y()), m_oldPos - QPoint(0, offset.y()));
        break;
    case TopRight:
        setgeometryfunc(m_oldSize - QSize(offset.x(), -offset.y()), m_oldPos - QPoint(0, offset.y()));
        break;
    case Left:
        setgeometryfunc(m_oldSize + QSize(offset.x(), 0), m_oldPos - QPoint(offset.x(), 0));;
        break;
    case Right:
        setgeometryfunc(m_oldSize - QSize(offset.x(), 0), position());
        break;
    case BottomLeft:
        setgeometryfunc(m_oldSize + QSize(offset.x(), -offset.y()), m_oldPos - QPoint(offset.x(), 0));
        break;
    case Bottom:
        setgeometryfunc(m_oldSize + QSize(0, -offset.y()), position());
        break;
    case BottomRight:
        setgeometryfunc(m_oldSize - QSize(offset.x(), offset.y()), position());
        break;
    default:
        break;
    }
}


void FramelessWindow::mousePressEvent(QMouseEvent *event)
{
    m_startPos = event->globalPos();
    m_oldPos = position();
    m_oldSize = size();
    event->ignore();
    QQuickView::mousePressEvent(event);
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_oldPos = position();
    QQuickView::mouseReleaseEvent(event);
}

void FramelessWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_currentArea == Move) {
        if(m_windowState==Qt::WindowMaximized)
            showNormal();
        else
            showMaximized();
        m_currentArea = Client;
    }
    QQuickView::mouseDoubleClickEvent(event);
}

void FramelessWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        if (m_currentArea == Move) {

#ifdef Q_OS_LINUX
            QPoint pos = this->mapToGlobal(event->pos());
            XEvent event;
            memset(&event, 0, sizeof(XEvent));

            Display *display = QX11Info::display();
            event.xclient.type = ClientMessage;
            event.xclient.message_type = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
            event.xclient.display = display;
            event.xclient.window = (XID)(winId());
            event.xclient.format = 32;
            event.xclient.data.l[0] = pos.x();
            event.xclient.data.l[1] = pos.y();
            event.xclient.data.l[2] = 8;
            event.xclient.data.l[3] = Button1;
            event.xclient.data.l[4] = 1;

            XUngrabPointer(display, CurrentTime);
            XSendEvent(display,
                       QX11Info::appRootWindow(QX11Info::appScreen()),
                       False,
                       SubstructureNotifyMask | SubstructureRedirectMask,
                       &event);
            XFlush(display);
#else
            setPosition(m_oldPos - m_startPos + event->globalPos());
            if (m_windowState == Qt::WindowMaximized)
                showNormal();
#endif
        } else if (m_currentArea != Move){
            setWindowGeometry(event->globalPos());
        }
    } else {
        QPoint pos = event->pos();
        m_currentArea = getArea(pos);
        setCursorIcon();
    }

    QQuickView::mouseMoveEvent(event);
}
#endif

#pragma endregion}

