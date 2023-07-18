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

#include "decode_service.h"

DecodeService::DecodeService()
{

}

qint32 DecodeService::Init()
{
    atk_GSList* l = (atk_GSList*)atk_decoder_list();
    if (l == NULL)
        return 3;
    else{
        decodeInit(l);
        return 0;
    }
}

QJsonObject DecodeService::getDecodeJson()
{
    return m_decodeJson;
}

QVector<atk_decoder *> *DecodeService::getDecodeList()
{
    return &m_decoderList;
}

QString DecodeService::getDecodeDoc(QString id)
{
    return m_decodeDoc[id];
}

void DecodeService::decodeInit(atk_GSList *data)
{
    while(m_decodeJson.size()>0)
        m_decodeJson.erase(m_decodeJson.begin());
    QJsonArray arrayList;
    while(data != NULL) {
        atk_decoder* d = (atk_decoder*)data->data;
        QJsonObject json;
        bool isFirst=false;
        json["id"]=d->id;
        json["name"]=d->name;
        for (const atk_GSList* l = d->inputs; l; l = l->next)
        {
            char* cBuffer = new char[256];
            strcpy(cBuffer, (char*)l->data);
            QString str = cBuffer;
            delete[] cBuffer;
            json["input"]=str;
            isFirst=str=="logic";
            break;
        }
        for (const atk_GSList* l = d->outputs; l; l = l->next)
        {
            char* cBuffer = new char[256];
            strcpy(cBuffer, (char*)l->data);
            QString str = cBuffer;
            delete[] cBuffer;
            json["output"]=str;
            break;
        }
        m_decoderList.append(d);
        QString temp=atk_decoder_doc_get(d);
        if(temp.at(0)=='\n')
            temp=temp.right(temp.size()-1);
        if(temp.right(1)=='\n')
            temp=temp.left(temp.size()-1);
        m_decodeDoc[d->id]=temp;
        json["first"]=isFirst;
        {
            QJsonArray array;
            for (const atk_GSList* i = d->channels; i; i = i->next)
            {
                const atk_channel* channel = (const atk_channel*)i->data;
                QJsonObject json2;
                json2["id"]=channel->id;
                json2["name"]=channel->name;
                json2["desc"]=channel->desc;
                array.append(json2);
            }
            json["channels"]=array;
        }
        {
            QJsonArray array;
            for (const atk_GSList* i = d->opt_channels; i; i = i->next)
            {
                const atk_channel* channel = (const atk_channel*)i->data;
                QJsonObject json2;
                json2["id"]=channel->id;
                json2["name"]=channel->name;
                json2["desc"]=channel->desc;
                array.append(json2);
            }
            json["opt_channels"]=array;
        }
        {
            QJsonArray array;
            for (const atk_GSList* i = d->options; i; i = i->next)
            {
                const atk_decoder_option* opt = (const atk_decoder_option*)i->data;
                QJsonObject json2;
                json2["id"]=opt->id;
                json2["desc"]=opt->desc;
                char* pstr=atk_get_gvariant_value(opt->def);
                json2["type"]=atk_get_gvariant_type(opt->def);
                json2["def"]=QString(pstr);
                atk_get_gvariant_free(pstr);
                if (opt->values)
                {
                    QJsonArray array2;
                    for (atk_GSList* j = opt->values; j; j = j->next)
                    {
                        QJsonObject temp;
                        char* pstr=atk_get_gvariant_value(j->data);
                        temp["data"]=QString(pstr);
                        atk_get_gvariant_free(pstr);
                        array2.append(temp);
                    }
                    json2["values"]=array2;
                }
                array.append(json2);
            }
            json["options"]=array;
        }
        {
            QJsonArray array;
            for (const atk_GSList* i = d->annotations; i; i = i->next)
            {
                const srd_decoder_annotation* ann = (const srd_decoder_annotation*)i->data;
                QJsonObject json2;
                json2["desc"]=ann->desc;
                json2["id"]=ann->id;
                array.append(json2);
            }
            json["annotations"]=array;
        }
        {
            QJsonArray array;
            for (const atk_GSList* i = d->annotation_rows; i; i = i->next)
            {
                const atk_decoder_annotation_row* ann = (const atk_decoder_annotation_row*)i->data;
                QJsonObject json2;
                json2["id"]=ann->id;
                json2["desc"]=ann->desc;
                {
                    QJsonArray array2;
                    for (atk_GSList* j = ann->ann_classes; j; j = j->next)
                    {
                        quint32 value;
                        memcpy(&value, &j->data, 4);
                        QJsonObject js;
                        js["data"]=QString::number(value);
                        array2.append(js);
                    }
                    json2["ann_classes"]=array2;
                }
                array.append(json2);
            }
            json["annotation_rows"]=array;
        }
        arrayList.append(json);
        data = data->next;
    }
    m_decodeJson["data"]=arrayList;
    refreshParent();
}

void DecodeService::refreshParent()
{
    QJsonArray array=m_decodeJson["data"].toArray();
    for(QJsonArray::Iterator it = array.begin(); it != array.end(); it++)
    {
        QJsonObject obj = it[0].toObject();
        bool isSet=false;
        if(obj["input"]=="logic"){
            obj["parent"]="logic";
            isSet=true;
        }else{
            for(auto ii : array){
                if(obj["input"]==ii.toObject()["output"]&&ii.toObject()["input"]!=ii.toObject()["output"]&&ii.toObject()["name"]!="cJTAG"){
                    obj["parent"]=ii.toObject()["name"];
                    isSet=true;
                    break;
                }
            }
        }
        if(!isSet)
            obj["parent"]="NULL";
        it[0] = obj;
    }
    m_decodeJson["data"]=array;
}
