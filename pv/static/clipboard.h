#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QApplication>
#include <QObject>
#include <QClipboard>

class Clipboard : public QObject
{
    Q_OBJECT
public:
    explicit Clipboard(QObject *parent = nullptr);
    Q_INVOKABLE void setText(QString text);

private:
    QClipboard *clipboard;
};

#endif // CLIPBOARD_H
