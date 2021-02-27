#include "ffcoloritem.h"

FFColorItem::FFColorItem(QString name, QObject *parent) : FFBaseObject(name, parent)
{
    _outputZScaleName="";
    _inputZScaleName="";
    _outputCSScaleName="";
    _inputCSScaleName="";
    _outputGScaleName="";
    _inputGScaleName="";
    _outputLScaleName="";
    _inputLScaleName="";
    _defaultScaleFilter = None;
    _abilities.setFlag(Input,true);
    _abilities.setFlag(Output,true);
    _metadataName = name;
}

FFColorItem::FFColorItem(QString name, QString prettyName, QObject *parent) : FFBaseObject(name, prettyName, parent)
{
    _outputZScaleName="";
    _inputZScaleName="";
    _outputCSScaleName="";
    _inputCSScaleName="";
    _outputGScaleName="";
    _inputGScaleName="";
    _outputLScaleName="";
    _inputLScaleName="";
    _defaultScaleFilter = None;
    _abilities.setFlag(Input,true);
    _abilities.setFlag(Output,true);
    _metadataName = name;
}

FFColorItem::FFColorItem(QString name, QString prettyName, QString outputScaleName, ScaleFilter scaleFilter, QObject *parent) : FFBaseObject(name, prettyName, parent)
{
    _outputZScaleName="";
    _inputZScaleName="";
    _outputCSScaleName="";
    _inputCSScaleName="";
    _outputGScaleName="";
    _inputGScaleName="";
    _outputLScaleName="";
    _inputLScaleName="";

    _defaultScaleFilter = scaleFilter;

    if (scaleFilter == ZScale){
        _outputZScaleName = outputScaleName;
        _inputZScaleName = outputScaleName;
    }
    else if (scaleFilter == Colorspace)
    {
        _outputCSScaleName = outputScaleName;
        _inputCSScaleName = outputScaleName;
    }
    else if (scaleFilter == Gamma)
    {
        _outputGScaleName = outputScaleName;
        _inputGScaleName = outputScaleName;
    }
    else if (scaleFilter == LUT)
    {
        _outputLScaleName = outputScaleName;
        _inputLScaleName = outputScaleName;
    }

    _abilities.setFlag(Input,true);
    _abilities.setFlag(Output,true);
    _metadataName = name;
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

FFColorItem::ScaleFilter FFColorItem::selectOutputFilter(FFColorItem *inputItem)
{
    if (!inputItem) return _defaultScaleFilter;

    if (defaultScaleFilter() == ZScale && inputItem->inputZScaleName() != "") return ZScale;

    if (defaultScaleFilter() == Colorspace && inputItem->inputCSScaleName() != "") return Colorspace;

    if (defaultScaleFilter() == Gamma && inputItem->inputZScaleName() != "") return Gamma;

    if (defaultScaleFilter() == LUT) return LUT;

    if (_outputZScaleName != "" && inputItem->inputZScaleName() != "") return ZScale;

    if (_outputCSScaleName != "" && inputItem->inputCSScaleName() != "" ) return Colorspace;

    if (_outputGScaleName != "" && inputItem->inputZScaleName() != "") return Gamma;

    if (_outputZScaleName != "") return ZScale;

    if (_outputCSScaleName != "") return Colorspace;

    if (_outputGScaleName != "") return Gamma;

    if (_outputLScaleName != "") return LUT;

    return None;
}

QString FFColorItem::outputZScaleName() const
{
    return _outputZScaleName;
}

void FFColorItem::setOutputZScaleName(const QString &zscaleName)
{
    _outputZScaleName = zscaleName;
}

FFColorItem::ScaleFilter FFColorItem::defaultScaleFilter() const
{
    return _defaultScaleFilter;
}

void FFColorItem::setDefaultScaleFilter(const ScaleFilter &scaleFilter)
{
    _defaultScaleFilter = scaleFilter;
}

QString FFColorItem::metadataName() const
{
    return _metadataName;
}

void FFColorItem::setMetadataName(const QString &metadataName)
{
    _metadataName = metadataName;
}

QString FFColorItem::outputGScaleName() const
{
    return _outputGScaleName;
}

void FFColorItem::setOutputGScaleName(const QString &outputGScaleName)
{
    _outputGScaleName = outputGScaleName;
}

QString FFColorItem::inputGScaleName() const
{
    return _inputGScaleName;
}

void FFColorItem::setInputGScaleName(const QString &inputGScaleName)
{
    _inputGScaleName = inputGScaleName;
}

QString FFColorItem::outputLScaleName() const
{
    return _outputLScaleName;
}

void FFColorItem::setOutputLScaleName(const QString &outputLScaleName)
{
    _outputLScaleName = outputLScaleName;
}

QString FFColorItem::inputLScaleName() const
{
    return _inputLScaleName;
}

void FFColorItem::setInputLScaleName(const QString &inputLScaleName)
{
    _inputLScaleName = inputLScaleName;
}

QString FFColorItem::inputZScaleName() const
{
    return _inputZScaleName;
}

void FFColorItem::setInputZScaleName(const QString &inputScaleName)
{
    _inputZScaleName = inputScaleName;
}

QString FFColorItem::outputCSScaleName() const
{
    return _outputCSScaleName;
}

void FFColorItem::setOutputCSScaleName(const QString &outputCSScaleName)
{
    _outputCSScaleName = outputCSScaleName;
}

QString FFColorItem::inputCSScaleName() const
{
    return _inputCSScaleName;
}

void FFColorItem::setInputCSScaleName(const QString &inputCSScaleName)
{
    _inputCSScaleName = inputCSScaleName;
}
