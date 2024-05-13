#ifndef DECODE_LIST_MODEL_H
#define DECODE_LIST_MODEL_H

#include <QAbstractListModel>
#include "./pv/static/data_service.h"

class DecodeListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit DecodeListModel(QObject *parent = nullptr);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void search(QString text);
    Q_INVOKABLE void exitSearch();
    Q_INVOKABLE bool containName(QString name);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    void setCount(qint32 count);

private:
    enum LIST_ITEM_ROLE
    {
        NameRole = Qt::DisplayRole+1,
        IDRole,
        NoteRole
    };
    DecodeService* m_decodeService=nullptr;
    mutable QMutex m_decoderMutex;
    QVector<atk_decoder*> m_decoderShowList;
    QVector<atk_decoder*>* m_decoderList=nullptr;
    QHash<int,QByteArray> m_roleName;
    qint32 m_count=0;
    mutable QMutex m_countMutex;
    bool m_isSearch=false;
    QString m_searchText="";
};

#endif // DECODE_LIST_MODEL_H
