#include "ffcodec.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif


FFCodec::FFCodec(QString name, QString prettyName, QObject *parent)  : FFBaseObject(name, prettyName, parent)
{
    _abilities =  Abilities(0);
    _defaultPixFormat = nullptr;

    setQualityParam();
}

FFCodec::FFCodec(QString name, QString prettyName, Abilities abilities, QObject *parent)  : FFBaseObject(name, prettyName, parent)
{
    _abilities = abilities;
    _defaultPixFormat = nullptr;

    setQualityParam();
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

FFPixFormat *FFCodec::defaultPixFormat(bool withAlpha) const
{
    return pixFormatWithAlpha( _defaultPixFormat, withAlpha);
}

FFPixFormat *FFCodec::pixFormatWithAlpha(FFPixFormat *pf, bool alpha) const
{
    if (pf == nullptr) return nullptr;
    if ( pf->hasAlpha() != alpha )
    {
        //find the closest one with(out) alpha
        foreach ( FFPixFormat *p, _pixFormats )
        {
            if ( p->hasAlpha() == alpha )
                if ( p->isOutput() && pf->isOutput() )
                    if (p->isInput() && pf->isInput())
                        if (p->colorSpace() == pf->colorSpace())
                            if ( (alpha && p->numComponents()+1 == pf->numComponents()) || (!alpha && p->numComponents() == pf->numComponents()+1) )
                                if( p->bitsPerPixel() == pf->bitsPerPixel() )
                                {
                                    return p;
                                }
        }
        foreach ( FFPixFormat *p, _pixFormats )
        {
            if ( p->hasAlpha() == alpha )
                if ( p->isOutput() && pf->isOutput() )
                    if (p->isInput() && pf->isInput())
                        if (p->colorSpace() == pf->colorSpace())
                            if ( (alpha && p->numComponents()+1 == pf->numComponents()) || (!alpha && p->numComponents() == pf->numComponents()+1) )
                            {
                                return p;
                            }

        }
        foreach ( FFPixFormat *p, _pixFormats )
        {
            if ( p->hasAlpha() == alpha )
                if ( p->isOutput() && pf->isOutput() )
                    if (p->isInput() && pf->isInput())
                        if (p->colorSpace() == pf->colorSpace())
                        {
                            return p;
                        }
        }
        foreach ( FFPixFormat *p, _pixFormats )
        {
            if ( p->hasAlpha() == alpha )
                if ( p->isOutput() && pf->isOutput() )
                    if (p->isInput() && pf->isInput())
                    {
                        return p;
                    }
        }
        foreach ( FFPixFormat *p, _pixFormats )
        {
            if ( p->hasAlpha() == alpha )
                if ( p->isOutput() && pf->isOutput() )
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
        if (quality < 10) quality = MathUtils::linearInterpolation( quality, 0, 10, 0, 15 );
        else if (quality < 25) quality = MathUtils::linearInterpolation( quality, 10, 25, 15, 21 );
        else if (quality < 50) quality = MathUtils::linearInterpolation( quality, 25, 50, 21, 28 );
        else if (quality < 75) quality = MathUtils::linearInterpolation( quality, 50, 75, 28, 34 );
        else quality = MathUtils::linearInterpolation( quality, 75, 100, 35, 51 );

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
        if (quality < 10) quality = MathUtils::linearInterpolation( quality, 0, 10, 0, 5 );
        else if (quality < 25) quality = MathUtils::linearInterpolation( quality, 10, 25, 5, 9 );
        else if (quality < 50) quality = MathUtils::linearInterpolation( quality, 25, 50, 9, 13 );
        else if (quality < 75) quality = MathUtils::linearInterpolation( quality, 50, 75, 13, 23 );
        else quality = MathUtils::linearInterpolation( quality, 75, 100, 23, 32 );

        return QString::number(quality);
    }
    return "";
}

void FFCodec::setQualityParam()
{
    if (_name == "h264")
    {
        _qualityParam = "-crf";
    }
    else if (_name.indexOf("prores") >= 0)
    {
        _qualityParam = "-qscale:v";
    }
    else
    {
        _qualityParam = "";
    }
}
