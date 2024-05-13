#include <QApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>
#include <QLocale>
#include <QDebug>
#include <QTranslator>
#include <QQuickWindow>
#include <QFontDatabase>
#include "QBreakpadHandler.h"
#include "./pv/controller/session_controller.h"
#include "./pv/controller/session_error.h"
#include "./pv/controller/setting.h"
#include "./pv/draw/draw_channel.h"
#include "./pv/draw/draw_channel_header.h"
#include "./pv/draw/draw_background.h"
#include "./pv/draw/draw_background_floor.h"
#include "./pv/draw/frameless_window.h"
#include "./pv/static/data_service.h"
#include "./pv/static/window_error.h"
#include "./pv/static/shortcut_listener.h"
#include "./pv/static/clipboard.h"
#include "./pv/static/fpsitem.h"
#include "./pv/model/measure_tree_view_model.h"
#include "./pv/model/vernier_list_model.h"
#include "./pv/model/decode_table_model.h"
#include "./pv/model/test_file_table_model.h"
#include "./pv/model/decode_list_model.h"
#include "./pv/model/search_table_model.h"
#include <pv/static/menustyle.h>

#pragma GCC optimize(3,"Ofast","inline")


////程式异常捕获
//long __stdcall callback(_EXCEPTION_POINTERS* pException)
//{
//    int code =  pException->ExceptionRecord->ExceptionCode;
//    int flags = pException->ExceptionRecord->ExceptionFlags;
//    LogHelp::write(QString("程序异常：Code=%1， Flags=%2").arg(QString::number(code),QString::number(flags)));
//    return EXCEPTION_EXECUTE_HANDLER;
//}

DataService* g_dataService;

int log_callback(void *cb_data, int loglevel,
                 const char *format, va_list args){
    static char buffer[2048];
    vsnprintf(buffer,2048,format,args);
    if(g_dataService!=nullptr){
        g_dataService->m_logLock.lock();
        g_dataService->m_log.append(QDateTime::currentDateTime().toString("hh:mm:ss.zzz: ")+QString(buffer)+"\n");
        g_dataService->m_logLock.unlock();
    }

    LogHelp::write(QString(buffer));
    return 0;
}

int main(int argc, char* argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);//启用系统百分比缩放
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_Use96Dpi);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Round);
#endif
#endif
    QApplication app(argc, argv);
    app.setApplicationVersion(APP_VERSION);
    app.setOrganizationName("ALIENTEK");
    app.setOrganizationDomain("www.alientek.com");
    app.setApplicationName("ATK-LogicView");
    app.setStyle(new MenuStyle());
    QString tempDir=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#if defined (QT_NO_DEBUG) && defined (Q_CC_MSVC)
    QBreakpadInstance.setDumpPath(tempDir+"/crash");
