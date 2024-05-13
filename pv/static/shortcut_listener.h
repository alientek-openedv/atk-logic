#ifndef SHORTCUTLISTENER_H
#define SHORTCUTLISTENER_H

#include <QEvent>
#include "qevent.h"
#include <QObject>
#include <QQmlEngine>

class ShortcutListener : public QObject
{
    Q_OBJECT
public:
    explicit ShortcutListener(QObject *parent = nullptr);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

signals:
    void keyPress(int key);
    void keyRelease(int key);
    void focusOut();

public:
    Q_INVOKABLE void init(QObject *object);

    bool Enable() const;
    void setEnable(bool newEnable);

private:
    QObject* m_parent;
    bool m_Enable=false;
    Q_PROPERTY(bool Enable READ Enable WRITE setEnable)
};

#endif // SHORTCUTLISTENER_H
