#include "ffmpeg.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

FFmpeg::FFmpeg(QString path,QObject *parent) : AbstractRendererInfo(parent)
{
    _status = MediaUtils::Initializing;

    // Defaults
    _defaultPixFormat = FFPixFormat::getDefault( this );
    _defaultSampleFormat = FFSampleFormat::getDefault( this );
    _defaultCodec = FFCodec::getDefault( this );
    _defaultMuxer = FFMuxer::getDefault( this );
    _defaultObject = new FFBaseObject("", "Default", this);

    // The color TRCs

    FFColorItem *trc = new FFColorItem("", "Auto");
    _colorTRCs << trc;

    trc = new FFColorItem("input", "Same as input", "input", FFColorItem::ZScale);
    trc->setMetadataName("");
    trc->setInput(false);
    _colorTRCs << trc;

    trc = new FFColorItem("iec61966_2_1", "sRGB / YCCIEC / 61966-2-1", "iec61966-2-1", FFColorItem::ZScale);
    trc->setOutputCSScaleName("iec61966-2-1");
    trc->setInputCSScaleName("iec61966-2-1");
    trc->setOutputGScaleName("1/2.2");
    trc->setInputGScaleName("2.2");
    _colorTRCs << trc;

    trc = new FFColorItem("iec61966_2_4", "Extended-gamut YCC (xvYCC) / IEC 61966-2-4", "iec61966-2-4", FFColorItem::Colorspace);
    _colorTRCs << trc;

    trc = new FFColorItem("linear", "Linear", "linear", FFColorItem::ZScale);
    trc->setOutputGScaleName("1");
    trc->setInputGScaleName("1");
    _colorTRCs << trc;

    trc = new FFColorItem("log100", "Logarithmic");
    trc->setOutput(false);
    _colorTRCs << trc;

    trc = new FFColorItem("log_sqrt", "Logarithmic square root");
    trc->setOutput(false);
    _colorTRCs << trc;

    trc = new FFColorItem("bt709", "BT.709 / BT.1361", "709", FFColorItem::ZScale);
    trc->setOutputCSScaleName("bt709");
    trc->setInputCSScaleName("bt709");
    trc->setOutputGScaleName("1/2.4");
    trc->setInputGScaleName("2.4");
    _colorTRCs << trc;

    trc = new FFColorItem("gamma22", "PAL & SECAM / BT.470 M / BT.1700 625 / Gamma 2.2", "bt470m", FFColorItem::Colorspace);
    trc->setOutputGScaleName("1/2.2");
    trc->setInputGScaleName("2.2");
    _colorTRCs << trc;

    trc = new FFColorItem("gamma28", "BT.470 BG / Gamma 2.8", "bt470bg", FFColorItem::Colorspace);
    trc->setOutputGScaleName("1/2.8");
    trc->setInputGScaleName("2.8");
    _colorTRCs << trc;

    trc = new FFColorItem("bt1361","BT.1361");
    trc->setOutput(false);
    _colorTRCs << trc;

    trc = new FFColorItem("bt1361e","BT.1361 Extended Colour Gamut");
    trc->setOutput(false);
    _colorTRCs << trc;

    trc = new FFColorItem("14","BT.2020 - 10 bit", "2020_10", FFColorItem::ZScale); // use the index instead of name as the name does not seem to work
    trc->setOutputCSScaleName("bt2020-10");
    trc->setInputCSScaleName("bt2020-10");
    _colorTRCs << trc;

    trc = new FFColorItem("15","BT.2020 - 12 bit", "2020_12", FFColorItem::ZScale); // use the index instead of name as the name does not seem to work
    trc->setOutputCSScaleName("bt2020-12");
    trc->setInputCSScaleName("bt2020-12");
    _colorTRCs << trc;

    trc = new FFColorItem("smpte170m","NTSC / BT.601-6 / BT.1358 / BT.1700  / SMPTE 170 M / BT.470 B.B1.G", "601", FFColorItem::ZScale);
    trc->setOutputGScaleName("1/2.4");
    trc->setInputGScaleName("2.4");
    _colorTRCs << trc;

    trc = new FFColorItem("smpte240m","SMPTE 240 M", "smpte240m", FFColorItem::Colorspace);
    _colorTRCs << trc;

    trc = new FFColorItem("smpte428","SMPTE 428 / SMPTE 428-1");
    trc->setOutput(false);
    _colorTRCs << trc;

    trc = new FFColorItem("smpte2084","SMPTE ST 2084 10/12/14/16 bit", "smpte2084", FFColorItem::ZScale);
    _colorTRCs << trc;

    trc = new FFColorItem("18","Hybrid log-gamma / ARIB STD-B67", "arib-std-b67", FFColorItem::ZScale);
    _colorTRCs << trc;

    trc = new FFColorItem("dcip3", "DCI P3 (Gamma 2.6)", "1/2.6", FFColorItem::Gamma);
    trc->setInputGScaleName("2.6");
    trc->setMetadataName("linear");
    _colorTRCs << trc;

    trc = new FFColorItem("qt196", "QT gamma 1.96", "1/1.96", FFColorItem::Gamma);
    trc->setInputGScaleName("1.96");
    trc->setMetadataName("linear");
    _colorTRCs << trc;

    // The color Primaries
    FFColorItem *pri = new FFColorItem("","Auto");
    _colorPrimaries << pri;

    pri = new FFColorItem("input","Same as input", "input", FFColorItem::ZScale);
    pri->setMetadataName("");
    pri->setInput(false);
    _colorPrimaries << pri;

    pri = new FFColorItem("film","Film / Illuminant C", "film", FFColorItem::Colorspace);
    _colorPrimaries << pri;

    pri = new FFColorItem("bt709","BT.709 / RGB / IEC 61966-2-4 / BT.1361", "709", FFColorItem::ZScale);
    pri->setOutputCSScaleName("bt709");
    pri->setInputCSScaleName("bt709");
    _colorPrimaries << pri;

    pri = new FFColorItem("bt470m","BT.470 M / FCC Title 47", "bt470m", FFColorItem::Colorspace);
    _colorPrimaries << pri;

    pri = new FFColorItem("bt470bg","PAL & SECAM / BT.470 BG / BT.601-6 625 / BT.1358 625 / BT.1700 625", "bt470bg", FFColorItem::Colorspace);
    _colorPrimaries << pri;

    pri = new FFColorItem("bt2020","BT.2020", "2020", FFColorItem::ZScale);
    pri->setOutputCSScaleName("bt2020");
    pri->setInputCSScaleName("bt2020");
    _colorPrimaries << pri;

    pri = new FFColorItem("smpte170m","NTSC / SMPTE 170 M / BT.601-6 525 / BT.1358 525 / BT.1700", "170m", FFColorItem::ZScale);
    pri->setOutputCSScaleName("smpte170m");
    pri->setInputCSScaleName("smpte170m");
    _colorPrimaries << pri;

    pri = new FFColorItem("smpte240m","SMPTE 240 M", "240m", FFColorItem::ZScale);
    pri->setOutputCSScaleName("smpte240m");
    pri->setInputCSScaleName("smpte240m");
    _colorPrimaries << pri;

    pri = new FFColorItem("smpte428","SMPTE 428 / SMPTE ST 428-1 / SMPTE ST 428-1 (CIE 1931 XYZ)");
    pri->setOutput(false);
    _colorPrimaries << pri;

    pri = new FFColorItem("smpte431","DCI P3 / SMPTE ST 431-2 (2011)", "smpte431", FFColorItem::Colorspace);
    _colorPrimaries << pri;

    pri = new FFColorItem("smpte432","P3 D65 / Display P3 / SMPTE ST 432-1 (2010)", "smpte432", FFColorItem::Colorspace);
    _colorPrimaries << pri;

    pri = new FFColorItem("jedec-p22","EBU Tech. 3213-E / JEDEC P22 phosphors", "jedec-p22", FFColorItem::Colorspace);
    _colorPrimaries << pri;

    pri = new FFColorItem("acescg", "ACEScg", ":/luts/rgb-to-aces.3dl", FFColorItem::LUT);
    pri->setMetadataName("bt709");
    pri->setInputLScaleName(":/luts/aces-to-rgb.3dl");
    _colorPrimaries << pri;

    // The color Ranges
    _colorRanges << new FFColorItem("","Auto");

    FFColorItem *r = new FFColorItem("input", "Same as input", "input", FFColorItem::ZScale);
    r->setMetadataName("");
    r->setInput(false);
    _colorRanges << r;

    _colorRanges << new FFColorItem("tv","Limited", "limited", FFColorItem::ZScale);
    _colorRanges << new FFColorItem("pc","Full", "full", FFColorItem::ZScale);

    // The color matrices
    FFColorItem *m = new FFColorItem("","Auto");
    _colorSpaces << m;

    m = new FFColorItem("input", "Same as input", "input", FFColorItem::ZScale);
    m->setMetadataName("");
    m->setInput(false);
    _colorSpaces << m;

    m = new FFColorItem("rgb","RGB / IEC 61966-2-1", "709", FFColorItem::ZScale);
    m->setInputCSScaleName("bt709");
    m->setOutputCSScaleName("bt709");
    _colorSpaces << m;

    m = new FFColorItem("bt709","BT.709 / BT.1361 / IEC 61966-2-4 xvYCC709 / SMPTE RP177", "709", FFColorItem::ZScale);
    m->setInputCSScaleName("bt709");
    m->setOutputCSScaleName("bt709");
    _colorSpaces << m;

    m = new FFColorItem("bt470bg","PAL & SECAM / BT.470 BG / BT.601-6 / BT.1358 / IEC 61966-2-4 xvYCC601", "470bg", FFColorItem::ZScale);
    m->setInputCSScaleName("bt470bg");
    m->setOutputCSScaleName("bt470bg");
    _colorSpaces << m;

    m = new FFColorItem("bt2020_ncl","BT.2020 NCL (non-constant luminance system)", "2020_ncl", FFColorItem::ZScale);
    m->setInputCSScaleName("bt2020ncl");
    m->setOutputCSScaleName("bt2020ncl");
    _colorSpaces << m;

    m = new FFColorItem("bt2020_cl","BT.2020 CL (constant luminance system)", "2020_cl", FFColorItem::ZScale);
    _colorSpaces << m;

    m = new FFColorItem("smpte2085","SMPTE 2085 / Y'D'zD'x");
    m->setOutput(false);
    _colorSpaces << m;

    m = new FFColorItem("smpte170m","NTSC / SMPTE 170 M / BT.601-6 / BT.1358", "170m", FFColorItem::ZScale);
    m->setInputCSScaleName("smpte170m");
    m->setOutputCSScaleName("smpte170m");
    _colorSpaces << m;

    m = new FFColorItem("smpte240m","SMPTE 240 M", "smpte240m", FFColorItem::Colorspace);
    _colorSpaces << m;

    m = new FFColorItem("ycgco","YCGCO / YCOCG / Dirac / VC-2 and H.264 FRext", "ycgco", FFColorItem::Colorspace);
    _colorSpaces << m;

    m = new FFColorItem("fcc","FCC Title 47", "fcc", FFColorItem::Colorspace);
    _colorSpaces << m;

    m = new FFColorItem("14","BT.2100-0 / ICtCp");
    m->setOutput(false);
    _colorSpaces << m;

    // The complete color Profiles
    _colorProfiles << new FFColorProfile("", "Auto", colorPrimary(""), colorTRC(""), colorSpace(""), colorRange(""));
    _colorProfiles << new FFColorProfile("input", "Same as input", colorPrimary("input"), colorTRC("input"), colorSpace("input"), colorRange("input"));
    _colorProfiles << new FFColorProfile("srgb", "Images (sRGB)", colorPrimary("bt709"), colorTRC("iec61966_2_1"), colorSpace("rgb"), colorRange("pc"));
    _colorProfiles << new FFColorProfile("bt709", "HD Video (BT.709)", colorPrimary("bt709"), colorTRC("bt709"), colorSpace("bt709"), colorRange("tv"));
    _colorProfiles << new FFColorProfile("bt2020_10", "UHD (4K/8K) Video (BT.2020-10bits)", colorPrimary("bt2020"), colorTRC("14"), colorSpace("bt2020_cl"), colorRange("pc"));
    _colorProfiles << new FFColorProfile("bt2020_12", "UHD (4K/8K) HDR Video (BT.2020-12bits)", colorPrimary("bt2020"), colorTRC("15"), colorSpace("bt2020_ncl"), colorRange("pc"));
    _colorProfiles << new FFColorProfile("displayp3", "Display P3 / P3 D65", colorPrimary("smpte432"), colorTRC("iec61966_2_1"), colorSpace("rgb"), colorRange("pc"));
    _colorProfiles << new FFColorProfile("dcip3", "DCI P3", colorPrimary("smpte431"), colorTRC("dcip3"), colorSpace("rgb"), colorRange("pc"));
    _colorProfiles << new FFColorProfile("linear", "Linear RGB", colorPrimary("bt709"), colorTRC("linear"), colorSpace("rgb"), colorRange("pc"));
    _colorProfiles << new FFColorProfile("acescg", "ACEScg", colorPrimary("acescg"), colorTRC("linear"), colorSpace("rgb"), colorRange("pc"));
    _colorProfiles << new FFColorProfile("palsecam", "PAL / SECAM", colorPrimary("bt470bg"), colorTRC("gamma22"), colorSpace("bt470bg"), colorRange("tv"));
    _colorProfiles << new FFColorProfile("ntsc", "NTSC", colorPrimary("smpte170m"), colorTRC("smpte170m"), colorSpace("smpte170m"), colorRange("tv"));
    _colorProfiles << new FFColorProfile("qt196", "QT 709 gamma 1.96", colorPrimary("bt709"), colorTRC("qt196"), colorSpace("bt709"), colorRange("tv"));

    // The LUTs
    _luts << new FFLut( "", "None");
    _luts << new FFLut( ":/luts/rgb-to-aces.3dl", "Convert to Linear ACES", "linear", "acescg", FFLut::ThreeD);
    _luts << new FFLut( ":/luts/aces-to-rgb.3dl", "Convert from Linear ACES to Linear RGB", "acescg", "linear", FFLut::ThreeD);
    _luts << new FFLut(  ":/luts/linear-to-filmic_veryhigh.3dl", "Blender Filmic (Very high contrast)", "linear", "srgb", FFLut::OneD);
    _luts << new FFLut(  ":/luts/linear-to-filmic_high.3dl", "Blender Filmic (High contrast)", "linear",  "srgb", FFLut::OneD);
    _luts << new FFLut(  ":/luts/linear-to-filmic_mediumhigh.3dl", "Blender Filmic (Medium high contrast)", "linear",  "srgb", FFLut::OneD);
    _luts << new FFLut(  ":/luts/linear-to-filmic_medium.3dl", "Blender Filmic (Medium contrast)", "linear",  "srgb", FFLut::OneD);
    _luts << new FFLut(  ":/luts/linear-to-filmic_mediumlow.3dl", "Blender Filmic (Medium low contrast)", "linear",  "srgb", FFLut::OneD);
    _luts << new FFLut(  ":/luts/linear-to-filmic_low.3dl", "Blender Filmic (Low contrast)", "linear",  "srgb", FFLut::OneD);
    _luts << new FFLut(  ":/luts/linear-to-filmic_verylow.3dl", "Blender Filmic (Very low contrast)", "linear",  "srgb", FFLut::OneD);
    _luts << new FFLut(  ":/luts/adobe-to-qt.cube", "Set QT Gamma 1.96 (Adobe LUT)", "bt709", "qt196", FFLut::OneD);
    _luts << new FFLut(  ":/luts/qt-to-adobe.cube", "Undo QT Gamma 1.96 (Adobe LUT)", "qt196", "bt709", FFLut::OneD);
    _luts << new FFLut( "custom", "Custom...");

    //The motion interpolation algorithms
    _motionInterpolationAlgorithms << new FFBaseObject("","Default (epzs)");
    _motionInterpolationAlgorithms << new FFBaseObject("esa", "Exhaustive search");
    _motionInterpolationAlgorithms << new FFBaseObject("tss", "Three step search");
    _motionInterpolationAlgorithms << new FFBaseObject("tdls", "Two dimensional logarithmic search");
    _motionInterpolationAlgorithms << new FFBaseObject("ntss", "New three step search");
    _motionInterpolationAlgorithms << new FFBaseObject("fss", "Four step search");
    _motionInterpolationAlgorithms << new FFBaseObject("ds", "Diamond search");
    _motionInterpolationAlgorithms << new FFBaseObject("hexbs", "Hexagon-based search");
    _motionInterpolationAlgorithms << new FFBaseObject("epzs", "Enhanced predictive zonal search");
    _motionInterpolationAlgorithms << new FFBaseObject("umh", "Uneven multi-hexagon");

    //The list of motion estimation modes
    _motionEstimationModes << new FFBaseObject("","Default (bilateral)");
    _motionEstimationModes << new FFBaseObject("bilat","Bilateral");
    _motionEstimationModes << new FFBaseObject("bidir","Bidirectionnal");

    //The list of resize algorithms
    _resizeAlgorithms << new FFBaseObject("", "Default (bicubic)");
    _resizeAlgorithms << new FFBaseObject("fast_bilinear", "Fast Bilinear");
    _resizeAlgorithms << new FFBaseObject("bilinear", "Bilinear");
    _resizeAlgorithms << new FFBaseObject("bicubic", "Bicubic");
    _resizeAlgorithms << new FFBaseObject("experimental", "Experimental scaling");
    _resizeAlgorithms << new FFBaseObject("neighbor", "Neighbor rescaling");
    _resizeAlgorithms << new FFBaseObject("area", "Area");
    _resizeAlgorithms << new FFBaseObject("bicublin", "Bicubic (Luma) + Bilinear (Chroma)");
    _resizeAlgorithms << new FFBaseObject("gauss", "Gaussian");
    _resizeAlgorithms << new FFBaseObject("sinc", "Sinc");
    _resizeAlgorithms << new FFBaseObject("lanczos", "Lanczos");
    _resizeAlgorithms << new FFBaseObject("spline", "Natural Bicubic Spline");

    if (path != "") setBinary(path, false);

    _instance = this;
}

