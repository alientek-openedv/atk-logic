#ifndef HOT_KEY_H
#define HOT_KEY_H

#include <QObject>
#include <QHotkey>
#include <QVector>
#include <pv/static/data_service.h>
#include "pv/draw/frameless_window.h"

class FramelessWindow;
class HotKey : public QObject
{
    Q_OBJECT
public:
    explicit HotKey(FramelessWindow* window, QObject *parent = nullptr);
    ~HotKey();

    void appendHotKey(Qt::Key keyCode, const QString &keyName);
    void appendHotKey(QString keyCode);

private:
    QVector<QHotkey*> m_HotkeyList;
    FramelessWindow* m_parent;
    DataService* m_dataService;
    QString m_errorMsgBefore;
    QString m_errorMsgAfter;
};

#endif // HOT_KEY_H
