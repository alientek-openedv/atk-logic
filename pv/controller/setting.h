#ifndef SETTING_H
#define SETTING_H


#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QFile>
#include <QDebug>
#include <QJSValue>
#include <QJsonObject>
#include <QJsonDocument>
#include "pv/static/data_service.h"

class Setting : public QObject
{
    Q_OBJECT
public:
    explicit Setting(QObject *parent = nullptr);
    Q_INVOKABLE void init();
    Q_INVOKABLE void save();
    Q_INVOKABLE void load();
    Q_INVOKABLE bool islock();
    Q_INVOKABLE void lockSave();
    Q_INVOKABLE void unlockSave();

private:
    QString jsonToString(QVariant val);

private:
    QString fileName() const;
    void setFileName(const QString &newFileName);

    QString category() const;
    void setCategory(const QString &newCategory);

    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)

signals:
    void fileNameChanged();
    void categoryChanged();

private:
    QString m_fileName;
    QString m_category;
    QVector<QMetaProperty> m_propertyList;
    QObject* m_transitionObject;
    bool m_isLockSave=false;
};

#endif // SETTING_H
