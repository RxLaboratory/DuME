#include "ffmpeg.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif


FFmpeg::FFmpeg(QString path,QObject *parent) : AbstractRendererInfo(parent)
{
    _status = MediaUtils::Initializing;

    // Defaults
    _defaultPixFormat = FFPixFormat::getDefault( this );
    _defaultCodec = FFCodec::getDefault( this );
    _defaultMuxer = FFMuxer::getDefault( this );

    // The list of Profiles
    _profiles << new FFProfile("", "Auto");
    _profiles << new FFProfile("0", "Proxy");
    _profiles << new FFProfile("1", "LT");
    _profiles << new FFProfile("2", "SQ");
    _profiles << new FFProfile("3", "HQ");
    _profiles << new FFProfile("baseline", "Baseline");
    _profiles << new FFProfile("main", "Main");
    _profiles << new FFProfile("high", "High");
    _profiles << new FFProfile("high10", "High 10");
    _profiles << new FFProfile("high422", "High 422");
    _profiles << new FFProfile("high444", "High 444");

    // The color TRCs
    _colorTRCs << new FFBaseObject("", "Auto");
    _colorTRCs << new FFBaseObject("iec61966_2_1", "sRGB");
    _colorTRCs << new FFBaseObject("iec61966_2_4", "Extended-gamut YCC (xvYCC)");
    _colorTRCs << new FFBaseObject("linear", "Linear");
    _colorTRCs << new FFBaseObject("log100", "Log");
    _colorTRCs << new FFBaseObject("log_sqrt", "Log square root");
    _colorTRCs << new FFBaseObject("bt709", "BT.709");
    _colorTRCs << new FFBaseObject("gamma22", "BT.470 M");
    _colorTRCs << new FFBaseObject("gamma28", "BT.470 BG");
    _colorTRCs << new FFBaseObject("bt1361","BT.1361");
    _colorTRCs << new FFBaseObject("bt1361e","BT.1361 E");
    _colorTRCs << new FFBaseObject("bt2020_10","BT.2020 - 10 bit");
    _colorTRCs << new FFBaseObject("bt2020_12","BT.2020 - 12 bit");
    _colorTRCs << new FFBaseObject("smpte170m","SMPTE 170 M");
    _colorTRCs << new FFBaseObject("smpte240m","SMPTE 240 M");
    _colorTRCs << new FFBaseObject("smpte428","SMPTE 428");
    _colorTRCs << new FFBaseObject("smpte428_1","SMPTE 428-1");
    _colorTRCs << new FFBaseObject("smpte2084","SMPTE 2084");

    // The color Ranges
    _colorRanges << new FFBaseObject("","Auto");
    _colorRanges << new FFBaseObject("tv","Limited");
    _colorRanges << new FFBaseObject("pc","Full");

    // The color Spaces
    _colorSpaces << new FFBaseObject("","Auto");
    _colorSpaces << new FFBaseObject("rgb","RGB");
    _colorSpaces << new FFBaseObject("bt709","BT.709");
    _colorSpaces << new FFBaseObject("bt470bg","BT.470 BG");
    _colorSpaces << new FFBaseObject("bt2020_ncl","BT.2020 NCL");
    _colorSpaces << new FFBaseObject("bt2020_cl","BT.2020 CL");
    _colorSpaces << new FFBaseObject("smpte2085","SMPTE 2085");
    _colorSpaces << new FFBaseObject("smpte170m","SMPTE 170 M");
    _colorSpaces << new FFBaseObject("smpte240m","SMPTE 240 M");
    _colorSpaces << new FFBaseObject("ycgco","YCGCO");
    _colorSpaces << new FFBaseObject("ycocg","YCOCG");
    _colorSpaces << new FFBaseObject("fcc","FCC");

    // The color Primaries
    _colorPrimaries << new FFBaseObject("","Auto");
    _colorPrimaries << new FFBaseObject("film","Film");
    _colorPrimaries << new FFBaseObject("bt709","BT.709 / RGB");
    _colorPrimaries << new FFBaseObject("bt470m","BT.470 M");
    _colorPrimaries << new FFBaseObject("bt470bg","BT.470 BG");
    _colorPrimaries << new FFBaseObject("bt2020","BT.2020");
    _colorPrimaries << new FFBaseObject("smpte170m","SMPTE 170 M");
    _colorPrimaries << new FFBaseObject("smpte240m","SMPTE 240 M");
    _colorPrimaries << new FFBaseObject("smpte428","SMPTE 428");
    _colorPrimaries << new FFBaseObject("smpte428_1","SMPTE 428-1");
    _colorPrimaries << new FFBaseObject("smpte431","SMPTE 431");
    _colorPrimaries << new FFBaseObject("smpte432","SMPTE 432");
    _colorPrimaries << new FFBaseObject("jedec-p22","JEDEC P22");

    // The complete color Profiles
    _colorProfiles << new FFColorProfile("", "Auto", colorPrimary(""), colorTRC(""), colorSpace(""), colorRange(""));
    _colorProfiles << new FFColorProfile("srgb", "Images (sRGB)", colorPrimary("bt709"), colorTRC("iec61966_2_1"), colorSpace("rgb"), colorRange("pc"));
    _colorProfiles << new FFColorProfile("bt709", "HD Video (BT.709)", colorPrimary("bt709"), colorTRC("bt709"), colorSpace("bt709"), colorRange("tv"));
    _colorProfiles << new FFColorProfile("bt2020_10", "UHD (4K/8K) Video (BT.2020-10bits)", colorPrimary("bt2020"), colorTRC("bt2020_10"), colorSpace("bt2020_cl"), colorRange("pc"));
    _colorProfiles << new FFColorProfile("bt2020_12", "UHD (4K/8K) HDR Video (BT.2020-12bits)", colorPrimary("bt2020"), colorTRC("bt2020_12"), colorSpace("bt2020_cl"), colorRange("pc"));

    if (path != "") setBinary(path, false);
}

