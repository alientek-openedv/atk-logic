#ifndef DRAW_BACKGROUND_H
#define DRAW_BACKGROUND_H

#include <QQuickPaintedItem>
#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include "pv/static/data_service.h"
#include "pv/static/util.h"

struct CrossChannelMeasure
{
    QPoint point=QPoint();
    qint32 YOffset;
    qint64 position=0;
    bool isHit;
    qint32 mouseY;
};

class DrawBackground : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit DrawBackground(QQuickItem *parent = nullptr);

private:
    void drawVernier(QPainter &painter);
    void drawMouseZoom(QPainter &painter);
    void drawCrossChannelMeasure(QPainter &painter);

private slots:
    void onSizeChanged();

private:
    Session* m_session=nullptr;
    QString m_sessionID="";
    qint32 m_height=0;//通道高度，只读
    qint32 m_width=0;//通道高度，只读
    bool m_showMouseZoom=false;//是否绘制鼠标缩放矩形
    QRect m_mouseZoomRect;//绘制矩形
    qint32 m_mouseZoomOffsetY=0;
    bool m_isExit=false;
    bool m_crossChannelMeasureState=false;
    CrossChannelMeasure m_crossChannelMeasureStartPosition;
    CrossChannelMeasure m_crossChannelMeasureEndPosition;
    QVector<QLine> m_crossChannelHitLines;

    Q_PROPERTY(bool isExit READ isExit WRITE setIsExit NOTIFY isExitChanged)

protected:
    void paint(QPainter *painter) override;

    //以下QML变量
public:
    Q_INVOKABLE void init(QString sessionID);
    Q_INVOKABLE void setMouseZoom(bool show, qint32 x, qint32 y, qint32 width, qint32 height);
    Q_INVOKABLE void setMouseYOffset(qint32 y);
    Q_INVOKABLE void setCrossChannelMeasureState(bool isStop);
    Q_INVOKABLE void setCrossChannelMeasurePosition(qint32 type, qint32 x, qint32 y, qint32 mouseY, qint64 position, bool isHit);
    Q_INVOKABLE void setCrossChannelMeasureYOffset(qint32 type, qint32 YOffset);

    bool isExit() const;
    void setIsExit(bool newIsExit);
signals:
    void isExitChanged();
};

#endif // DRAW_BACKGROUND_H
