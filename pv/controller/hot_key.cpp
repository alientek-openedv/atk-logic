#include "hot_key.h"

HotKey::HotKey(FramelessWindow* window, QObject *parent)
    : m_parent(window),QObject{parent}
{
    m_dataService = DataService::getInstance();
    m_errorMsgBefore=QObject::tr("注册采集热键");
    m_errorMsgAfter=QObject::tr("冲突，请确认其他软件是否使用此热键。");
    appendHotKey(Qt::Key_F5, "F5");

#ifdef Q_OS_MAC
    appendHotKey("⌘+G");
#else
    appendHotKey("ctrl+G");
#endif
}

HotKey::~HotKey()
{
    for(int i=0;i<m_HotkeyList.count();i++){
        delete m_HotkeyList[i];
    }
}

void HotKey::appendHotKey(Qt::Key keyCode, const QString &keyName)
{
    QHotkey* hotkey=new QHotkey(keyCode, Qt::NoModifier, true, this);
    if(!hotkey->isRegistered())
    {
        if(keyCode==Qt::Key_F5)
            m_dataService->getWindowError()->setError_msg(m_errorMsgBefore+" "+keyName+" "+m_errorMsgAfter,true);
        delete hotkey;
    }
//    else{
//        if(keyCode==Qt::Key_F5)
//            connect(hotkey, &QHotkey::activated, m_parent, &FramelessWindow::onHotkeyClick_F5);
//        m_HotkeyList.append(hotkey);
//    }
}

void HotKey::appendHotKey(QString keyCode)
{
    QHotkey* hotkey=new QHotkey(QKeySequence(keyCode), true);
    if(!hotkey->isRegistered())
    {
        m_dataService->getWindowError()->setError_msg(m_errorMsgBefore+" "+keyCode+" "+m_errorMsgAfter,true);
        delete hotkey;
    }
//    else{
//        if(keyCode=="ctrl+G")
//            connect(hotkey, &QHotkey::activated, m_parent, &FramelessWindow::onHotkeyClick_Ctrl_G);
//        m_HotkeyList.append(hotkey);
//    }
}
