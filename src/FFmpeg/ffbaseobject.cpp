#include "ffbaseobject.h"

FFBaseObject::FFBaseObject(QString name, QObject *parent)  : QObject(parent)
{
    _name = name;
    _prettyName = name;
}

FFBaseObject::FFBaseObject(QString name, QString prettyName, QObject *parent)  : QObject(parent)
{
    _name = name;
    if (prettyName == "")
    {
        _prettyName = name;
    }
    else
    {
        _prettyName = prettyName;
    }
}

QString FFBaseObject::name() const
{
    return _name;
}

QString FFBaseObject::prettyName() const
{
    return _prettyName;
}

void FFBaseObject::setName(const QString &name)
{
    _name = name;
}

void FFBaseObject::setPrettyName(const QString &prettyName)
{
    _prettyName = prettyName;
}

QJsonObject FFBaseObject::toJson()
{
    QJsonObject obj;
    obj.insert("name", _name);
    obj.insert("prettyName", _prettyName);
    return obj;
}
