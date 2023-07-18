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
    QVector<atk_decoder*> m_decoderList;
    QMap<QString, QString> m_decodeDoc;
};

#endif 
