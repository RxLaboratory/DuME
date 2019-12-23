#include "ffbaseobject.h"

FFBaseObject::FFBaseObject(QString name, QString prettyName, QObject *parent)  : FFObject(parent)
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

QString FFBaseObject::name()
{
    return _name;
}

QString FFBaseObject::prettyName()
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
