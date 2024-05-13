#ifndef MEASURE_TREE_VIEW_MODEL_H
#define MEASURE_TREE_VIEW_MODEL_H

#include <QAbstractListModel>
#include "./pv/static/data_service.h"

class measureTreeViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit measureTreeViewModel(QObject *parent = nullptr);
    Q_INVOKABLE void init(QString sessionID);
    Q_INVOKABLE void append();
    Q_INVOKABLE void remove(int measureID);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void refresh(int measureID);
    Q_INVOKABLE QVariantMap get(int measureID);
    Q_INVOKABLE void set(int measureID, QString key, QVariant value);
    Q_INVOKABLE void setSelect(int measureID);
    Q_INVOKABLE qint32 getSelectID();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    void refreshAt(int row);

private:
    enum LIST_ITEM_ROLE
    {
        NameRole = Qt::DisplayRole+1,
        MeasureIDRole,
        ChannelIDRole,
        StartRole,
        EndRole,
        StartTextRole,
        StartTextDetailRole,
        EndTextRole,
        EndTextDetailRole,
        TimeRole,
        TimeDetailRole,
        MinFreqRole,
        MinFreqDetailRole,
        MaxFreqRole,
        MaxFreqDetailRole,
        RisingRole,
        FallingRole,
        NoteRole,
        DataColorRole,
        IsSelectRole,
        IsAutoOffsetRole,
        IsCalcRole,
        OffsetRole,
    };
    Session* m_session;
    QVector<MeasureData>* m_measure=nullptr;
    QHash<int,QByteArray> m_roleName;
    qint32 m_count=0;
};

#endif // MEASURE_TREE_VIEW_MODEL_H