#endif
    //载入全局字体
    QFont font = app.font();
    int fontId = QFontDatabase::addApplicationFont(":/resource/SourceHanSansCN-Regular.otf");
    QFontDatabase::addApplicationFont(":/resource/OPPOSans-M.ttf");
    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
    font.setHintingPreference(QFont::PreferVerticalHinting);
    font.setStyleHint(QFont::System, QFont::NoSubpixelAntialias);
    font.setFamily(fontFamilies.at(0));
    app.setFont(font);

    //设置中文编码
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("GBK");
#else
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForTr(codec);
#else
#if defined (Q_OS_LINUX) || defined (Q_OS_MACX)
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
#else
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForLocale(codec);
#endif
#endif

    //导入model
    qmlRegisterType<measureTreeViewModel>("atk.qml.Model", 1, 0, "MeasureTreeViewModel");
    qmlRegisterType<VernierListModel>("atk.qml.Model", 1, 0, "VernierListModel");
    qmlRegisterType<DecodeTableModel>("atk.qml.Model", 1, 0, "DecodeTableModel");
    qmlRegisterType<TestFileTableModel>("atk.qml.Model", 1, 0, "TestFileTableModel");
    qmlRegisterType<DecodeListModel>("atk.qml.Model", 1, 0, "DecodeListModel");
    qmlRegisterType<SearchTableModel>("atk.qml.Model", 1, 0, "SearchTableModel");
    //导入事件过滤器
    qmlRegisterType<ShortcutListener>("atk.qml.App", 1, 0, "ShortcutListener");
    //导入控件
    qmlRegisterType<Setting>("atk.qml.Controls", 1, 0, "QSetting");
    qmlRegisterType<FpsItem>("atk.qml.Controls", 1, 0, "FpsItem");
    qmlRegisterType<FramelessWindow>("atk.qml.Controls", 1, 0, "FramelessWindow");
    qmlRegisterType<WindowError>("atk.qml.Controls", 1, 0, "WindowError");
    qmlRegisterType<SessionController>("atk.qml.Controls", 1, 0, "SessionController");
    qmlRegisterType<SessionError>("atk.qml.Controls", 1, 0, "SessionError");
    qmlRegisterType<DrawBackground>("atk.qml.Controls", 1, 0, "DrawBackground");
    qmlRegisterType<DrawBackgroundFloor>("atk.qml.Controls", 1, 0, "DrawBackgroundFloor");
    qmlRegisterType<DrawChannel>("atk.qml.Controls", 1, 0, "DrawChannel");
    qmlRegisterType<DrawChannelHeader>("atk.qml.Controls", 1, 0, "DrawChannelHeader");
    QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Software);
    QQmlApplicationEngine engine;
    qDebug()<<tempDir;
    engine.rootContext()->setContextProperty("tempDir", tempDir);
    engine.rootContext()->setContextProperty("clipboard",new Clipboard());
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject * obj, const QUrl & objUrl)
    {
        if(!obj && url == objUrl)
        {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);
    DataService* dataService=DataService::getInstance();
    dataService->m_app=&app;
    dataService->m_engine=&engine;
    g_dataService=dataService;
    engine.rootContext()->setContextProperty("maxChannelNum", dataService->m_channelCount);
    engine.rootContext()->setContextProperty("app_VERSION", APP_VERSION);
    engine.rootContext()->setContextProperty("app_VERSION_NUM", APP_VERSION_NUM);

#ifdef Q_OS_MACX
    engine.addImportPath(QApplication::applicationDirPath()+"/../Resources/qml/");
#endif

    //初始化日志
    LogHelp::init(tempDir+"/log");

    //输出系统信息
    {
        QString tmp="AppVersion：";
        tmp+=APP_VERSION;
#ifdef _WIN64
        tmp+=" x64";
#else
        tmp+=" x86";
#endif
        LogHelp::write(tmp);

        tmp=QObject::tr("系统版本：")+QSysInfo::productType()+" "+QSysInfo::productVersion()+" - "+QSysInfo::kernelType()+" "+QSysInfo::kernelVersion();
        LogHelp::write(tmp);

        tmp=QObject::tr("用户：")+QSysInfo::machineHostName();
        LogHelp::write(tmp);

        tmp=QObject::tr("CPU架构：")+QSysInfo::currentCpuArchitecture();
        LogHelp::write(tmp);

#ifdef Q_OS_WIN
        QSettings *CPU = new QSettings("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",QSettings::NativeFormat);
        tmp="CPU："+CPU->value("ProcessorNameString").toString().trimmed();
        delete CPU;
        LogHelp::write(tmp);

        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof (statex);
        GlobalMemoryStatusEx(&statex);
        tmp=QObject::tr("物理内存总量：")+QString::number(statex.ullTotalPhys*1.0/(1024*1024*1024))+" GB";
        LogHelp::write(tmp);
        tmp=QObject::tr("可用的物理内存：")+QString::number(statex.ullAvailPhys*1.0/(1024*1024*1024))+" GB";
        LogHelp::write(tmp);
#endif
    }

    //设置解码器回调函数
    atk_log_callback_set(log_callback,nullptr);

    //设置解码器回调等级
    atk_log_loglevel_set(ATK_LOG_WARN);

    //QApplication::applicationDirPath().toStdString().c_str()
    qint32 ret=0;
    if (atk_decoder_init(QApplication::applicationDirPath().toStdString().c_str()) == ATK_OK){
        if(atk_decoder_load_all()!=ATK_OK)
            ret=2;
    }else
        ret=1;

#ifdef Q_OS_WIN
    engine.rootContext()->setContextProperty("windowType", 1);
#endif
#ifdef Q_OS_LINUX
    engine.rootContext()->setContextProperty("windowType", 2);
#endif
#ifdef Q_OS_MACX
    engine.rootContext()->setContextProperty("windowType", 3);
#endif
    engine.rootContext()->setContextProperty("decode_init_code", ret);
    engine.rootContext()->setContextProperty("setRoot", 0);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [&](){
        DataService* dataService=DataService::getInstance();
        dataService->setRoot(engine.rootObjects().at(0));
        engine.rootContext()->setContextProperty("setRoot", 1);
    });
    engine.load(url);
    //    SetUnhandledExceptionFilter(callback);
    return app.exec();
}
