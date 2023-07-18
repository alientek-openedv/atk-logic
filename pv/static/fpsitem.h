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

#ifndef FPSITEM_H
#define FPSITEM_H

#include <QQuickItem>
#include <QTimer>
#include <QQuickWindow>

class FpsItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int fps READ fps NOTIFY fpsChanged)

public:
    explicit FpsItem(QQuickItem *parent = nullptr);

    int fps() const;

signals:
    void fpsChanged();

private:
    QTimer *m_timer;
    int m_fps = 0;
    int m_frameCount = 0;
};

#endif // FPSITEM_H
