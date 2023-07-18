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

#ifndef WINDOW_ERROR_H
#define WINDOW_ERROR_H

#include <QObject>
#include <QApplication>
#include <pv/static/log_help.h>

class WindowError : public QObject
{
    Q_OBJECT
public:
    explicit WindowError(QObject *parent = nullptr);

    void setError_msg(const QString &newError_msg);
    const QString &getError_msg() const;
    void setError_msg(const QString &newError_msg, bool waitClose_);
    void waitClose();

    bool getIsClose() const;
    void setIsClose(bool newIsClose);

private:
    QString error_msg;
    bool isClose;

signals:
    void error_msgChanged();
    void isCloseChanged();

private:
    Q_PROPERTY(QString error_msg READ getError_msg WRITE setError_msg NOTIFY error_msgChanged)
    Q_PROPERTY(bool isClose READ getIsClose WRITE setIsClose NOTIFY isCloseChanged)
};

#endif // WINDOW_ERROR_H
