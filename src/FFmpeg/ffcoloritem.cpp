#include "ffcoloritem.h"

FFColorItem::FFColorItem(QString name, QObject *parent) : FFBaseObject(name, parent)
{
    _scaleName="";
    _scaleFilter = ZScale;
}

FFColorItem::FFColorItem(QString name, QString prettyName, QObject *parent) : FFBaseObject(name, prettyName, parent)
{
    _scaleName="";
    _scaleFilter = ZScale;
}

FFColorItem::FFColorItem(QString name, QString prettyName, QString scaleName, QObject *parent) : FFBaseObject(name, prettyName, parent)
{
    _scaleName = scaleName;
    _scaleFilter = ZScale;
}

FFColorItem::FFColorItem(QString name, QString prettyName, QString scaleName, ScaleFilter scaleFilter, QObject *parent) : FFBaseObject(name, prettyName, parent)
{
    _scaleName = scaleName;
    _scaleFilter = scaleFilter;
}


QString FFColorItem::scaleName() const
{
    return _scaleName;
}

void FFColorItem::setScaleName(const QString &zscaleName)
{
    _scaleName = zscaleName;
}

FFColorItem::ScaleFilter FFColorItem::scaleFilter() const
{
    return _scaleFilter;
}

void FFColorItem::setScaleFilter(const ScaleFilter &scaleFilter)
{
    _scaleFilter = scaleFilter;
}