FFmpeg *FFmpeg::instance()
{
    if (!_instance) _instance = new FFmpeg();
    return _instance;
}

FFBaseObject *FFmpeg::defaultObject()
{
    return _defaultObject;
}

bool FFmpeg::setBinary(QString path, bool initialize)
{
    QSettings settings;

    if ( AbstractRendererInfo::setBinary(path) )
    {
        settings.setValue("ffmpeg/path", path );
        if (initialize) init();
        emit binaryChanged( path );
        qDebug() << "New FFmpeg path correctly set: " + path;
        return true;
    }
    return false;
}

void FFmpeg::init()
{   
    //TODO auto find ffmpeg if no settings or path invalid
    QSettings settings;

    _version = settings.value("ffmpeg/version","").toString();

#ifdef Q_OS_LINUX
    //First, look for a dume static build in the app folder
    QString defaultFFmpegPath = QCoreApplication::applicationDirPath() + "/dume-ffmpeg";
    //IF not found, try with a standard name
    if (!QFileInfo::exists(defaultFFmpegPath)) defaultFFmpegPath = QCoreApplication::applicationDirPath() + "/ffmpeg";
    //IF not found, try with a system command
    if (!QFileInfo::exists(defaultFFmpegPath)) defaultFFmpegPath = "ffmpeg";
#endif
#ifdef Q_OS_WIN
    QString defaultFFmpegPath = QCoreApplication::applicationDirPath() + "/ext/ffmpeg.exe";
    if (!QFileInfo::exists(defaultFFmpegPath)) defaultFFmpegPath = QCoreApplication::applicationDirPath() + "/ffmpeg.exe";
#endif
#ifdef Q_OS_MAC
    QString defaultFFmpegPath = QCoreApplication::applicationDirPath() + "/ffmpeg";
    //IF not found, try with a system command
    if (!QFileInfo::exists(defaultFFmpegPath)) defaultFFmpegPath = "ffmpeg";
#endif

    QString path = settings.value("ffmpeg/path",defaultFFmpegPath).toString();
    setBinary(path, false);

    qDebug() << "Init FFmpeg. Path " + path;

    _status = MediaUtils::Initializing;
    emit statusChanged(_status);

    //get version
    emit newLog( "Checking version" );
    QString newVersion = "";
    if (runCommand( "", 1000))
    {
        newVersion = gotVersion(_output);
    }

    //get pixFormats
    emit newLog( "Loading Pixel Formats" );
    if (runCommand( "-hide_banner -pix_fmts", 10000))
    {
        gotPixFormats( _output, newVersion );
    }

    emit newLog("Loading Audio Formats");
    if (runCommand("-hide_banner -sample_fmts", 10000))
    {
        gotSampleFormats( _output, newVersion );
    }

    //get codecs
    emit newLog( "Loading Codecs" );
    if (runCommand( "-hide_banner -codecs" , 10000))
    {
        gotCodecs( _output, newVersion );
    }

    //get muxers
    emit newLog( "Loading Muxers" );
    if (runCommand( "-hide_banner -formats" , 10000))
    {
        gotMuxers( _output, newVersion );
    }

    //get long help
    emit newLog( "Loading Documentation" );
    // ignore errors as this is not so important and sometimes fails to start for some reason...
    if (runCommand( "-h long", 3000, QIODevice::ReadOnly, true))
    {
        _longHelp = _output;
    }

    //get help
    _help = settings.value("ffmpeg/help","").toString();
    emit newLog( "Loading help" );
    // ignore errors as this is not so important and sometimes fails to start for some reason...
    if (runCommand( "-h", 3000, QIODevice::ReadOnly, true))
    {
        _help = _output;
    }

    _version = newVersion;
    settings.setValue("ffmpeg/version",_version);

    emit newLog( "FFmpeg ready" );

    _status = MediaUtils::Waiting;
    emit statusChanged(_status);

    settings.sync();
}

