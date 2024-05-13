#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include "pv/static/shared_memory_helper.h"
#include "qtimer.h"
#include <QQuickWindow>
#include <QScreen>
#include <QMouseEvent>
#include <QQuickView>
#include <QRegion>
#include <QSysInfo>
#include <QTextCodec>

#ifdef WIN_DEVICE_CHECK
#include <QSettings>
#include <QProcess>
#include <QDesktopServices>
#include <dbt.h>
#include <windows.h>
#include <VersionHelpers.h>
#include <WinUser.h>
#include <dwmapi.h>
#include <objidl.h> // Fixes error C2504: 'IUnknown' : base class undefined
#include <windowsx.h>
#include <wtypes.h>
#pragma comment(lib, "Dwmapi.lib") // Adds missing library, fixes error LNK2019: unresolved
#pragma comment(lib, "Gdi32.lib")
#endif
#ifdef MAC_DEVICE_CHECK
#include "pv/usb/usb_hotplug.h"
#endif

#include <QTranslator>
#include <QJsonObject>
#include <QQuickItem>

//#include <pv/controller/hot_key.h>
#include <pv/static/data_service.h>
#include <pv/static/decode_service.h>
#include <pv/usb/usb_server.h>
#include <pv/thread/connect.h>
#include <pv/thread/sharedthread.h>
#include <pv/thread/thread_download.h>
#include <pv/static/log_help.h>
#include <stdlib.h>
#pragma comment(lib, "user32.lib")

int log_callback(void *cb_data, int loglevel,
                 const char *format, va_list args);

#ifdef Q_OS_WIN
class TaoFrameLessViewPrivate;
#endif

class FramelessWindow : public QQuickView
{
    Q_OBJECT

    Q_PROPERTY(QString firstLanguage READ firstLanguage NOTIFY firstLanguageChanged)
    Q_PROPERTY(QJsonObject decodeJson READ decodeJson NOTIFY decodeJsonChanged)
    Q_PROPERTY(Qt::WindowState windowState READ getWindowState WRITE setWindowState_ NOTIFY windowState_Changed)
    Q_PROPERTY(bool firstOpenFile READ firstOpenFile WRITE setFirstOpenFile NOTIFY firstOpenFileChanged)
    Q_PROPERTY(bool isLinuxMemoryLimit READ isLinuxMemoryLimit WRITE setIsLinuxMemoryLimit NOTIFY isLinuxMemoryLimitChanged)
    Q_PROPERTY(QString rootDir READ rootDir WRITE setRootDir NOTIFY rootDirChanged)

#ifndef Q_OS_WIN
    enum MouseArea {
        TopLeft = 1,
        Top,
        TopRight,
        Left,
        Move,
        Right,
        BottomLeft,
        Bottom,
        BottomRight,
        Client
    };
#endif


public:
    Q_INVOKABLE void windowClose();
    Q_INVOKABLE void activateWindow();
    void moveToScreenCenter();
    static QRect calcCenterGeo(const QRect& screenGeo, const QSize& normalSize);

public:
    explicit FramelessWindow(QWindow *parent = nullptr);
    ~FramelessWindow();

    Q_INVOKABLE void initSet(qint32 decode_init_code);
    Q_INVOKABLE void initLanguage();
    //type: Demo=0, Device=1 ,File=2
    Q_INVOKABLE QVariant createSession(QVariant name, QVariant channelCount, QVariant type, QVariant port, QString usbName="NULL", qint32 level=0);
    Q_INVOKABLE void setLanguage(QString path);
    Q_INVOKABLE void removeSession(QString sessionID);
    Q_INVOKABLE void openFile(QString path);
    Q_INVOKABLE void checkUpdate(int version, bool isShowError);
    Q_INVOKABLE void checkHardwareUpdate();
    Q_INVOKABLE qint32 getConnectPort();
    Q_INVOKABLE QRect getScreenQRect();
    Q_INVOKABLE void startUpdate(QString fpgaURL, QString mcuURL);
    Q_INVOKABLE void startDownloadFirmware();
    Q_INVOKABLE void enterBootloader();
    Q_INVOKABLE QString getVersionStr();
    Q_INVOKABLE bool isWindows();
    Q_INVOKABLE void initMethod();
    Q_INVOKABLE bool openDataResources();
    Q_INVOKABLE void showMinimized_();
    Q_INVOKABLE void setDecodeLogLevel(qint32 level);
    Q_INVOKABLE void reloadDecoder(qint32 level);
    Q_INVOKABLE QVariantList getDecodeLogList();
    Q_INVOKABLE QString pathRepair_(QString path);
    Q_INVOKABLE void updateApp();
    Q_INVOKABLE void checkUpdateNow();

