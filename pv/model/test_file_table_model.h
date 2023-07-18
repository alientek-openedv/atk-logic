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

#ifndef TEST_FILE_TABLE_MODEL_H
#define TEST_FILE_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QDir>
#include <QCoreApplication>
#include "qmath.h"

struct TestFileInfo
{
    QString FileName;
    QString FilePath;
};

class TestFileTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TestFileTableModel(QObject *parent = nullptr);
    Q_INVOKABLE void refresh();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    void setCount(qint32 count);

private:
    enum LIST_ITEM_ROLE
    {
        FileNameRole = Qt::DisplayRole+1,
        FilePathRole,
    };
    qint32 m_count=0;
    QVector<TestFileInfo> m_data;
    QHash<int,QByteArray> m_roleName;
};

#endif // TEST_FILE_TABLE_MODEL_H
