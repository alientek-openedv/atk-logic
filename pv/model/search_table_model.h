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

#ifndef SEARCH_TABLE_MODEL_H
#define SEARCH_TABLE_MODEL_H

#include <QTimer>
#include <QAbstractItemModel>
#include <QtConcurrent/QtConcurrent>
#include "./pv/static/data_service.h"

struct SearchData
{
    qint64 start;
    qint64 end;
    SearchData(qint64 Start, qint64 End){
        start=Start;
        end=End;
    }
};

class SearchTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit SearchTableModel(QObject *parent = nullptr);
    ~SearchTableModel();
    Q_INVOKABLE void init(QString sessionID);
    Q_INVOKABLE void stopAll();
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void setLineHeight(qint32 lineHeight);
    Q_INVOKABLE qint32 refreshBuffer(qint32 y);
    Q_INVOKABLE qint32 getShowCount();
    Q_INVOKABLE qint32 getShowFirst();
    Q_INVOKABLE qint32 scrollPosition(double position);

    Q_INVOKABLE void refreshCount();
    Q_INVOKABLE bool search(const QString &text, qint64 start, qint64 end);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

signals:
    void refreshCountTimer(bool isRun);
    void modelDataChanged();

private:
    void setCount(qint32 count);
    void searchData();
    QVector<qint8> getSearchEdge(const QString &searchText);
    QVector<qint8> getSearchLevel(const QString &searchText);
    bool isData(const QString &str);
    void refreshBuffer();
    void resetSort();

private:
    enum LIST_ITEM_ROLE
    {
        StartRole = Qt::DisplayRole+1,
        EndRole,
        StartTextRole,
        TimeTextRole
    };
    Session* m_session=nullptr;
    Segment* m_segment=nullptr;

    QVector<SearchData> m_data;
    qint32 m_count=0, m_column=3;
    mutable QMutex m_countMutex;
    qint32 m_state=0;
    qint64 m_start=0, m_end=0;
    QString m_searchText="";
    QVector<SearchData> m_sortData;
    mutable QMutex m_sortDataMutex;
    qint32 m_sortCount=0;
    BufferSet m_set;
    QHash<int,QByteArray> m_roleName;
};

#endif // SEARCH_TABLE_MODEL_H