QList<FFSampleFormat *> FFmpeg::sampleFormats() const
{
    return _sampleFormats;
}

QList<FFColorProfile *> FFmpeg::colorProfiles() const
{
    return _colorProfiles;
}

QList<FFColorItem *> FFmpeg::colorRanges() const
{
    return _colorRanges;
}

QList<FFColorItem *> FFmpeg::colorSpaces() const
{
    return _colorSpaces;
}

QList<FFColorItem *> FFmpeg::colorTRCs() const
{
    return _colorTRCs;
}

QList<FFColorItem *> FFmpeg::colorPrimaries() const
{
    return _colorPrimaries;
}

QList<FFLut *> FFmpeg::luts() const
{
    return _luts;
}

FFLut *FFmpeg::lut(QString name)
{
    name = name.trimmed();
    foreach(FFLut *l, _luts)
    {
        if (l->name().toLower() == name.trimmed().toLower()) return l;
    }
    foreach(FFLut *l, _luts)
    {
        if (l->prettyName() == name) return l;
    }
    return _luts[0];
}

QList<FFMuxer *> FFmpeg::muxers(bool encodeOnly)
{
    if (encodeOnly) return _encodeMuxers;
    else return _muxers;
}

FFMuxer *FFmpeg::muxer(QString nameOrExtension)
{
    foreach(FFMuxer *muxer,_muxers)
    {
        if (muxer->name().toLower() == nameOrExtension.trimmed().toLower())
        {
            return muxer;
        }
    }
    foreach(FFMuxer *muxer,_muxers)
    {
        if (muxer->prettyName() == nameOrExtension)
        {
            return muxer;
        }
    }
    foreach(FFMuxer *muxer,_muxers)
    {
        if (muxer->extensions().contains(nameOrExtension))
        {
            return muxer;
        }
    }
    return _defaultMuxer;
}

