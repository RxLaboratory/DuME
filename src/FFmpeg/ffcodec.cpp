#include "ffcodec.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif


FFCodec::FFCodec(QString name, QString prettyName, QObject *parent)  : FFBaseObject(name, prettyName, parent)
{
    _abilities =  Abilities(0);

    setQualityParam();
    setDefaultPixFormat( );
    setSpeedParam();
}

FFCodec::FFCodec(QString name, QString prettyName, Abilities abilities, QObject *parent)  : FFBaseObject(name, prettyName, parent)
{
    _abilities = abilities;

    setQualityParam();
    setDefaultPixFormat( );
    setSpeedParam();
}

bool FFCodec::isVideo() const
{
    return _abilities.testFlag(Video);
}

bool FFCodec::isAudio() const
{
    return _abilities.testFlag(Audio);
}

bool FFCodec::isEncoder() const
{
    return _abilities.testFlag(Encoder);
}

bool FFCodec::isDecoder() const
{
    return _abilities.testFlag(Decoder);
}

bool FFCodec::isLossy() const
{
    return _abilities.testFlag(Lossy);
}

bool FFCodec::isLossless() const
{
    return _abilities.testFlag(Lossless);
}

bool FFCodec::isIframe() const
{
    return _abilities.testFlag(IFrame);
}

void FFCodec::setDecoder(bool decoder)
{
    _abilities.setFlag(Decoder,decoder);
}

void FFCodec::setEncoder(bool encoder)
{
    _abilities.setFlag(Encoder,encoder);
}

void FFCodec::setAudio(bool audio)
{
    _abilities.setFlag(Audio,audio);
}

void FFCodec::setVideo(bool video)
{
    _abilities.setFlag(Video,video);
}

void FFCodec::setLossy(bool lossy)
{
    _abilities.setFlag(Lossy,lossy);
}

void FFCodec::setLossless(bool lossless)
{
    _abilities.setFlag(Lossless,lossless);
}

void FFCodec::setIframe(bool iframe)
{
    _abilities.setFlag(IFrame,iframe);
}

void FFCodec::setAbilities(const Abilities &abilities)
{
    _abilities = abilities;
}

QList<FFPixFormat *> FFCodec::pixFormats() const
{
    return _pixFormats;
}

void FFCodec::addPixFormat(FFPixFormat *pixFormat)
{
    _pixFormats << pixFormat;
}

FFPixFormat *FFCodec::defaultPixFormat() const
{
    return _defaultPixFormat;
}

FFPixFormat *FFCodec::defaultPixFormat(bool withAlpha)
{
    return pixFormatWithAlpha( _defaultPixFormat, withAlpha);
}

FFPixFormat *FFCodec::pixFormatWithAlpha(FFPixFormat *pf, bool alpha)
{
    if (pf == nullptr) return FFPixFormat::getDefault( this );

    if ( pf->hasAlpha() != alpha )
    {
        //find the closest one with(out) alpha
        foreach ( FFPixFormat *p, _pixFormats )
        {
            if ( p->hasAlpha() == alpha )
                if ( p->isOutput() == pf->isOutput() )
                    if (p->isInput() == pf->isInput())
                        if (p->colorSpace() == pf->colorSpace())
                            if ( (alpha && p->numComponents()+1 == pf->numComponents()) || (!alpha && p->numComponents() == pf->numComponents()+1) )
                                if( p->bitsPerPixel() / p->numComponents() == pf->bitsPerPixel() / pf->numComponents() )
                                {
                                    return p;
                                }
        }
        foreach ( FFPixFormat *p, _pixFormats )
        {
            if ( p->hasAlpha() == alpha )
                if ( p->isOutput() == pf->isOutput() )
                    if (p->isInput() == pf->isInput())
                        if (p->colorSpace() == pf->colorSpace())
                            if ( (alpha && p->numComponents()+1 == pf->numComponents()) || (!alpha && p->numComponents() == pf->numComponents()+1) )
                            {
                                return p;
                            }

        }
        foreach ( FFPixFormat *p, _pixFormats )
        {
            if ( p->hasAlpha() == alpha )
                if ( p->isOutput() == pf->isOutput() )
                    if (p->isInput() == pf->isInput())
                        if (p->colorSpace() == pf->colorSpace())
                        {
                            return p;
                        }
        }
        foreach ( FFPixFormat *p, _pixFormats )
        {
            if ( p->hasAlpha() == alpha )
                if ( p->isOutput() == pf->isOutput() )
                    if (p->isInput() == pf->isInput())
                    {
                        return p;
                    }
        }
        foreach ( FFPixFormat *p, _pixFormats )
        {
            if ( p->hasAlpha() == alpha )
                if ( p->isOutput() == pf->isOutput() )
                {
                    return p;
                }
        }
        foreach ( FFPixFormat *p, _pixFormats )
        {
            if ( p->hasAlpha() == alpha )
            {
                return p;
            }
        }
    }
    return pf;
}

