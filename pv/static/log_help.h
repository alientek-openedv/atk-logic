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
