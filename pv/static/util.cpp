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

#include "util.h"

QString getRandString(qint32 len)
{
    QString tmp = QString("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWZYZ");
    QString str;
    for(int i=0;i<len;i++) {
        qint32 ir = QRandomGenerator::global()->generate()%tmp.length();
        str.append(tmp.at(ir));
    }
    return str;
}

qint64 frequencyToHz(double num, QString unit) {
    if (unit=="Hz")
        return num;
    else if (unit=="kHz")
        return num * 1000;
    else if (unit=="MHz")
        return num * 1000000;
    else if (unit=="GHz")
        return num * 1000000000;
    return num;
}

qint64 unitToBit(double num, QString unit) {
    if (unit=="B")
        return num;
    else if (unit=="K")
        return num * 1000;
    else if (unit=="M")
        return num * 1000000;
    else if (unit=="G")
        return num * 1000000000;
    return num;
}

QString nsToShowStr(double time, int decimal)
{
    QString refStr="";
    QString unitList[]={"ns","μs","ms","s"};
    QRegExp reg;
    reg.setPattern("(\\.){0,1}0+$");
    for(qint32 i=0;i<4;i++){
        if(time>=1000 && i<3)
            time/=1000;
        else
        {
            refStr=QString::number(time, 'f', decimal).replace(reg,"")+" "+unitList[i];
            break;
        }
    }
    return refStr;
}

QString hzToShowStr(double hz, int decimal)
{
    QString refStr="";
    QString unitList[]={"Hz","KHz","MHz","GHz"};
    QRegExp reg;
    reg.setPattern("(\\.){0,1}0+$");
    for(qint32 i=0;i<4;i++){
        if(hz>=1000 && i<3)
            hz/=1000;
        else
        {
            refStr=QString::number(hz, 'f', decimal).replace(reg,"")+" "+unitList[i];
            break;
        }
    }
    return refStr;
}

QString bitToShowStr(double bit, int decimal)
{
    QString refStr="";
    QString unitList[]={"B","K","M","G"};
    QRegExp reg;
    reg.setPattern("(\\.){0,1}0+$");
    for(qint32 i=0;i<4;i++){
        if(bit>=1000 && i<3)
            bit/=1000;
        else
        {
            refStr=QString::number(bit, 'f', decimal).replace(reg,"")+" "+unitList[i];
            break;
        }
    }
    return refStr;
}

QString getStringMiddle(QString source, QString before, QString after, int offset) {
    int iBefore = source.indexOf(before, offset);
    if (iBefore == -1)
        return "";
    iBefore += before.size();
    int iAfter = source.indexOf(after, iBefore);
    if (iAfter == -1)
        return "";
    return source.mid(iBefore, iAfter - iBefore);
}

const quint32 table[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
};

quint32 gCRC32(quint8* buf, qint32 nLength)
{
    if (nLength < 1)
        return 0xffffffff;
    quint32 crc = 0;
    for (int i(0); i != nLength; ++i)
    {
        crc = table[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
    }
    crc = crc ^ 0xffffffff;
    return crc;
}

QByteArray intToBytes(quint64 value, qint8 len)
{
    QByteArray src;

    for(qint8 i=0;i<len;i++)
        src.append((uchar)(value >>(i*8) & 0xFF));
    return src;
}

QByteArray triggerStringToByte(QJsonArray channelArray,QString text){
    QByteArray array;
    uchar b=0;
    qint32 ii = 0;
    qint32 index=15;
    for (QChar i : text)
    {
        if(channelArray[index].toObject()["enable"].toBool())
            b+=(uchar)(ii%2==0?128:8);
        if(i=='R')
            b += (uchar)(ii % 2 == 0 ? 16 : 1);
        else if(i=='1')
            b += (uchar)(ii % 2 == 0 ? 64 : 4);
        else if(i=='F')
            b += (uchar)(ii % 2 == 0 ? 32 : 2);
        else if(i=='0')
        {}
        else if(i=='C')
        {
            b += (uchar)(ii % 2 == 0 ? 16 : 1);
            b += (uchar)(ii % 2 == 0 ? 32 : 2);
        }else{
            b += (uchar)(ii % 2 == 0 ? 16 : 1);
            b += (uchar)(ii % 2 == 0 ? 32 : 2);
            b += (uchar)(ii % 2 == 0 ? 64 : 4);
        }
        ii++;
        index--;
        if(index<0)
            index=0;
        if (ii % 2 == 0)
        {
            array.append(b);
            b = 0;
        }
    }
    return array;
}

QString moveDecimal(quint64 num, int decimal)
{
    QString str=QString("%1").arg(num, decimal, 10, QLatin1Char('0'));
    if(num>=qPow(10,decimal))
        str.insert(str.length()-decimal,'.');
    else
        str="0."+str;
    return str;
}

QString reverseString(QString str){
    if(str.isEmpty())
        return "";
    QString sb = "";
    qint32 len=str.length();
    for(int i=len-1; i>=0; i--)
        sb.append(str[i]);
    return sb;
}

bool zipDir(QString& dirPath,QString savePath)
{
    bool  ret;

    QZipWriter *writer = new QZipWriter(savePath);
    if(QZipWriterEx(writer,dirPath,dirPath))
        ret=true;
    else
        ret=false;
    writer->close();
    delete writer;
    return  ret;
}

bool  QZipWriterEx(QZipWriter *writer, QString tmpPath, QString basePath)
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
        }
        else if (info.isDir())
        {
            ret = QZipWriterEx(writer,info.filePath(),basePath);
        }
    }
    return ret;
}