FFCodec *FFmpeg::muxerDefaultCodec(FFMuxer *muxer, FFCodec::Ability ability)
{
    FFCodec *videoCodec = muxer->defaultVideoCodec();
    FFCodec *audioCodec = muxer->defaultAudioCodec();

    //return
    if (ability == FFCodec::Video) return videoCodec;
    if (ability == FFCodec::Audio) return audioCodec;

    return _defaultCodec;
}

FFCodec *FFmpeg::muxerDefaultCodec(QString name, FFCodec::Ability ability)
{
    return muxerDefaultCodec(muxer(name),ability);
}

QList<FFCodec *> FFmpeg::encoders()
{
    QList<FFCodec *> encoders = _videoEncoders;
    encoders.append(_audioEncoders);

    return encoders;
}

QList<FFCodec *> FFmpeg::videoEncoders()
{
    return _videoEncoders;
}

QList<FFCodec *> FFmpeg::audioEncoders()
{
    return _audioEncoders;
}

FFCodec *FFmpeg::videoEncoder(QString name)
{
    foreach(FFCodec *codec,_videoEncoders)
    {
        if (codec->name().toLower() == name.trimmed().toLower()) return codec;
    }
    foreach(FFCodec *codec,_videoEncoders)
    {
        if (codec->prettyName() == name) return codec;
    }
    return _defaultCodec;
}

FFCodec *FFmpeg::audioEncoder(QString name)
{
    name = name.trimmed();
    foreach(FFCodec *codec,_audioEncoders)
    {
        if (codec->name().toLower() == name.trimmed().toLower()) return codec;
    }
    foreach(FFCodec *codec,_audioEncoders)
    {
        if (codec->prettyName() == name) return codec;
    }
    return _defaultCodec;
}

FFCodec *FFmpeg::videoDecoder(QString name)
{
    name = name.trimmed();
    foreach(FFCodec *codec,_videoDecoders)
    {
        if (codec->name().toLower() == name.trimmed().toLower()) return codec;
    }
    foreach(FFCodec *codec,_videoDecoders)
    {
        if (codec->prettyName() == name) return codec;
    }
    return _defaultCodec;
}

FFCodec *FFmpeg::audioDecoder(QString name)
{
    name = name.trimmed();
    foreach(FFCodec *codec,_audioDecoders)
    {
        if (codec->name().toLower() == name.trimmed().toLower()) return codec;
    }
    foreach(FFCodec *codec,_audioDecoders)
    {
        if (codec->prettyName() == name) return codec;
    }
    return _defaultCodec;
}

FFPixFormat *FFmpeg::pixFormat(QString name)
{
    name = name.trimmed();
    foreach(FFPixFormat *pf,_pixFormats)
    {
        if (pf->name().toLower() == name.toLower()) return pf;
    }
    foreach(FFPixFormat *pf,_pixFormats)
    {
        if (pf->prettyName() == name) return pf;
    }
    return _defaultPixFormat;
}

FFSampleFormat *FFmpeg::sampleFormat(QString name)
{
    name = name.trimmed();
    foreach(FFSampleFormat *sf, _sampleFormats)
    {
        if (sf->name().toLower() == name.toLower()) return sf;
    }
    foreach(FFSampleFormat *sf, _sampleFormats)
    {
        if (sf->prettyName() == name) return sf;
    }
    return _defaultSampleFormat;
}

FFColorItem *FFmpeg::colorTRC(QString name)
{
    name = name.trimmed();
    foreach(FFColorItem *c,_colorTRCs)
    {
        if (c->name().toLower() == name.trimmed().toLower()) return c;
    }
    foreach(FFColorItem *c,_colorTRCs)
    {
        if (c->prettyName() == name) return c;
    }
    return _colorTRCs[0];
}

