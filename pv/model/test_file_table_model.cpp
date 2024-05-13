#include "test_file_table_model.h"
#include "qdebug.h"


TestFileTableModel::TestFileTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_roleName[FileNameRole]="fileName";
    m_roleName[FilePathRole]="filePath";
}

void TestFileTableModel::refresh()
{
    m_data.clear();
    QDir dir(QCoreApplication::applicationDirPath()+"/test");
    if(dir.exists() && !dir.isEmpty()){
        QFileInfoList list=dir.entryInfoList(QDir::Files);
        for(auto &i : list){
            TestFileInfo temp;
            temp.FileName=i.fileName();
            temp.FilePath=i.filePath();
            m_data.append(temp);
        }
    }
    setCount(m_data.count());
    if(m_data.count()>0)
    {
        QModelIndex start_index = createIndex(0, 0);
        QModelIndex end_index = createIndex(qCeil((double)m_count/3), 0);
        emit dataChanged(start_index, end_index);
    }
}

int TestFileTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return qCeil((double)m_count/3);
}

int TestFileTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QHash<int, QByteArray> TestFileTableModel::roleNames() const
{
    return m_roleName;
}

QVariant TestFileTableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row*3+index.column() >= m_count)
        return QVariant("");
    TestFileInfo rowData=m_data[row*3+index.column()];
    switch (role) {
    case FileNameRole:
        return rowData.FileName;
    case FilePathRole:
        return rowData.FilePath;
    }
    return QVariant("");
}

void TestFileTableModel::setCount(qint32 count)
{

    if(count!=m_count){
        qint32 temp=m_count;
        if(count>temp){
            if(temp<=0)
                temp=1;
            beginInsertRows(QModelIndex(),temp-1,count-1);
            m_count=count;
            endInsertRows();
        }else{
            beginRemoveRows(QModelIndex(),0,temp-count-1);
            m_count=count;
            endRemoveRows();
        }
    }
}