FFmpeg::~FFmpeg()
{

}

bool FFmpeg::setBinary(QString path, bool initialize)
{
    if ( path == binary() ) return false;
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
    QString defaultFFmpegPath = "ffmpeg";
#endif
#ifdef Q_OS_WIN
    QString defaultFFmpegPath = QCoreApplication::applicationDirPath() + "/ffmpeg.exe";
#endif
#ifdef Q_OS_MAC
    QString defaultFFmpegPath = QCoreApplication::applicationDirPath() + "/ffmpeg";
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
}

QList<FFColorProfile *> FFmpeg::colorProfiles() const
{
    return _colorProfiles;
}

QList<FFBaseObject *> FFmpeg::colorRanges() const
{
    return _colorRanges;
}

QList<FFBaseObject *> FFmpeg::colorSpaces() const
{
    return _colorSpaces;
}

QList<FFBaseObject *> FFmpeg::colorTRCs() const
{
    return _colorTRCs;
}

QList<FFBaseObject *> FFmpeg::colorPrimaries() const
{
    return _colorPrimaries;
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
        if (pf->name().toLower() == name.trimmed().toLower()) return pf;
    }
    foreach(FFPixFormat *pf,_pixFormats)
    {
        if (pf->prettyName() == name) return pf;
    }
    return _defaultPixFormat;
}

FFProfile *FFmpeg::profile(QString name)
{
    name = name.trimmed();
    foreach(FFProfile *pf,_profiles)
    {
        if (pf->name().toLower() == name.trimmed().toLower()) return pf;
    }
    foreach(FFProfile *pf,_profiles)
    {
        if (pf->prettyName() == name) return pf;
    }
    return _profiles[0];
}

FFBaseObject *FFmpeg::colorTRC(QString name)
{
    name = name.trimmed();
    foreach(FFBaseObject *c,_colorTRCs)
    {
        if (c->name().toLower() == name.trimmed().toLower()) return c;
    }
    foreach(FFBaseObject *c,_colorTRCs)
    {
        if (c->prettyName() == name) return c;
    }
    return _colorTRCs[0];
}