FFColorItem *FFmpeg::colorPrimary(QString name)
{
    name = name.trimmed();
    foreach(FFColorItem *c,_colorPrimaries)
    {
        if (c->name().toLower() == name.trimmed().toLower()) return c;
    }
    foreach(FFColorItem *c,_colorPrimaries)
    {
        if (c->prettyName() == name) return c;
    }
    return _colorPrimaries[0];
}

FFColorItem *FFmpeg::colorSpace(QString name)
{
    name = name.trimmed();
    foreach(FFColorItem *c,_colorSpaces)
    {
        if (c->name().toLower() == name.trimmed().toLower()) return c;
    }
    foreach(FFColorItem *c,_colorSpaces)
    {
        if (c->prettyName() == name) return c;
    }
    return _colorSpaces[0];
}

FFColorItem *FFmpeg::colorRange(QString name)
{
    name = name.trimmed();
    foreach(FFColorItem *c,_colorRanges)
    {
        if (c->name().toLower() == name.trimmed().toLower()) return c;
    }
    foreach(FFColorItem *c,_colorRanges)
    {
        if (c->prettyName() == name) return c;
    }
    return _colorRanges[0];
}

FFColorProfile *FFmpeg::colorProfile(QString name)
{
    name = name.trimmed();
    foreach(FFColorProfile *c,_colorProfiles)
    {
        if (c->name().toLower() == name.trimmed().toLower()) return c;
    }
    foreach(FFColorProfile *c,_colorProfiles)
    {
        if (c->prettyName() == name) return c;
    }
    return _colorProfiles[0];
}

FFColorProfile *FFmpeg::colorProfile(FFColorItem *primaries, FFColorItem *trc, FFColorItem *space, FFColorItem *range, QObject *parent)
{
    foreach( FFColorProfile *p, _colorProfiles)
    {
        if (p->primaries()->name() == primaries->name() && p->trc()->name() == trc->name() && p->space()->name() == space->name() && p->range()->name() == range->name())
        {
            return p;
        }
    }
    return new FFColorProfile("temp", "temp", primaries, trc, space, range, parent);
}

QList<FFBaseObject *> FFmpeg::motionInterpolationAlgorithms() const
{
    return _motionInterpolationAlgorithms;
}

QList<FFBaseObject *> FFmpeg::motionEstimationModes() const
{
    return _motionEstimationModes;
}

FFBaseObject *FFmpeg::motionEstimationMode(QString name)
{
    name = name.toLower().trimmed();
    foreach(FFBaseObject *me, _motionEstimationModes)
    {
        if (me->name().toLower() == name) return me;
    }
    foreach(FFBaseObject *me,_motionEstimationModes)
    {
        if (me->prettyName().toLower() == name) return me;
    }
    return _motionEstimationModes[0];
}

FFBaseObject *FFmpeg::motionInterpolationAlgorithm(QString name)
{
    name = name.toLower().trimmed();
    foreach(FFBaseObject *mia, _motionInterpolationAlgorithms)
    {
        if (mia->name().toLower() == name) return mia;
    }
    foreach(FFBaseObject *mia,_motionInterpolationAlgorithms)
    {
        if (mia->prettyName().toLower() == name) return mia;
    }
    return _motionInterpolationAlgorithms[0];
}

QList<FFBaseObject *> FFmpeg::resizeAlgorithms() const
{
    return _resizeAlgorithms;
}

FFBaseObject *FFmpeg::resizeAlgorithm(QString name)
{
    name = name.toLower().trimmed();
    foreach(FFBaseObject *r, _resizeAlgorithms)
    {
        if (r->name().toLower() == name) return r;
    }
    foreach(FFBaseObject *r,_resizeAlgorithms)
    {
        if (r->prettyName().toLower() == name) return r;
    }
    return _resizeAlgorithms[0];
}

QString FFmpeg::help()
{
    return _help;
}

QString FFmpeg::longHelp()
{
    return _longHelp;
}

QString FFmpeg::analyseMedia(QString mediaPath)
{
    QStringList args("-hide_banner");
    args << "-i" << QDir::toNativeSeparators(mediaPath);
    if ( runCommand( args, 3000) )
    {
        return _output;
    }
    return "";
}

MediaUtils::RenderStatus FFmpeg::status() const
{
    return _status;
}

QString FFmpeg::escapeFilterOption(QString option)
{
    return option.replace(":", "\\:").replace("'", "\\'");
}

QString FFmpeg::version() const
{
    return _version;
}

QList<FFPixFormat *> FFmpeg::pixFormats()
{
    return _pixFormats;
}

bool muxerSorter(FFMuxer *m1,FFMuxer *m2)
{
    if (m1->extensions().count() == 0 && m2->extensions().count() == 0) return m1->prettyName().toLower() < m2->prettyName().toLower();
    if (m1->extensions().count() == 0) return true;
    if (m2->extensions().count() == 0) return false;
    return m1->extensions()[0] < m2->extensions()[0];
}