void FFCodec::setDefaultPixFormat(FFPixFormat *defaultPixFormat)
{
    _defaultPixFormat = defaultPixFormat;
}

void FFCodec::setDefaultPixFormat( )
{
    _defaultPixFormat = FFPixFormat::getDefault(this);
}

QList<FFProfile *> FFCodec::profiles() const
{
    return _profiles;
}

void FFCodec::addProfile(FFProfile *profile)
{
    _profiles << profile;
}

QString FFCodec::qualityParam() const
{
    return _qualityParam;
}

QString FFCodec::qualityValue(int quality)
{
    if (_name == "h264")
    {
        quality = 100-quality;
        //adjust to CRF values
        if (quality < 10) quality = Interpolations::linear( quality, 0, 10, 0, 15 );
        else if (quality < 25) quality = Interpolations::linear( quality, 10, 25, 15, 21 );
        else if (quality < 50) quality = Interpolations::linear( quality, 25, 50, 21, 28 );
        else if (quality < 75) quality = Interpolations::linear( quality, 50, 75, 28, 34 );
        else quality = Interpolations::linear( quality, 75, 100, 35, 51 );

        return QString::number(quality);
    }

    if (_name.indexOf("prores") >= 0)
    {
        //0-32
        quality = 100-quality;
        //excellent 0-5
        //very good 5-9
        //good 9-13
        //bad 14-23
        //very bad 24-32
        if (quality < 10) quality = Interpolations::linear( quality, 0, 10, 0, 5 );
        else if (quality < 25) quality = Interpolations::linear( quality, 10, 25, 5, 9 );
        else if (quality < 50) quality = Interpolations::linear( quality, 25, 50, 9, 13 );
        else if (quality < 75) quality = Interpolations::linear( quality, 50, 75, 13, 23 );
        else quality = Interpolations::linear( quality, 75, 100, 23, 32 );

        return QString::number(quality);
    }

    if (_name.indexOf("jpg") >= 0 || _name.indexOf("jpeg") >= 0)
    {
        //1-32
        quality = 100-quality;
        //excellent 1-4
        //very good 4-8
        //good 8-12
        //bad 12-20
        //very bad 20-32
        if (quality < 10) quality = Interpolations::linear( quality, 0, 10, 0, 4 );
        else if (quality < 25) quality = Interpolations::linear( quality, 10, 25, 4, 8 );
        else if (quality < 50) quality = Interpolations::linear( quality, 25, 50, 8, 12 );
        else if (quality < 75) quality = Interpolations::linear( quality, 50, 75, 12, 20 );
        else quality = Interpolations::linear( quality, 75, 100, 20, 32 );

        return QString::number(quality);
    }
    return "";
}

QString FFCodec::speedParam() const
{
    return _speedParam;
}

QString FFCodec::speedValue(int speed)
{
    if (_name == "h264")
    {
        if (speed >= 90) return "ultrafast";
        else if (speed >= 80) return "superfast";
        else if (speed >= 70) return "veryfast";
        else if (speed >= 60) return "faster";
        else if (speed >= 50) return "fast";
        else if (speed >= 40) return "medium";
        else if (speed >= 30) return "slow";
        else if (speed >= 20) return "slower";
        else return "veryslow";
    }

    return "";
}

FFCodec *FFCodec::getDefault( QObject* parent )
{
    FFCodec *c = new FFCodec( "", "Default", FFCodec::Audio | FFCodec::Video | FFCodec::Encoder | FFCodec::Lossless | FFCodec::Lossy | FFCodec::IFrame , parent);
    return c;
}

void FFCodec::setQualityParam()
{
    if (_name == "h264")
    {
        _qualityParam = "-crf";
    }
    else if (_name.indexOf("prores") >= 0 || _name.indexOf("jpg") >= 0 || _name.indexOf("jpeg") >= 0)
    {
        _qualityParam = "-qscale:v";
    }
    else
    {
        _qualityParam = "";
    }
}

void FFCodec::setSpeedParam()
{
    if (_name == "h264")
    {
        _speedParam = "-preset";
    }
    else
    {
        _speedParam = "";
    }
}
