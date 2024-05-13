#ifndef DRAW_BACKGROUND_FLOOR_H
#define DRAW_BACKGROUND_FLOOR_H

#include <QQuickPaintedItem>
#include "pv/static/data_service.h"

class DrawBackgroundFloor : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit DrawBackgroundFloor(QQuickItem *parent = nullptr);

private:
    void vernierMethod(bool isClick=false);
    void vernierMove(qint32 x);
    qint64 getAdsorbPosition(qint32 x);

private slots:
    void onSizeChanged();

signals:
    void vernierDataChanged(qint32 vernierID);
    void measureSelectChanged();
    void closeVernierPopup();
    void hoverLeave();
    void drawUpdate();
    void showViewScope(qint64 start, qint64 end);
    void mouseZoom(bool show, qint32 x, qint32 y, qint32 width, qint32 height);
    void deleteVernier(qint32 vernierID);
    void vernierCreateComplete();
    void getAdsorbChannelID(qint32 y);
    void vernierMoveState(bool isMove, qint32 id);
    void crossChannelMeasureState(bool isStop);
    void sendCrossChannelMeasurePosition(qint32 type, qint32 x, qint32 y, qint32 mouseY, qint64 position, bool isHit);

private:
    Session* m_session=nullptr;
    QString m_sessionID="";
    qint32 m_height=0;//通道高度，只读
    qint32 m_width=0;//通道高度，只读
    QPoint m_mousePoint;
    QPoint m_oldPoint;
    qint32 m_selectVernierIndex=-1;
    qint64 m_selectVernierPosition=0;
    QPoint m_zoomPoint,m_zoomPointEnd;
    Qt::MouseButton m_currentButton;

protected:
    void paint(QPainter *painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override;

    //以下QML变量
public:
    Q_INVOKABLE void init(QString sessionID);
    Q_INVOKABLE void vernierCancelMove();
    Q_INVOKABLE void setCrossChannelMeasureState(bool isStop);

    bool showCursor() const;
    void setShowCursor(bool newShowCursor);

    bool vernierCreateModel() const;
    void setVernierCreateModel(bool newVernierCreateModel);

    qint32 adsorbChannelID() const;
    void setAdsorbChannelID(qint32 newAdsorbChannelID);

    bool isExit() const;
    void setIsExit(bool newIsExit);

signals:
    void showCursorChanged();

    void vernierCreateModelChanged();

    void adsorbChannelIDChanged();

    void isExitChanged();

private:
    bool m_showCursor=false;
    bool m_vernierCreateModel=false;
    bool m_isExit=false;
    qint32 m_adsorbChannelID;
    bool m_crossChannelMeasureState=false;

    Q_PROPERTY(bool showCursor READ showCursor WRITE setShowCursor NOTIFY showCursorChanged)
    Q_PROPERTY(bool vernierCreateModel READ vernierCreateModel WRITE setVernierCreateModel NOTIFY vernierCreateModelChanged)
    Q_PROPERTY(qint32 adsorbChannelID READ adsorbChannelID WRITE setAdsorbChannelID NOTIFY adsorbChannelIDChanged)
    Q_PROPERTY(bool isExit READ isExit WRITE setIsExit NOTIFY isExitChanged)
};

#endif // DRAW_BACKGROUND_FLOOR_H
