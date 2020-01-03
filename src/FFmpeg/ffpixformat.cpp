#include "ffpixformat.h"
#include <QtDebug>

FFPixFormat::FFPixFormat(QString name, QString prettyName, int numComponents, int bitsPerPixel, QObject *parent)  : FFBaseObject(name, prettyName, parent)
{
    _abilities =  Abilities(nullptr);
    _numComponents = numComponents;
    _bitsPerPixel = bitsPerPixel;

    this->setPrettyName(this->prettyName() + " (" + QString::number(_bitsPerPixel) + " bits - " + QString::number(_numComponents) + " channels)" );

    if (this->name().indexOf("a") >= 0 && _numComponents > 1) _hasAlpha = true;
    else _hasAlpha = false;

    if ( ( this->name().indexOf("rgb") >= 0 || this->name().indexOf("bgr") >= 0 ) && _numComponents >= 3) _colorSpace = RGB;
    else if (this->name().indexOf("yuv") >= 0 && _numComponents >= 3) _colorSpace = YUV;
    else _colorSpace = OTHER;
}

FFPixFormat::FFPixFormat(QString name, QString prettyName, int numComponents, int bitsPerPixel, Abilities abilities, QObject *parent)  : FFBaseObject(name, prettyName, parent)
{
    _abilities = abilities;
    _numComponents = numComponents;
    _bitsPerPixel = bitsPerPixel;

    this->setPrettyName(this->prettyName() + " (" + QString::number(_bitsPerPixel) + "bits - " + QString::number(_numComponents) + " channels)" );

    if (this->name().indexOf("a") >= 0 && _numComponents > 1) _hasAlpha = true;
    else _hasAlpha = false;

    if (this->name().indexOf("rgb") >= 0 && _numComponents >= 3) _colorSpace = RGB;
    else if (this->name().indexOf("yuv") >= 0 && _numComponents >= 3) _colorSpace = YUV;
    else _colorSpace = OTHER;
}

void FFPixFormat::setAbilities(Abilities &abilities)
{
    _abilities = abilities;
}

void FFPixFormat::setInput(bool input)
{
    _abilities.setFlag(Input,input);
}

void FFPixFormat::setOutput(bool output)
{
    _abilities.setFlag(Input,output);
}

void FFPixFormat::setHardware(bool hardware)
{
    _abilities.setFlag(Hardware,hardware);
}

void FFPixFormat::setPaletted(bool paletted)
{
    _abilities.setFlag(Paletted,paletted);
}

void FFPixFormat::setBitstream(bool bitstream)
{
    _abilities.setFlag(Bitstream,bitstream);
}

bool FFPixFormat::isInput()
{
    return _abilities.testFlag(Input);
}

bool FFPixFormat::isOutput()
{
    return _abilities.testFlag(Output);
}

int FFPixFormat::bitsPerPixel() const
{
    return _bitsPerPixel;
}

int FFPixFormat::numComponents() const
{
    return _numComponents;
}

bool FFPixFormat::hasAlpha() const
{
    return _hasAlpha;
}

FFPixFormat::ColorSpace FFPixFormat::colorSpace() const
{
    return _colorSpace;
}

void FFPixFormat::setAlpha(bool hasAlpha)
{
    _hasAlpha = hasAlpha;
}