bool unZipDir(QString &filePath, QString savePath)
{
    QDir dir(savePath);
    if (!dir.exists())
        dir.mkpath(savePath);
    bool unzipok = true;
    try {
        QZipReader zipreader(filePath);
        if(zipreader.fileInfoList().size()<=0)
            return false;
        for (int i = 0; i < zipreader.fileInfoList().size(); ++i) {
            QStringList paths = zipreader.fileInfoList().at(i).filePath.split("/");
            paths.removeLast();
            QString path = paths.join("/");
            QDir subdir(savePath + "/" + path);
            if (!subdir.exists())
                dir.mkpath(QString::fromLocal8Bit("%1").arg(savePath + "/" + path));

            QFile file(savePath + "/" + zipreader.fileInfoList().at(i).filePath);
            file.open(QIODevice::WriteOnly);
            QByteArray dt = zipreader.fileInfoList().at(i).filePath.toUtf8();
            QString strtemp = QString::fromLocal8Bit(dt);
            QByteArray array = zipreader.fileData(strtemp);
            file.write(array);
            file.close();
        }
        zipreader.close();
    } catch (...) {
        unzipok=false;
    }
    return unzipok;
}

QString JsonToString(const QJsonObject& json)
{
    return QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

QJsonObject StringToJson(const QString& str)
{
    QJsonObject l_ret;
    QJsonParseError l_err;
    QJsonDocument l_doc = QJsonDocument::fromJson(str.toUtf8(), &l_err);
    if (l_err.error == QJsonParseError::NoError)
    {
        if (l_doc.isObject())
        {
            l_ret = l_doc.object();
        }
    }
    return l_ret;
}

QJsonArray StringToJsonArray(const QString& str)
{
    QJsonDocument document;
    QJsonParseError err;
    document = QJsonDocument::fromJson(str.toUtf8(), &err);

    if (err.error != QJsonParseError::NoError)
        return QJsonArray();
    return QJsonArray(document.array());
}

qint64 GetTickCount_()
{
    return QDateTime::currentDateTime().toMSecsSinceEpoch();
}

void pathRepair(QString &path)
{
    qint32 deleteLen=8;
#if defined (Q_OS_LINUX) || defined (Q_OS_MACX)
    deleteLen--;
#endif
    if(path.left(8)=="file:
        path=path.right(path.length()-deleteLen);
    path=path.replace("\\\\","/");
    path=path.replace("\\","/");
}

void deleteEmptyDir(QString path)
{
    QDir tmp(path);
    tmp.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden); 
    foreach(QFileInfo objFileInfo, tmp.entryInfoList())
    {
        if(objFileInfo.isDir())
        {
            QString qstrSubFilePath = objFileInfo.absoluteFilePath();
            deleteEmptyDir(qstrSubFilePath);
            QDir qdrSubPath(qstrSubFilePath);
            qdrSubPath.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
            QFileInfoList qlstFileInfo =  qdrSubPath.entryInfoList();
            if(qlstFileInfo.count() <= 0)
                qdrSubPath.rmdir(qstrSubFilePath);
        }
    }
}
