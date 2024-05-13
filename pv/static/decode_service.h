#ifndef DECODE_SERVICE_H
#define DECODE_SERVICE_H


#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QMap>
#include <atk_decoder.h>
#include "util.h"


struct srd_decoder_annotation
{
    char* id;
    char* desc;
};

class DecodeService
{
public:
    DecodeService();
    qint32 Init();
    QJsonObject getDecodeJson();
    QVector<atk_decoder*>* getDecodeList();
    QString getDecodeDoc(QString id);

private:
    void decodeInit(atk_GSList* data);
    void refreshParent();

private:
    QJsonObject m_decodeJson;
    QVector<atk_decoder*> m_decoderList;//全局解码库列表
    QMap<QString, QString> m_decodeDoc; //解码库文档列表
};

#endif // DECODE_SERVICE_H
