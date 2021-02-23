#include "fflut.h"

FFLut::FFLut(QString name, QString prettyName, QString inputTrc, QString outputTrc, QObject *parent):
        FFBaseObject(name, prettyName, parent)
{
    _inputPrimaries = "";
    _outputPrimaries = "";
    _inputTrc = inputTrc;
    _outputTrc = outputTrc;
    _type = OneD;
}

FFLut::Type FFLut::type() const
{
    return _type;
}

void FFLut::setType(const Type &type)
{
    _type = type;
}

QString FFLut::outputTrc() const
{
    return _outputTrc;
}

void FFLut::setOutputTrc(const QString &outputTrc)
{
    _outputTrc = outputTrc;
}

QString FFLut::inputTrc() const
{
    return _inputTrc;
}

void FFLut::setInputTrc(const QString &inputTrc)
{
    _inputTrc = inputTrc;
}

QString FFLut::outputPrimaries() const
{
    return _outputPrimaries;
}

void FFLut::setOutputPrimaries(const QString &outputPrimaries)
{
    _outputPrimaries = outputPrimaries;
}

QString FFLut::inputPrimaries() const
{
    return _inputPrimaries;
}

void FFLut::setInputPrimaries(const QString &inputPrimaries)
{
    _inputPrimaries = inputPrimaries;
}
