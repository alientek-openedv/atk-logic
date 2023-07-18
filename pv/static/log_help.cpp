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

#include "log_help.h"

QString LogHelp::m_path="";
QFile LogHelp::m_logFile;
QTextStream LogHelp::m_log;
bool LogHelp::m_init=false;
int LogHelp::m_count=0;

LogHelp::LogHelp()
{

}

void LogHelp::init(QString path)
{
    m_init=false;
    try {
        m_path=path;
        QDir dir(path);
        if(!dir.exists())
            dir.mkpath(path);
        m_logFile.setFileName(path+"/"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss")+".log");
        if(m_logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            m_log.setDevice(&LogHelp::m_logFile);
            m_init=true;
        }
    } catch (QException &e) {
        qDebug()<<"初始化日志失败:"<<e.what();
    }
}

void LogHelp::close()
{
    if(m_init){
        m_log.flush();
        m_logFile.flush();
        m_logFile.close();
        m_init=false;
    }
}

void LogHelp::write(QString log)
{
    if(m_init){
        qDebug()<<"log:"<<log;
        m_log<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss.zzz")<<": "<<log<<"\n";
        m_count++;
        if(m_count>5){
            m_count=0;
            m_log.flush();
            m_logFile.flush();
        }
    }
}