FFBaseObject *FFmpeg::colorPrimary(QString name)
{
    name = name.trimmed();
    foreach(FFBaseObject *c,_colorPrimaries)
    {
        if (c->name().toLower() == name.trimmed().toLower()) return c;
    }
    foreach(FFBaseObject *c,_colorPrimaries)
    {
        if (c->prettyName() == name) return c;
    }
    return _colorPrimaries[0];
}

FFBaseObject *FFmpeg::colorSpace(QString name)
{
    name = name.trimmed();
    foreach(FFBaseObject *c,_colorSpaces)
    {
        if (c->name().toLower() == name.trimmed().toLower()) return c;
    }
    foreach(FFBaseObject *c,_colorSpaces)
    {
        if (c->prettyName() == name) return c;
    }
    return _colorSpaces[0];
}

FFBaseObject *FFmpeg::colorRange(QString name)
{
    name = name.trimmed();
    foreach(FFBaseObject *c,_colorRanges)
    {
        if (c->name().toLower() == name.trimmed().toLower()) return c;
    }
    foreach(FFBaseObject *c,_colorRanges)
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
    //delete all
    qDeleteAll(_muxers);
    _muxers.clear();

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
        emit newLog("Updating muxers list from FFmpeg.");

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
            QString muxer = muxers[i];
            emit progress(_prevMax + i);

            QRegularExpressionMatch match = re.match(muxer);
            if (match.hasMatch())
            {
                QString name = match.captured(1).trimmed();
                QString prettyName = match.captured(2).trimmed();

                emit newLog("Loading muxer: " + name);

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
        emit newLog("FFmpeg executable binary not found.\nYou can download it at http://ffmpeg.org", LogUtils::Critical );
    }

    return v;
}

void FFmpeg::gotCodecs(QString output, QString newVersion )
{
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
                //add profiles
                if( co->name().startsWith("prores") )
                {
                    co->addProfile( profile("0") );
                    co->addProfile( profile("1") );
                    co->addProfile( profile("2") );
                    co->addProfile( profile("3") );
                }
                else if (co->name() == "h264")
                {
                    co->addProfile( profile("baseline") );
                    co->addProfile( profile("main") );
                    co->addProfile( profile("high") );
                    co->addProfile( profile("high10") );
                    co->addProfile( profile("high422") );
                    co->addProfile( profile("high444") );
                }

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
        emit newLog("Updating codec list from FFmpeg.");

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
            QString codec = codecs[i];
            emit progress( _prevMax + i );

            QRegularExpressionMatch match = re.match(codec);
            if (match.hasMatch())
            {
                QString codecName = match.captured(7);
                QString codecPrettyName = match.captured(8);
                FFCodec *co = new FFCodec(codecName,codecPrettyName,this);

                emit newLog("Loading codec: " + codecName);

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
                    //add profiles
                    if( co->name().startsWith("prores") )
                    {
                        co->addProfile( profile("0") );
                        co->addProfile( profile("1") );
                        co->addProfile( profile("2") );
                        co->addProfile( profile("3") );
                    }
                    else if (co->name() == "h264")
                    {
                        co->addProfile( profile("baseline") );
                        co->addProfile( profile("main") );
                        co->addProfile( profile("high") );
                        co->addProfile( profile("high10") );
                        co->addProfile( profile("high422") );
                        co->addProfile( profile("high444") );
                    }

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


    emit newLog("Sorting Codecs...");
    std::sort(_videoEncoders.begin(),_videoEncoders.end(),ffSorter);
    std::sort(_audioEncoders.begin(),_audioEncoders.end(),ffSorter);
}

void FFmpeg::gotPixFormats(QString output, QString newVersion)
{
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
        emit newLog("Updating pixel formats list from FFmpeg.");
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
            QString pixFmt = pixFmts[i];

            emit progress(i);

            QRegularExpressionMatch match = re.match(pixFmt);
            if (match.hasMatch())
            {
                QString name = match.captured(6);
                QString numComponents = match.captured(7);
                QString bpp = match.captured(8);
                FFPixFormat *pf = new FFPixFormat(name, "", numComponents.toInt(), bpp.toInt());

                emit newLog("Loading pixel format: " + name);

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

