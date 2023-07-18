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

#ifndef DECODE_TABLE_MODEL_H
#define DECODE_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QtConcurrent/QtConcurrent>
#include <QJsonArray>
#include <algorithm>
#include "./pv/static/data_service.h"

int cmpData(DecodeRowData&e1,DecodeRowData&e2);

struct SortShow{
    QString name;
    qint32 showIndex;
    qint32 dataPosition;
    qint32 rowPosition;
    SortShow(QString Name, qint32 Index){
        name=Name;
        showIndex=Index;
        dataPosition=-1;
        rowPosition=0;
    }
};

struct searchRow{
    qint32 decodeIndex;
    QJsonArray type;
};

class DecodeTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DecodeTableModel(QObject *parent = nullptr);
    ~DecodeTableModel();
    Q_INVOKABLE void init(QString sessionID);
    Q_INVOKABLE QJsonArray initShow(QString decodeID);
    Q_INVOKABLE void appendShow(QString rowID);
    Q_INVOKABLE void removeShow(QString rowID);
    Q_INVOKABLE void search(const QString &text);
    Q_INVOKABLE void exitSearch();
    Q_INVOKABLE void stopAll(bool isRemove);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void setLineHeight(qint32 lineHeight);
    Q_INVOKABLE qint32 refreshBuffer(qint32 y);
    Q_INVOKABLE qint32 getShowCount();
    Q_INVOKABLE qint32 getShowFirst();
    Q_INVOKABLE qint32 scrollPosition(double position);
    Q_INVOKABLE void setComplete(bool isComplete);
    Q_INVOKABLE void resetSort();
    Q_INVOKABLE void saveTableReady(QJSValue val);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;


    bool isExport() const;
    Q_PROPERTY(bool isExport READ isExport NOTIFY isExportChanged)

signals:
    void modelDataChanged();
    void isExportChanged();

private:
    void setCount(qint32 count);
    void refreshBuffer();
    void sortData();

private:
    enum LIST_ITEM_ROLE
    {
        TextRole = Qt::DisplayRole+1,
        StartTextRole,
        StartRole,
        EndRole,
        TimeTextRole
    };
    Session* m_session;
    mutable QMutex m_runStateMutex;
    qint32 m_isRun=0;
    bool m_isNext=false;
    QHash<QString,DecodeController*>* m_decodeList=nullptr;
    DecodeController* m_decode=nullptr;
    bool m_isComplete=false;
    QVector<QString> m_showRowList;
    QVector<DataOrder>* m_orderList;
    QVector<SortShow> m_sortPositionList;
    QVector<DecodeRowData> m_sortData;
    mutable QMutex m_sortDataMutex;
    qint32 m_sortCount=0;
    QVector<DecodeRowData> m_searchData;
    QVector<DecodeRowData> m_data;
    QVector<QMap<QString,QJsonObject>> m_rowType;
    QString m_searchText="";
    bool m_isSearch=false;
    bool m_isStop=false;
    qint32 m_count=0, m_column=4;
    mutable QMutex m_countMutex;
    QHash<int,QByteArray> m_roleName;
    BufferSet m_set;
};

#endif // DECODE_TABLE_MODEL_H
