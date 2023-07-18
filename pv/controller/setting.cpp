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

#include "setting.h"

Setting::Setting(QObject *parent)
    : QObject{parent}
{

}

void Setting::init()
{
    m_transitionObject=DataService::getInstance()->getRoot();
    qint32 count=metaObject()->propertyCount();
    for(qint32 i=0;i<count;i++){
        QMetaProperty temp=metaObject()->property(i);
        if(!temp.isStored() && temp.isValid())
            m_propertyList.append(temp);
    }

    load();
}

void Setting::save()
{
    try {
        if(m_fileName!="" && m_category!="" && !m_isLockSave){
            QString str="";
            QFile setFile(m_fileName);
            if(setFile.exists())
            {
                if(setFile.open(QIODevice::ReadOnly | QIODevice::Text)){
                    QTextStream setStream(&setFile);
                    setStream.setCodec("UTF-8");
                    bool isChildren=false;
                    do{
                        QString line=setStream.readLine();
                        if(line.startsWith("[") && line.endsWith("]")){
                            if(line=="["+m_category+"]")
                                isChildren=true;
                            else
                                isChildren=false;
                        }
                        if(!isChildren && !line.isEmpty()){
                            str.append(line+"\n");
                        }
                    }while(!setStream.atEnd());
                    setFile.close();
                }
                setFile.remove();
            }
            if(setFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)){
                QTextStream setStream(&setFile);
                setStream.setCodec("UTF-8");
                setStream<<str<<"\n"<<"["<<m_category<<"]"<<"\n";
                qint32 count=m_propertyList.count();
                for(qint32 i=0;i<count;i++){
                    QVariant value=property(m_propertyList[i].name());
                    QString write=value.toString();
                    if(QString(value.typeName())=="QJSValue"){
                        QJSValue* js=(QJSValue*)value.data();
                        write=jsonToString(js->toVariant());
                    }
                    setStream<<m_propertyList[i].name()<<"="<<write<<"\n";
                }
                setFile.close();
            }
        }
    } catch (...) {
    }
}

void Setting::load()
{
    try {
        if(m_fileName!="" && m_category!=""){
            QFile setFile(m_fileName);
            if(setFile.exists())
            {
                if(setFile.open(QIODevice::ReadOnly | QIODevice::Text)){
                    QTextStream setStream(&setFile);
                    setStream.setCodec("UTF-8");
                    bool isChildren=false;
                    do{
                        QString str=setStream.readLine();
                        if(str.startsWith("[") && str.endsWith("]")){
                            if(str=="["+m_category+"]")
                                isChildren=true;
                            else if(isChildren)
                                break;
                        }else if(isChildren)
                        {
                            if(str.contains("=")){
                                QStringList list=str.split("=");
                                if(list.count()==2 && !list[1].isEmpty()){
                                    for(auto &i : m_propertyList){
                                        if(i.name()==list[0]){
                                            QVariant val;
                                            if(list[1].left(1)=="["&&list[1].right(1)=="]")
                                                val.setValue(StringToJsonArray(list[1]));
                                            else if(list[1].left(1)=="{"&&list[1].right(1)=="}")
                                                val.setValue(StringToJson(list[1]));
                                            else if(list[1].toLower()=="true")
                                                val.setValue(true);
                                            else if(list[1].toLower()=="false")
                                                val.setValue(false);
                                            else
                                                val.setValue(list[1]);
                                            setProperty(i.name(),val);
                                            i.notifySignal().invoke(this, Qt::AutoConnection);
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }while(!setStream.atEnd());
                    setFile.close();
                }
            }
        }
    } catch (...) {
    }
}

bool Setting::islock()
{
    return m_isLockSave;
}

void Setting::lockSave()
{
    m_isLockSave=true;
}

void Setting::unlockSave()
{
    m_isLockSave=false;
}

QString Setting::jsonToString(QVariant val)
{
    QVariant ret;
    QMetaObject::invokeMethod(m_transitionObject, "jsonToString", Q_RETURN_ARG(QVariant, ret), Q_ARG(QVariant, val));
    return ret.toString();
}

QString Setting::fileName() const
{
    return m_fileName;
}

void Setting::setFileName(const QString &newFileName)
{
    if (m_fileName == newFileName)
        return;
    m_fileName = newFileName;
    emit fileNameChanged();
}

QString Setting::category() const
{
    return m_category;
}

void Setting::setCategory(const QString &newCategory)
{
    if (m_category == newCategory)
        return;
    m_category = newCategory;
    emit categoryChanged();
}
