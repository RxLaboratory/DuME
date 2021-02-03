#include "ffpixformat.h"
#include <QtDebug>

FFPixFormat::FFPixFormat(QString name, QString prettyName, int numComponents, int bitsPerPixel, QObject *parent)  : FFBaseObject(name, prettyName, parent)
{
    _abilities =  Abilities(nullptr);
    _numComponents = numComponents;
    _bitsPerPixel = bitsPerPixel;

    init();
}

FFPixFormat::FFPixFormat(QString name, QString prettyName, int numComponents, int bitsPerPixel, Abilities abilities, QObject *parent)  : FFBaseObject(name, prettyName, parent)
{
    _abilities = abilities;
    _numComponents = numComponents;
    _bitsPerPixel = bitsPerPixel;

    init();
}

void FFPixFormat::init()
{
    _yuvComponentsDistribution = "";

    //Check Alpha
    if (this->name().indexOf("a") >= 0 && _numComponents > 1) _hasAlpha = true;
    else _hasAlpha = false;

    //Check space
    if ( ( this->name().indexOf("rgb") >= 0 || this->name().indexOf("bgr") >= 0 ) && _numComponents >= 3) _colorSpace = RGB;
    else if (this->name().indexOf("yuv") >= 0 && _numComponents >= 3)
    {
        _colorSpace = YUV;
        if (this->name().indexOf("444") >= 0) _yuvComponentsDistribution = "444";
        else if (this->name().indexOf("440") >= 0) _yuvComponentsDistribution = "440";
        else if (this->name().indexOf("422") >=0) _yuvComponentsDistribution = "422";
        else if (this->name().indexOf("420") >= 0) _yuvComponentsDistribution = "420";
        else if (this->name().indexOf("411") >= 0) _yuvComponentsDistribution = "411";
        else if (this->name().indexOf("410") >= 0) _yuvComponentsDistribution = "410";
    }
    else _colorSpace = OTHER;

    //Computes bpc and set pretty name
    _bitsPerChannel = 0;
    if ( _colorSpace == YUV && _yuvComponentsDistribution != "")
    {
        //separate
        int y = QString(_yuvComponentsDistribution.at(0)).toInt();
        int u = QString(_yuvComponentsDistribution.at(1)).toInt();
        int v = QString(_yuvComponentsDistribution.at(2)).toInt();
        int t = y + u + v;
        _bitsPerChannel = _bitsPerPixel * y / t;
        this->setPrettyName(this->prettyName() + " (YUV - " + QString::number(_bitsPerChannel) + "bits - " + QString::number(_numComponents) + " channels)" );
    }
    else if (_colorSpace == RGB)
    {
        _bitsPerChannel = _bitsPerPixel / _numComponents;
        this->setPrettyName(this->prettyName() + " (RGB - " + QString::number(_bitsPerChannel) + "bpc - " + QString::number(_numComponents) + " channels)" );
    }
    else
    {
        this->setPrettyName(this->prettyName() + " (" + QString::number(_bitsPerPixel) + " bits - " + QString::number(_numComponents) + " channels)" );
    }
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

int FFPixFormat::bitsPerChannel() const
{
    return _bitsPerChannel;
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

FFPixFormat *FFPixFormat::getDefault( QObject *parent )
{
    return new FFPixFormat( "", "Default", 0, 0, parent );
}

QString FFPixFormat::yuvComponentsDistribution() const
{
    return _yuvComponentsDistribution;
}

void FFPixFormat::setAlpha(bool hasAlpha)
{
    _hasAlpha = hasAlpha;
}