    const QString &firstLanguage() const;
    void setFirstLanguage(const QString &newFirstLanguage);

    const QJsonObject &decodeJson() const;
    void setDecodeJson(const QJsonObject &newDecodeJson);

    Qt::WindowState getWindowState() const;
    void setWindowState_(Qt::WindowState newWindowState);

    bool firstOpenFile() const;
    void setFirstOpenFile(bool newFirstOpenFile);

    bool isLinuxMemoryLimit() const;
    void setIsLinuxMemoryLimit(bool newIsLinuxMemoryLimit);

    QString rootDir() const;
    void setRootDir(const QString &newRootDir);

protected:
    void showEvent(QShowEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
#ifndef Q_OS_WIN
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
#endif
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

signals:
    void firstLanguageChanged();
    void isDecodeChanged();
    void decodeJsonChanged();
    void windowState_Changed();
    void firstOpenFileChanged();
    void rootDirChanged();

    void sendSharedLock();
    void deviceToFile(QString windowID, qint32 newPort);
    void sendCheckDeviceCreanInfo(USBControl* usb, qint32 port);
    void sendConnectSchedule(qint32 schedule, qint32 state_);
    void sendCheckUpdate(int version, bool isShowError);
    void sendCheckHardwareUpdate(int MCUVersion, int FPGAVersion, int deviceVersion);
    void sendCheckUpdateDate(int state, QString url, QJsonObject json);//state:0=无更新、1=有更新、2=获取错误、3=升级标记
    void sendHardwareCheckUpdateDate(int state, QString url, QJsonObject json);//state:0=无更新、1=有更新、2=获取错误、3=清空文本、4=异常设备
    void sendDownload(qint32 index);
    void sendDownloadSchedule(qint32 schedule, qint32 type, qint32 index);
    void sendStartDownloadFirmware();
    void languageChanged();
    void sendStartUSBHotplug();
    void sendReloadDecoder();
    void isLinuxMemoryLimitChanged();

public slots:
    void onDevicePlugIN();
    void onDevicePlugOUT();
    void receiveDeviceCreanInfo(QString name, QString usbName, qint32 port, qint32 mcuVersions,
                                qint32 fpgaVersions, qint16 deviceVersion, qint32 level, qint8 state);

private:
    void checkedLock();
    void cleanTempFile();
#ifndef Q_OS_WIN
    MouseArea getArea(const QPoint &pos);
    void setWindowGeometry(const QPoint &pos);
    void setCursorIcon();
    void setgeometryfunc(const QSize &size, const QPoint &pos);
#endif
#ifdef WIN_DEVICE_CHECK
    bool registerDeviceNotification();
#endif
    void onWindowStateChanged(Qt::WindowState windowState);

private:
    qint32 m_borderWidth=4;
    qint32 m_moveHeight=36;
    qint32 m_moveRightOfficeWidth=140;
    qint32 m_moveLeftOfficeWidth=360;
    qint32 m_connectPort=-1;
    QPoint m_mousePoint;
    qint32 m_isClicked=0;
    qint32 m_windowMaxState=0;
    bool m_firstOpenFile=false;
    QRect m_moveArea = { m_borderWidth+m_moveLeftOfficeWidth, m_borderWidth, m_borderWidth, m_moveHeight };
    ThreadDownload* m_updateDownload=nullptr;
    DataService* m_dataService;
    QSysInfo::WinVersion m_windowVersion;
    SharedMemoryHelper* m_shared;
    SharedThread m_sharedThread;
    QString m_rootDir;

#ifdef MAC_DEVICE_CHECK
    USBHotplug* m_usbHotplug=nullptr;
#endif
    //    HotKey* m_hotkey=nullptr;
    QMutex m_deviceMutex;
    QJsonObject m_decodeJson;
    QString m_firstLanguage;
    QString m_language;
    bool m_isLinuxMemoryLimit=true;
    Qt::WindowState m_windowState=Qt::WindowNoState;
    ConnectDevice m_connect;
    //语言文件
    QTranslator m_enTrans;
    QTranslator m_zhTrans;
    QTranslator* m_lastTrans=nullptr;
    QVector<QString> m_lengArr;

#ifdef Q_OS_WIN
    TaoFrameLessViewPrivate* d;
#else
    QCursor m_cursorRecode=Qt::ArrowCursor;
    QPoint m_startPos;
    QPoint m_oldPos;
    QSize m_oldSize;
    QSize m_sizeRecode;
    MouseArea m_currentArea = Move;
#endif
};

#endif
