#include "ffcoloritem.h"

FFColorItem::FFColorItem(QString name, QObject *parent) : FFBaseObject(name, parent)
{
    _outputScaleName="";
    _scaleFilter = ZScale;
    _abilities.setFlag(Input,true);
    _abilities.setFlag(Output,true);
    _metadataName = name;
    _inputScaleName = "";
}

FFColorItem::FFColorItem(QString name, QString prettyName, QObject *parent) : FFBaseObject(name, prettyName, parent)
{
    _outputScaleName="";
    _scaleFilter = ZScale;
    _abilities.setFlag(Input,true);
    _abilities.setFlag(Output,true);
    _metadataName = name;
    _inputScaleName = "";
}

FFColorItem::FFColorItem(QString name, QString prettyName, QString outputScaleName, QObject *parent) : FFBaseObject(name, prettyName, parent)
{
    _outputScaleName = outputScaleName;
    _scaleFilter = ZScale;
    _abilities.setFlag(Input,true);
    _abilities.setFlag(Output,true);
    _metadataName = name;
    _inputScaleName = "";
}

FFColorItem::FFColorItem(QString name, QString prettyName, QString outputScaleName, ScaleFilter scaleFilter, QObject *parent) : FFBaseObject(name, prettyName, parent)
{
    _outputScaleName = outputScaleName;
    _scaleFilter = scaleFilter;
    _abilities.setFlag(Input,true);
    _abilities.setFlag(Output,true);
    _metadataName = name;
    _inputScaleName = "";
}

bool FFColorItem::isInput() const
{
    return _abilities.testFlag(Input);
}

bool FFColorItem::isOutput() const
{
    return _abilities.testFlag(Output);
}

void FFColorItem::setInput(bool i)
{
    _abilities.setFlag(Input,i);
}

void FFColorItem::setOutput(bool o)
{
    _abilities.setFlag(Output,o);
}


QString FFColorItem::outputScaleName() const
{
    return _outputScaleName;
}

void FFColorItem::setOutputScaleName(const QString &zscaleName)
{
    _outputScaleName = zscaleName;
}

FFColorItem::ScaleFilter FFColorItem::scaleFilter() const
{
    return _scaleFilter;
}

void FFColorItem::setScaleFilter(const ScaleFilter &scaleFilter)
{
    _scaleFilter = scaleFilter;
}

QString FFColorItem::metadataName() const
{
    return _metadataName;
}

void FFColorItem::setMetadataName(const QString &metadataName)
{
    _metadataName = metadataName;
}

QString FFColorItem::inputScaleName() const
{
    return _inputScaleName;
}

void FFColorItem::setInputScaleName(const QString &inputScaleName)
{
    _inputScaleName = inputScaleName;
}
