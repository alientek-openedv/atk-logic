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

#ifndef VERNIER_LIST_MODEL_H
#define VERNIER_LIST_MODEL_H

#include <QAbstractListModel>
#include "./pv/static/data_service.h"

class VernierListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit VernierListModel(QObject *parent = nullptr);
    Q_INVOKABLE void init(QString sessionID);
    Q_INVOKABLE void append();
    Q_INVOKABLE void remove(int vernierID);
    Q_INVOKABLE void removeAll(bool isForce);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void refresh(int vernierID);
    Q_INVOKABLE void appendBusy(int vernierID, QString busyID, qint32 flag, QString str);
    Q_INVOKABLE void removeBusy(int vernierID, QString busyID, qint32 flag);
    Q_INVOKABLE QVariantMap get(int index);
    Q_INVOKABLE QVariantMap getAt(int vernierID);
    Q_INVOKABLE void set(int vernierID, QString key, QVariant value);
    Q_INVOKABLE int count();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

signals:
    void closePopup();
    void countChanged();

private:
    void refreshAt(int row);

private:
    enum LIST_ITEM_ROLE
    {
        NameRole = Qt::DisplayRole+1,
        VisibleRole,
        VernierIDRole,
        DataColorRole,
        PositionRole,
        PositionStrRole,
        NoteRole,
        ShowTextRole,
        CostRole
    };
    Session* m_session;
    qint32 m_count=0;
    QVector<VernierData>* m_vernier=nullptr;
    QHash<int,QByteArray> m_roleName;
};

#endif // VERNIER_LIST_MODEL_H
