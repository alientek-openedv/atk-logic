#ifndef LOG_HELP_H
#define LOG_HELP_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include "qexception.h"

class LogHelp
{
public:
    LogHelp();

    static void init(QString path);
    static void close();
    static void write(QString log);

private:
    static QString m_path;
    static QFile m_logFile;
    static QTextStream m_log;
    static bool m_init;
    static int m_count;
};

#endif // LOG_HELP_H
