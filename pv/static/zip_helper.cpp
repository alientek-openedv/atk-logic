#include "zip_helper.h"

ZipHelper::ZipHelper(QObject *parent)
    : QObject{parent}
{

}

bool ZipHelper::zipDir(QString& dirPath,QString savePath)
{
    bool  ret;
    emit sendZipDirSchedule(0);
    QZipWriter *writer = new QZipWriter(savePath);
    qint32 count=0,index=0;
    GetDirFilesCount(dirPath,count);
    if(QZipWriterEx(writer,dirPath,dirPath,index,count))//dirPath为了方便回调，所以传了两次。
        ret=true;
    else
        ret=false;
    writer->close();
    delete writer;
    emit sendZipDirSchedule(100);
    return  ret;
}

bool ZipHelper::QZipWriterEx(QZipWriter *writer, QString tmpPath, QString basePath, qint32 &index, qint32 count)
{
    QDir dir(tmpPath);
    bool ret=true;
    foreach (QFileInfo info, dir.entryInfoList())
    {
        if (info.fileName() == "." || info.fileName() == ".." || !ret)
            continue;
        if (info.isFile())
        {
            QFile upfile(info.filePath());
            upfile.open(QIODevice::ReadOnly);
            QString fileName = info.filePath().mid(basePath.size()+1,info.filePath().size());
            writer->addFile(fileName,upfile.readAll());
            upfile.close();
            index++;
            emit sendZipDirSchedule(qMin((qreal)index/count*100,99.));
        }
        else if (info.isDir())
        {
            ret = QZipWriterEx(writer,info.filePath(),basePath,index,count);
        }
    }
    return ret;
}

bool ZipHelper::unZipDir(QString &filePath, QString savePath)
{
    // 解压文件夹
    emit sendUnZipDirSchedule(0);
    QDir dir(savePath);
    if (!dir.exists())
        dir.mkpath(savePath);
    bool unzipok = true;
    try {
        QZipReader zipreader(filePath);
        //    unzipok = zipreader.extractAll(savePath);   //可加可不加没有什么影响
        if(zipreader.fileInfoList().size()<=0)
            return false;
        double count=zipreader.fileInfoList().size();
        for (int i = 0; i < count; ++i) {
            QStringList paths = zipreader.fileInfoList().at(i).filePath.split("/");
            paths.removeLast();
            QString path = paths.join("/");
            QDir subdir(savePath + "/" + path);
            if (!subdir.exists())
                dir.mkpath(QString::fromLocal8Bit("%1").arg(savePath + "/" + path));

            QFile file(savePath + "/" + zipreader.fileInfoList().at(i).filePath);
            file.open(QIODevice::WriteOnly);
            //注意编码问题
            QByteArray dt = zipreader.fileInfoList().at(i).filePath.toUtf8();
            QString strtemp = QString::fromLocal8Bit(dt);

            QByteArray array = zipreader.fileData(strtemp);
            file.write(array);
            file.close();
            emit sendUnZipDirSchedule(qMin(i/count*100,99.));
        }
        zipreader.close();
    } catch (...) {
        unzipok=false;
    }
    emit sendUnZipDirSchedule(100);
    return unzipok;
}
