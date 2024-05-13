#ifndef DRAW_CHANNEL_HEADER_H
#define DRAW_CHANNEL_HEADER_H

#include <QQuickPaintedItem>
#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include "pv/static/data_service.h"

class DrawChannelHeader : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit DrawChannelHeader(QQuickItem *parent = 0);

private:
    void drawScale(QPainter* painter,quint32 position,QString text,bool isHigh);
    bool vernierMethod(QPainter* painter=nullptr);
    QString time2TimeStr(qint64 time,QString unit);
    void appendVernierData(qint32 x);
    qint64 getAdsorbPosition(qint32 x);

signals:
    void vernierAppend(qint32 vernierID);
    void showVernierPopup(qint32 vernierID, qint32 mouse_x);
    void closeVernierPopup();
    void vernierDataChanged(qint32 vernierID);
    void deleteVernier(qint32 vernierID);
    void vernierCreate();
    void vernierCreateComplete();
    void vernierMoveState(bool isMove,qint32 id);
    void isExitChanged();

private slots:
    void onSizeChanged();

private:
    QBrush m_text_color = Qt::black;
    Session* m_session=nullptr;
    QString m_sessionID="";
    quint32 m_highLineTop=13;
    quint32 m_highLineBottom=0;
    quint32 m_lowLineTop=22;
    QColor m_vernierTypeColor[6];
    qint32 m_vernierMoveIndex=-1;
    qint64 m_vernierMovePosition=0;
    Qt::MouseButton m_currentButton;
    qint32 m_height=0;//通道高度，只读
    qint32 m_width=0;//通道高度，只读
    qint32 m_selectVernierIndex=0;
    QPoint m_mousePoint;
    bool m_isCreating=false;
    bool m_isExit=false;

protected:
    void paint(QPainter *painter) override;
    void hoverMoveEvent(QHoverEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    //以下为QML属性
public:
    Q_INVOKABLE void init(QString sessionID);
    Q_INVOKABLE void setTheme(QString theme);
    Q_INVOKABLE void createVernier();
    Q_INVOKABLE void vernierCancelMove();

    bool isExit() const;
    void setIsExit(bool newIsExit);

private:
    bool isCreating() const;
    void setIsCreating(bool newIsCreating);

private:
    Q_PROPERTY(bool isCreating READ isCreating WRITE setIsCreating)
    Q_PROPERTY(bool isExit READ isExit WRITE setIsExit NOTIFY isExitChanged)
};

#endif // DRAW_CHANNEL_HEADER_H
