#include "ffpixformat.h"

FFPixFormat::FFPixFormat(QString name, QString prettyName, int numComponents, int bitsPerPixel, QObject *parent)  : FFBaseObject(name, prettyName, parent)
{
    _abilities =  Abilities(0);
    _numComponents = numComponents;
    _bitsPerPixel = bitsPerPixel;

    _bitsPerComponent = _bitsPerPixel;
    if (_numComponents > 0) _bitsPerComponent = _bitsPerComponent / _numComponents;
    this->setPrettyName(this->prettyName() + "  - " + QString::number(_bitsPerPixel) + "bits (" + QString::number(_numComponents) + " channels @" + QString::number(_bitsPerComponent) + "bpc)" );
}

FFPixFormat::FFPixFormat(QString name, QString prettyName, int numComponents, int bitsPerPixel, Abilities abilities, QObject *parent)  : FFBaseObject(name, prettyName, parent)
{
    _abilities = abilities;
    _numComponents = numComponents;
    _bitsPerPixel = bitsPerPixel;

    _bitsPerComponent = _bitsPerPixel;
    if (_numComponents > 0) _bitsPerComponent = _bitsPerComponent / _numComponents;
    this->setPrettyName(this->prettyName() + "  - " + QString::number(_bitsPerPixel) + "bits (" + QString::number(_numComponents) + " channels @" + QString::number(_bitsPerComponent) + "bpc)" );
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

int FFPixFormat::bitsPerComponent() const
{
    return _bitsPerComponent;
}
