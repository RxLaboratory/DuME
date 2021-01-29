#include "ffcodec.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif


FFCodec::FFCodec(QString name, QString prettyName, QObject *parent)  : FFBaseObject(name, prettyName, parent)
{
    _abilities =  Abilities(0);

    init();
}

FFCodec::FFCodec(QString name, QString prettyName, Abilities abilities, QObject *parent)  : FFBaseObject(name, prettyName, parent)
{
    _abilities = abilities;
    init();
}

void FFCodec::init()
{
    setQualityParam();
    setDefaultPixFormat( );
    setSpeedParam();
    if (_name == "h264" || _name == "h265" || _name == "hevc" || _name == "libx265" || _name == "libx264")
    {
        setBitrateTypeCapability(true);
    }
    else
    {
        setBitrateTypeCapability(false);
    }

    //TUNINGS
    _tunings << new FFBaseObject("", "Default", this);
    if (_name == "h264" || _name == "libx264")
    {
        setTuningCapability(true);
        _tunings << new FFBaseObject("film", "Film", this);
        _tunings << new FFBaseObject("animation", "Animation", this);
        _tunings << new FFBaseObject("grain", "Video with grain", this);
        _tunings << new FFBaseObject("stillimage", "Slideshow", this);
        _tunings << new FFBaseObject("fastdecode", "Fast decode", this);
        _tunings << new FFBaseObject("zerolatency", "Streaming (low latency)", this);
    }
    else if (_name == "h265" || _name == "hevc" || _name == "libx265")
    {
        setTuningCapability(true);
        _tunings << new FFBaseObject("grain", "Video with grain", this);
        _tunings << new FFBaseObject("fastdecode", "Fast decode", this);
        _tunings << new FFBaseObject("zerolatency", "Streaming (low latency)", this);
    }
    else
    {
        setTuningCapability(false);
    }

    //PROFILES
    _profiles << new FFBaseObject("", "Default", this);
    if (_name == "h264" || _name == "libx264")
    {
        setProfileCapability(true);
        _profiles << new FFBaseObject("baseline", "Constrained Baseline");
        _profiles << new FFBaseObject("main", "Main");
        _profiles << new FFBaseObject("high", "High");
        //_profiles << new FFProfile("high10", "High 10");
        //_profiles << new FFProfile("high422", "High 422");
        //_profiles << new FFProfile("high444", "High 444");
    }
    else if (_name.toLower().startsWith("prores"))
    {
        setProfileCapability(true);
        _profiles << new FFBaseObject("0", "Proxy");
        _profiles << new FFBaseObject("1", "LT");
        _profiles << new FFBaseObject("2", "SQ");
        _profiles << new FFBaseObject("3", "HQ");
    }
    else
    {
        setProfileCapability(false);
    }
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

bool FFCodec::useSpeed()
{
    return _capabilities.testFlag(Speed);
}

void FFCodec::setSpeedCapability(bool useSpeed)
{
    _capabilities.setFlag(Speed, useSpeed);
}

bool FFCodec::useTuning()
{
    return _capabilities.testFlag(Tuning);
}

void FFCodec::setTuningCapability(bool useTuning)
{
    _capabilities.setFlag(Tuning, useTuning);
}

bool FFCodec::useQuality()
{
    return _capabilities.testFlag(Quality);
}

void FFCodec::setQualityCapability(bool useQuality)
{
    _capabilities.setFlag(Quality, useQuality);
}

bool FFCodec::useBitrateType()
{
    return _capabilities.testFlag(BitrateType);
}

void FFCodec::setBitrateTypeCapability(bool useType)
{
    _capabilities.setFlag(BitrateType, useType);
}

bool FFCodec::useProfile()
{
    return _capabilities.testFlag(Profile);
}

void FFCodec::setProfileCapability(bool useProfile)
{
    _capabilities.setFlag(Profile, useProfile);
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

QList<FFBaseObject *> FFCodec::profiles() const
{
    return _profiles;
}

FFBaseObject *FFCodec::profile(QString name)
{
    name = name.trimmed().toLower();
    foreach(FFBaseObject *pf,_profiles)
    {
        if (pf->name().toLower() == name) return pf;
    }
    foreach(FFBaseObject *pf,_profiles)
    {
        if (pf->prettyName().toLower() == name) return pf;
    }
    return _profiles[0];
}

QString FFCodec::qualityParam() const
{
    return _qualityParam;
}

QString FFCodec::qualityValue(int quality)
{
    if (_name == "h264" || _name == "libx264")
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

    if (_name == "h265" || _name == "hevc" || _name == "libx265")
    {
        quality = 100-quality;
        //adjust to CRF values
        if (quality < 10) quality = Interpolations::linear( quality, 0, 10, 0, 21 );
        else if (quality < 25) quality = Interpolations::linear( quality, 10, 25, 21, 25 );
        else if (quality < 50) quality = Interpolations::linear( quality, 25, 50, 25, 28 );
        else if (quality < 75) quality = Interpolations::linear( quality, 50, 75, 28, 35 );
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
    if (_name == "h264" || _name == "h265" || _name == "hevc" || _name == "libx265" || _name == "libx264")
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

void FFCodec::setCapabilities(const Capabilities &capabilities)
{
    _capabilities = capabilities;
}

void FFCodec::setQualityParam()
{
    if (_name == "h264" || _name == "h265" || _name == "hevc" || _name == "libx265" || _name == "libx264")
    {
        _qualityParam = "-crf";
        setQualityCapability(true);
    }
    else if (_name.indexOf("prores") >= 0 || _name.indexOf("jpg") >= 0 || _name.indexOf("jpeg") >= 0)
    {
        _qualityParam = "-qscale:v";
        setQualityCapability(true);
    }
    else
    {
        _qualityParam = "";
        setQualityCapability(false);
    }

}

void FFCodec::setSpeedParam()
{
    if (_name == "h264" || _name == "h265" || _name == "hevc" || _name == "libx265" || _name == "libx264")
    {
        _speedParam = "-preset";
        setSpeedCapability(true);
    }
    else
    {
        _speedParam = "";
        setSpeedCapability(false);
    }


}

QList<FFBaseObject *> FFCodec::tunings() const
{
    return _tunings;
}

FFBaseObject *FFCodec::tuning(QString name)
{
    name = name.trimmed().toLower();
    foreach(FFBaseObject *t, _tunings)
    {
        if (t->name().toLower() == name) return t;
    }
    foreach(FFBaseObject *t, _tunings)
    {
        if (t->prettyName().toLower() == name) return t;
    }
    return _tunings.at(0);
}
