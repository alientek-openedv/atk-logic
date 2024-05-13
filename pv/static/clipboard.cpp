#include "clipboard.h"

Clipboard::Clipboard(QObject *parent) : QObject(parent)
{
    clipboard = QApplication::clipboard();
}

void Clipboard::setText(QString text)
{
    clipboard->setText(text,QClipboard::Clipboard);
}
