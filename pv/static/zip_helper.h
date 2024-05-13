#ifndef ZIP_HELPER_H
#define ZIP_HELPER_H

#include <QObject>

class ZipHelper : public QObject
{
    Q_OBJECT
public:
    explicit ZipHelper(QObject *parent = nullptr);

    bool zipDir(QString& dirPath, QString savePath);
    bool unZipDir(QString& filePath, QString savePath);
private:
    bool QZipWriterEx(QZipWriter *writer, QString tmpPath, QString basePath, qint32 &index, qint32 count);

signals:
    void sendZipDirSchedule(qint32 schedule);
    void sendUnZipDirSchedule(qint32 schedule);
};

#endif // ZIP_HELPER_H
