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

#pragma GCC optimize(3,"Ofast","inline")


int log_callback(void *cb_data, int loglevel,
                 const char *format, va_list args){
    static char buffer[2048];
    vsnprintf(buffer,2048,format,args);
    LogHelp::write(QString(buffer));
    return 0;
}

int main(int argc, char* argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::RoundPreferFloor);
#endif
#endif
    QApplication app(argc, argv);
    app.setApplicationVersion(APP_VERSION);
    app.setOrganizationName("ALIENTEK");
    app.setOrganizationDomain("www.alientek.com");
    app.setApplicationName("ATK-LogicView");
    
    QFont font = app.font();
    bool isSetFont=false;
    QFontDatabase database;
    for (auto &i : database.families()) {
        if(i==QStringLiteral("微软雅黑")){
            font.setFamily(QStringLiteral("微软雅黑"));
            isSetFont=true;
            break;
        }
    }
    if(!isSetFont){
        int fontId = QFontDatabase::addApplicationFont(":/resource/OPPOSans-M.ttf");
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        font.setFamily(fontFamilies.at(0));
    }
    app.setFont(font);

    
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

    
    qmlRegisterType<measureTreeViewModel>("atk.qml.Model", 1, 0, "MeasureTreeViewModel");
    qmlRegisterType<VernierListModel>("atk.qml.Model", 1, 0, "VernierListModel");
    qmlRegisterType<DecodeTableModel>("atk.qml.Model", 1, 0, "DecodeTableModel");
    qmlRegisterType<TestFileTableModel>("atk.qml.Model", 1, 0, "TestFileTableModel");
    qmlRegisterType<DecodeListModel>("atk.qml.Model", 1, 0, "DecodeListModel");
    qmlRegisterType<SearchTableModel>("atk.qml.Model", 1, 0, "SearchTableModel");
    
    qmlRegisterType<ShortcutListener>("atk.qml.App", 1, 0, "ShortcutListener");
    
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
    qDebug()<<QDir::tempPath()+"/ATK-Logic";
    engine.rootContext()->setContextProperty("tempDir",QDir::tempPath()+"/ATK-Logic");
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
    engine.rootContext()->setContextProperty("maxChannelNum", dataService->m_channelCount);
    engine.rootContext()->setContextProperty("app_VERSION", APP_VERSION);
    engine.rootContext()->setContextProperty("app_VERSION_NUM", APP_VERSION_NUM);

#ifdef Q_OS_MACX
    engine.addImportPath(QApplication::applicationDirPath()+"/../Resources/qml/");
#endif

    
    LogHelp::init(QDir::tempPath()+"/ATK-Logic/log");

    
    {
        QString tmp="AppVersion：";
        tmp+=APP_VERSION;
        LogHelp::write(tmp);

        tmp=QObject::tr("系统版本：")+QSysInfo::productType()+" "+QSysInfo::productVersion()+" - "+QSysInfo::kernelType()+" "+QSysInfo::kernelVersion();
        LogHelp::write(tmp);

        tmp=QObject::tr("用户：")+QSysInfo::machineHostName();
        LogHelp::write(tmp);

        tmp=QObject::tr("CPU架构：")+QSysInfo::currentCpuArchitecture();
        LogHelp::write(tmp);

#ifdef Q_OS_WIN
        QSettings *CPU = new QSettings("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",QSettings::NativeFormat);
        tmp="CPU："+CPU->value("ProcessorNameString").toString();
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

        QTcpSocket tcpClient;
        tcpClient.abort();
        tcpClient.connectToHost("www.baidu.com", 80);
        tmp=QObject::tr("互联网连接：");
        tmp+=tcpClient.waitForConnected(300)?"true":"false";
        LogHelp::write(tmp);
    }

    
    atk_log_callback_set(log_callback,nullptr);

    
    atk_log_loglevel_set(ATK_LOG_WARN);

    
    qint32 ret=0;
    if (atk_decoder_init(QApplication::applicationDirPath().toStdString().c_str()) == ATK_OK){
        if(atk_decoder_load_all()!=ATK_OK)
            ret=2;
    }else
        ret=1;

    engine.rootContext()->setContextProperty("decode_init_code", ret);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [&](){
        DataService* dataService=DataService::getInstance();
        dataService->setRoot(engine.rootObjects().at(0));
    });
    engine.load(url);
#if defined (QT_NO_DEBUG) && defined (Q_CC_MSVC)
    QBreakpadInstance.setDumpPath(QDir::tempPath()+"/ATK-Logic/crash");
#endif
    return app.exec();
}