void FFmpeg::gotMuxers(QString output, QString newVersion)
{
    QSettings settings;
    //delete all
    qDeleteAll(_muxers);
    _muxers.clear();
    _decodeMuxers.clear();
    _encodeMuxers.clear();

    //if the version is the same as previously, just read data from settings
    if (newVersion == _version)
    {
        emit newLog("Getting muxers from stored list.");

        //open array
        int arraySize = settings.beginReadArray("ffmpeg/muxers");

        _progressMax = _prevMax + arraySize; //pixfmts + codecs + muxers
        emit progressMax( _progressMax );

        for (int i = 0; i < arraySize; i++)
        {
            settings.setArrayIndex(i);

            emit progress( _prevMax + i);

            QString name = settings.value("name").toString();
            QString prettyName = settings.value("prettyName").toString();

            //emit newLog("Loading muxer:" + name);
            if (name == "") continue;

            FFMuxer *m = new FFMuxer(name,prettyName,this);
            _muxers << m;
            _decodeMuxers << m;
            _encodeMuxers << m;

            //get default codecs
            QString defltVideoCodec = settings.value("defaultVideoCodec").toString();
            m->setDefaultVideoCodec(videoEncoder( defltVideoCodec ));

            QString defltAudioCodec = settings.value("defaultAudioCodec").toString();
            m->setDefaultAudioCodec(audioEncoder( defltAudioCodec ));

            QString extensions = settings.value("extensions").toString();
            QStringList extensionsList = extensions.split(",");
            if (extensionsList[0] != "") m->setExtensions(extensionsList);
        }

        settings.endArray();

        _prevMax = _prevMax + arraySize;
    }

    //if the version has changed, or if we did not get the list from settings
    if (newVersion != _version || _muxers.count() == 0)
    {
        emit newLog("Updating muxers list...");

        QStringList muxers = output.split("\n");
        QRegularExpression re("[D. ]E (\\w+)\\s+(.+)");

        int max = muxers.count();
        _progressMax = _prevMax + max; //pixfmts + codecs + muxers
        emit progressMax( _progressMax );

        //remove the previous codecs
        settings.remove("ffmpeg/muxers");
        //create the new array
        settings.beginWriteArray("ffmpeg/muxers");
        int n = -1;

        for (int i = 0 ; i < muxers.count() ; i++)
        {
            if (i%10 == 0) QCoreApplication::processEvents();
            QString muxer = muxers[i];
            emit progress(_prevMax + i);

            QRegularExpressionMatch match = re.match(muxer);
            if (match.hasMatch())
            {
                QString name = match.captured(1).trimmed();
                QString prettyName = match.captured(2).trimmed();

#ifdef FFMPEG_VERBOSE_DEBUG
                emit newLog("Loading muxer: " + name);
#endif

                // skip image sequence
                if (name == "image2") continue;

                FFMuxer *m = new FFMuxer(name,prettyName,this);
                _muxers << m;
                _decodeMuxers << m;
                _encodeMuxers << m;

                //save to settings
                n++;
                settings.setArrayIndex( n );
                settings.setValue("name", name);
                settings.setValue("prettyName", prettyName);

                //get default codecs
                QStringList args("-hide_banner");
                args << "-h";
                args << "muxer=" + m->name();
                if (runCommand(args,3000))
                {
                    QStringList lines = _output.split("\n");

                    QRegularExpression reVideo("Default video codec:\\s*(.+)\\.");
                    QRegularExpression reAudio("Default audio codec:\\s*(.+)\\.");
                    QRegularExpression reExtensions("Common extensions:\\s*(.+)\\.");

                    foreach(QString line,lines)
                    {
                        //video codec
                        QRegularExpressionMatch videoMatch = reVideo.match(line);
                        if (videoMatch.hasMatch())
                        {
                            QString defltVideoCodec = videoMatch.captured(1);
                            m->setDefaultVideoCodec(videoEncoder( defltVideoCodec ));
                            settings.setValue("defaultVideoCodec", defltVideoCodec );
                        }

                        //audio codec
                        QRegularExpressionMatch audioMatch = reAudio.match(line);
                        if (audioMatch.hasMatch())
                        {
                            QString defltAudioCodec = audioMatch.captured(1);
                            m->setDefaultAudioCodec(audioEncoder( defltAudioCodec ));
                            settings.setValue("defaultAudioCodec", defltAudioCodec );
                        }

                        //extensions
                        QRegularExpressionMatch extensionsMatch = reExtensions.match(line);
                        if (extensionsMatch.hasMatch())
                        {
                            QString extensions = extensionsMatch.captured(1);
                            m->setExtensions(extensions.split(","));
                            settings.setValue("extensions", extensions );
                        }
                    }
                }
            }
        }

        settings.endArray();
        _prevMax = _prevMax + max;
    }

    //add image sequences
    QStringList extensions;

    emit newLog("Image sequences muxers...");

    FFMuxer *muxer = new FFMuxer("exr","openEXR Sequence");
    muxer->setSequence(true);
    extensions << "exr" << "exr_pipe";
    muxer->setExtensions( extensions );
    extensions.clear();
    muxer->setDefaultVideoCodec(videoDecoder("exr"));
    muxer->setDecodeOnly(true);
    _muxers << muxer;
    _decodeMuxers << muxer;

    muxer = new FFMuxer("bmp","Bitmap Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("bmp"));
    muxer->setDefaultVideoCodec(videoEncoder("bmp"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("dpx","DPX Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("dpx"));
    muxer->setDefaultVideoCodec(videoEncoder("dpx"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("mjpeg","JPEG Sequence");
    muxer->setSequence(true);
    extensions << "jpg" << "jpeg";
    muxer->setExtensions(extensions);
    extensions.clear();
    muxer->setDefaultVideoCodec(videoEncoder("mjpeg"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("jpegls","Lossless JPEG Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("ljpg"));
    muxer->setDefaultVideoCodec(videoEncoder("jpegls"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("pam","PAM (Portable AnyMap) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("pam"));
    muxer->setDefaultVideoCodec(videoEncoder("pam"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("pbm","PBM (Portable BitMap) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("pbm"));
    muxer->setDefaultVideoCodec(videoEncoder("pbm"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("pcx","PC Paintbrush PCX Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("pcx"));
    muxer->setDefaultVideoCodec(videoEncoder("pcx"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("pgm","PGM (Portable GrayMap) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("pgm"));
    muxer->setDefaultVideoCodec(videoEncoder("pgm"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("pgmyuv","PGMYUV (Portable GrayMap YUV) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("pgmyuv"));
    muxer->setDefaultVideoCodec(videoEncoder("pgmyuv"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("png","PNG (Portable Network Graphics) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("png"));
    muxer->setDefaultVideoCodec(videoEncoder("png"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("ppm","PPM (Portable PixelMap) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("ppm"));
    muxer->setDefaultVideoCodec(videoEncoder("ppm"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("sgi","SGI Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("sgi"));
    muxer->setDefaultVideoCodec(videoEncoder("sgi"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("targa","TARGA (Truevision Targa) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("tga"));
    muxer->setDefaultVideoCodec(videoEncoder("targa"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("tiff","TIFF Sequence");
    muxer->setSequence(true);
    extensions << "tif" << "tiff";
    muxer->setExtensions(extensions);
    extensions.clear();
    muxer->setDefaultVideoCodec(videoEncoder("tiff"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("jpeg2000","JPEG 2000 Sequence");
    muxer->setSequence(true);
    extensions << "jp2" << "j2k";
    muxer->setExtensions(extensions);
    extensions.clear();
    muxer->setDefaultVideoCodec(videoEncoder("jpeg2000"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("xwd","XWD (X Window Dump) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("xwd"));
    muxer->setDefaultVideoCodec(videoEncoder("xwd"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;

    muxer = new FFMuxer("xbm","XBM (X BitMap) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("xbm"));
    muxer->setDefaultVideoCodec(videoEncoder("xbm"));
    _muxers << muxer;
    _decodeMuxers << muxer;
    _encodeMuxers << muxer;


    emit newLog("Sorting muxers...");
    std::sort(_muxers.begin(),_muxers.end(),muxerSorter);
    std::sort(_decodeMuxers.begin(),_decodeMuxers.end(),muxerSorter);
    std::sort(_encodeMuxers.begin(),_encodeMuxers.end(),muxerSorter);
}

bool ffSorter(FFBaseObject *c1,FFBaseObject *c2)
{
    return c1->prettyName().toLower() < c2->prettyName().toLower();
}

QString FFmpeg::gotVersion(QString output)
{
    QSettings settings;
    QString v = "";

    //ffmpeg version (\S+)
    QRegularExpression re("ffmpeg version (\\S+)");
    QRegularExpressionMatch match = re.match(output);
    if (match.hasMatch())
    {
        v = match.captured(1);
        emit newLog("Current version: " + v);
    }
    else
    {
        emit newLog("FFmpeg executable binary not found.\nYou can download it at http://ffmpeg.org", DuQFLog::Critical );
    }

    return v;
}

void FFmpeg::gotCodecs(QString output, QString newVersion )
{
    QSettings settings;

    //delete all
    qDeleteAll(_videoEncoders);
    qDeleteAll(_audioEncoders);
    qDeleteAll(_videoDecoders);
    qDeleteAll(_audioDecoders);
    _videoEncoders.clear();
    _audioEncoders.clear();
    _videoDecoders.clear();
    _audioDecoders.clear();

    //add copy
    emit newLog("Loading codec: video copy");
    FFCodec *copyVideo = new FFCodec("copy","Copy video stream",FFCodec::Video | FFCodec::Encoder | FFCodec::Lossless | FFCodec::Lossy | FFCodec::IFrame,this);
    _videoEncoders << copyVideo;
    emit newLog("Loading codec: audio copy");
    FFCodec *copyAudio = new FFCodec("copy","Copy audio stream",FFCodec::Audio | FFCodec::Encoder | FFCodec::Lossless | FFCodec::Lossy | FFCodec::IFrame,this);
    _audioEncoders << copyAudio;

    qDebug() << newVersion + " - " + _version;

    //if the version is the same as previously, just read data from settings
    if (newVersion == _version)
    {
        emit newLog("Getting codecs from stored list.");

        //open array
        int arraySize = settings.beginReadArray("ffmpeg/codecs");

        _progressMax = _prevMax + arraySize + arraySize/1.5; //pixfmts + codecs + estimated muxers
        emit progressMax( _progressMax );

        for (int i = 0; i < arraySize; i++)
        {
            settings.setArrayIndex(i);

            emit progress( _prevMax + i);

            QString codecName = settings.value("codecName").toString();
            QString codecPrettyName = settings.value("codecPrettyName").toString();
            FFCodec *co = new FFCodec(codecName,codecPrettyName,this);

            //emit newLog("Loading codec: " + codecName);

            bool decoder = settings.value("decoder").toBool();
            bool encoder = settings.value("encoder").toBool();
            bool video = settings.value("video").toBool();
            bool audio = settings.value("audio").toBool();
            bool iFrame = settings.value("iFrame").toBool();
            bool lossy = settings.value("lossy").toBool();
            bool lossless = settings.value("lossless").toBool();

            co->setDecoder( decoder );
            co->setEncoder( encoder );
            co->setVideo( video );
            co->setAudio( audio );
            co->setIframe( iFrame );
            co->setLossy( lossy );
            co->setLossless( lossless );

            if (co->isVideo())
            {
                //get pixel formats
                int pArraySize = settings.beginReadArray("pixFmts");
                for (int j = 0; j < pArraySize; j++)
                {
                    settings.setArrayIndex(j);

                    QString pixFmt = settings.value("name").toString();

                    FFPixFormat *pf = pixFormat(pixFmt);
                    if (pf->name() == "") continue;

                    co->addPixFormat(pf);

                    bool deflt = settings.value("default").toBool();
                    if ( deflt ) co->setDefaultPixFormat(pf);
                }

                settings.endArray();
            }

            if (co->isVideo() && co->isEncoder()) _videoEncoders << co;
            else if (co->isAudio() && co->isEncoder()) _audioEncoders << co;
            else if (co->isVideo() && co->isDecoder()) _videoDecoders << co;
            else if (co->isAudio() && co->isDecoder()) _audioDecoders << co;
        }

        //close the array
        settings.endArray();

        _prevMax = _prevMax + arraySize;
    }

    //if the version has changed, or if we did not get the list from settings
    if (newVersion != _version || _audioEncoders.count() < 2 || _videoEncoders.count() < 2)
    {
        emit newLog("Updating codec list...");

        QStringList codecs = output.split("\n");
        QRegularExpression re("([D.])([E.])([VAS])([I.])([L.])([S.]) (\\w+) +([^\\(\\n]+)");

        int max = codecs.count();
        _progressMax = _prevMax + max + max/1.5; //pixfmts + codecs + estimated muxers
        emit progressMax( _progressMax );

        //remove the previous codecs
        settings.remove("ffmpeg/codecs");
        //create the new array
        settings.beginWriteArray("ffmpeg/codecs");
        int n = -1;

        for (int i = 0 ; i < codecs.count() ; i++)
        {
            if (i%10 == 0) QCoreApplication::processEvents();
            QString codec = codecs[i];
            emit progress( _prevMax + i );

            QRegularExpressionMatch match = re.match(codec);
            if (match.hasMatch())
            {
                QString codecName = match.captured(7);
                QString codecPrettyName = match.captured(8);
                FFCodec *co = new FFCodec(codecName,codecPrettyName,this);

#ifdef FFMPEG_VERBOSE_DEBUG
                emit newLog("Loading codec: " + codecName);
#endif

                bool decoder = match.captured(1) == "D";
                bool encoder = match.captured(2) == "E";
                bool video = match.captured(3) == "V";
                bool audio = match.captured(3) == "A";
                bool iFrame = match.captured(4) == "I";
                bool lossy = match.captured(5) == "L";
                bool lossless = match.captured(6) == "S";

                co->setDecoder( decoder );
                co->setEncoder( encoder );
                co->setVideo( video );
                co->setAudio( audio );
                co->setIframe( iFrame );
                co->setLossy( lossy );
                co->setLossless( lossless );

                //save it to settings
                n++;
                settings.setArrayIndex(n);
                settings.setValue("codecName", codecName);
                settings.setValue("codecPrettyName", codecPrettyName);

                settings.setValue("decoder", decoder);
                settings.setValue("encoder", encoder);
                settings.setValue("video", video);
                settings.setValue("audio", audio);
                settings.setValue("iFrame", iFrame);
                settings.setValue("lossy", lossy);
                settings.setValue("lossless", lossless);

                if (co->isVideo())
                {
                    //get pixel formats
                    QStringList args("-hide_banner");
                    args << "-h";
                    if (co->isEncoder()) args << "encoder=" + co->name();
                    else args << "decoder=" + co->name();
                    if ( runCommand(args, 3000))
                    {
                        QStringList lines = _output.split("\n");
                        if (lines.count() > 0)
                        {
                            QRegularExpression rePixFmt("Supported pixel formats: (.+)");

                            foreach(QString line,lines)
                            {
                                QRegularExpressionMatch pixFmtMatch = rePixFmt.match(line);
                                if (pixFmtMatch.hasMatch())
                                {
                                    //create array for supported pixfmts
                                    settings.beginWriteArray("pixFmts");

                                    QStringList pixFmts = pixFmtMatch.captured(1).split(" ");
                                    QString defaultPF = pixFmts[0];
                                    pixFmts.sort();
                                    int num = 0;
                                    foreach ( QString pfStr, pixFmts )
                                    {
                                        QString pixFmt = pfStr.trimmed();

                                        FFPixFormat *pf = pixFormat(pixFmt);
                                        if (pf->name() == "") continue;
                                        co->addPixFormat(pf);
                                        bool deflt = pixFmt == defaultPF;
                                        if ( deflt ) co->setDefaultPixFormat(pf);

                                        settings.setArrayIndex( num );
                                        num++;
                                        settings.setValue("name",pixFmt);
                                        settings.setValue("default",deflt);
                                    }

                                    settings.endArray();

                                    break;
                                }
                            }
                        }
                    }
                }

                if (co->isVideo() && co->isEncoder()) _videoEncoders << co;
                else if (co->isAudio() && co->isEncoder()) _audioEncoders << co;
                else if (co->isVideo() && co->isDecoder()) _videoDecoders << co;
                else if (co->isAudio() && co->isDecoder()) _audioDecoders << co;
            }
        }

        //close array
        settings.endArray();
        _prevMax = _prevMax + max;
    }

    qDebug() << "Found " + QString::number(_audioEncoders.count()) + " audio encoders and " + QString::number(_videoEncoders.count()) + " video encoders.";

    emit newLog("Sorting Codecs...");
    std::sort(_videoEncoders.begin(),_videoEncoders.end(),ffSorter);
    std::sort(_audioEncoders.begin(),_audioEncoders.end(),ffSorter);
}

void FFmpeg::gotPixFormats(QString output, QString newVersion)
{
    QSettings settings;
    //delete all
    qDeleteAll(_pixFormats);
    _pixFormats.clear();

    //if the version is the same as previously, just read data from settings
    if (newVersion == _version)
    {
        //open array
        int arraySize = settings.beginReadArray("ffmpeg/pixFmts");
        _progressMax = arraySize + arraySize*2 + arraySize/3; //pixfmts + estimated codecs + estimated muxers
        _prevMax = arraySize;
        emit progressMax( _progressMax );

        for (int i = 0; i < arraySize; ++i)
        {
            settings.setArrayIndex(i);
            emit progress(i);


            QString name = settings.value("name").toString();
            int numComponents = settings.value("numComponents").toInt();
            int bpp = settings.value("bpp").toInt();

            FFPixFormat *pf = new FFPixFormat(name, "", numComponents, bpp);

            //emit newLog("Loading pixel format: " + name);

            bool input = settings.value("input").toBool();
            bool output = settings.value("output").toBool();
            bool hardware = settings.value("hardware").toBool();
            bool paletted = settings.value("paletted").toBool();
            bool bitStream = settings.value("bitStream").toBool();

            pf->setInput( input );
            pf->setOutput( output );
            pf->setHardware( hardware );
            pf->setPaletted( paletted );
            pf->setBitstream( bitStream );

            _pixFormats << pf;
        }

        //close the array
        settings.endArray();
    }

    //if the version has changed, or if we did not get the list from settings
    if (newVersion != _version || _pixFormats.count() == 0)
    {
        emit newLog("Updating pixel formats list...");
        QStringList pixFmts = output.split("\n");
        int max = pixFmts.count();
        _progressMax = max + max*2 + max/3; //pixfmts + estimated codecs + estimated muxers
        _prevMax = max;
        emit progressMax( _progressMax );
        QRegularExpression re("([I.])([O.])([H.])([P.])([B.]) (\\w+) +(\\d+) +(\\d+)");

        //remove the previous formats
        settings.remove("ffmpeg/pixFmts");
        //create the new array
        settings.beginWriteArray("ffmpeg/pixFmts");

        int n = -1;

        for (int i = 0 ; i < pixFmts.count() ; i++)
        {
            if (i%10 == 0) QCoreApplication::processEvents();
            QString pixFmt = pixFmts[i];

            emit progress(i);

            QRegularExpressionMatch match = re.match(pixFmt);
            if (match.hasMatch())
            {
                QString name = match.captured(6);
                QString numComponents = match.captured(7);
                QString bpp = match.captured(8);
                FFPixFormat *pf = new FFPixFormat(name, "", numComponents.toInt(), bpp.toInt());
#ifdef FFMPEG_VERBOSE_DEBUG
                emit newLog("Loading pixel format: " + name);
#endif

                bool input = match.captured(1) == "I";
                bool output = match.captured(2) == "O";
                bool hardware = match.captured(3) == "H";
                bool paletted = match.captured(4) == "P";
                bool bitStream = match.captured(5) == "B";

                pf->setInput( input );
                pf->setOutput( output );
                pf->setHardware( hardware );
                pf->setPaletted( paletted );
                pf->setBitstream( bitStream );

                _pixFormats << pf;

                //save it to settings
                n++;
                settings.setArrayIndex(n);
                settings.setValue("name", name);
                settings.setValue("numComponents", numComponents);
                settings.setValue("bpp", bpp);

                settings.setValue("input", input);
                settings.setValue("output", output);
                settings.setValue("hardware", hardware);
                settings.setValue("paletted", paletted);
                settings.setValue("bitStream", bitStream);
            }
        }

        //close the array
        settings.endArray();
    }

    emit newLog("Sorting pixel formats...");
    std::sort(_pixFormats.begin(),_pixFormats.end(),ffSorter);
}

void FFmpeg::gotSampleFormats(QString output, QString newVersion)
{
    QSettings settings;
    //delete all
    qDeleteAll(_sampleFormats);
    _sampleFormats.clear();

    //if the version is the same as previously, just read data from settings
    if (newVersion == _version)
    {
        //open array
        int arraySize = settings.beginReadArray("ffmpeg/sampleFmts");

        for (int i = 0; i < arraySize; ++i)
        {
            settings.setArrayIndex(i);

            QString name = settings.value("name").toString();
            int depth = settings.value("depth").toInt();

            FFSampleFormat *sf = new FFSampleFormat(name, depth);

            //emit newLog("Loading sample format: " + name);

            _sampleFormats << sf;
        }

        //close the array
        settings.endArray();
    }

    //if the version has changed, or if we did not get the list from settings
    if (newVersion != _version || _sampleFormats.count() == 0)
    {
        emit newLog("Updating sample formats list...");
        QStringList sampleFmts = output.split("\n");

        QRegularExpression re("(\\w+)\\s+(\\d+)");

        //remove the previous formats
        settings.remove("ffmpeg/sampleFmts");
        //create the new array
        settings.beginWriteArray("ffmpeg/sampleFmts");

        int n = -1;

        for (int i = 0 ; i < sampleFmts.count() ; i++)
        {
            if (i%10 == 0) QCoreApplication::processEvents();
            QString sampleFmt = sampleFmts[i];

            QRegularExpressionMatch match = re.match(sampleFmt);
            if (match.hasMatch())
            {
                QString name = match.captured(1);
                int depth = match.captured(2).toInt();

                FFSampleFormat *sf = new FFSampleFormat(name, depth);
#ifdef FFMPEG_VERBOSE_DEBUG
                emit newLog("Loading pixel format: " + name);
#endif
                _sampleFormats << sf;

                //save it to settings
                n++;
                settings.setArrayIndex(n);
                settings.setValue("name", name);
                settings.setValue("depth", depth);
            }
        }

        //close the array
        settings.endArray();
    }

    emit newLog("Sorting sample formats...");
    std::sort(_sampleFormats.begin(),_sampleFormats.end(),ffSorter);
}

FFmpeg *FFmpeg::_instance = nullptr;
