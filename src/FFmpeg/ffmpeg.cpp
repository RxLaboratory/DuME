#include "ffmpeg.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif


FFmpeg::FFmpeg(QString path,QObject *parent) : AbstractRendererInfo(parent)
{
    _status = MediaUtils::Initializing;
    _lastErrorMessage = "";

    // The Process
    ffmpegProcess = new QProcess(this);

    // The output
    _ffmpegOutput = "";

    //Connect process
    connect(ffmpegProcess,SIGNAL(readyReadStandardError()),this,SLOT(stdError()));
    connect(ffmpegProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(stdOutput()));
    connect(ffmpegProcess,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(errorOccurred(QProcess::ProcessError)));

    //TODO auto find ffmpeg if no settings or path invalid
    QSettings settings;

#ifdef Q_OS_LINUX
    QString defaultFFmpegPath = "ffmpeg";
#endif
#ifdef Q_OS_WIN
    QString defaultFFmpegPath = "ffmpeg.exe";
#endif

    if (path == "") setBinary(settings.value("ffmpeg/path",defaultFFmpegPath).toString(), false);
    else setBinary(path, false);

    _version = settings.value("ffmpeg/version","").toString();
}

FFmpeg::~FFmpeg()
{

}

bool FFmpeg::setBinary(QString path, bool initialize)
{
    if ( AbstractRendererInfo::setBinary(path) )
    {
        ffmpegProcess->setProgram(path);
        if (initialize) init();
        emit binaryChanged( path );
        return true;
    }

    return false;
}

void FFmpeg::runCommand(QString commands, QIODevice::OpenModeFlag of)
{
    //detect arguments
    QRegularExpression re("(\"[^\"]*\"|[\\S]+)");
    QRegularExpressionMatchIterator i = re.globalMatch(commands);
    QStringList commandList;
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString command = match.captured(1);
        command.replace("\"","");
        commandList << command;
    }
    runCommand(commandList, of);
}

void FFmpeg::runCommand(QStringList commands, QIODevice::OpenModeFlag of)
{
    _ffmpegOutput = "";
    ffmpegProcess->setArguments(commands);
    ffmpegProcess->start(of);
}

void FFmpeg::init()
{   
#if INIT_FFMPEG //used when developping to skip ffmpeg loading
    _status = MediaUtils::Initializing;

    //get version
    emit newLog( "Checking version" );
    runCommand( "" );
    QString newVersion = "";
    if (ffmpegProcess->waitForFinished(1000))
    {
        newVersion = gotVersion(_ffmpegOutput);
    }

    //get pixFormats
    emit newLog( "Loading Pixel Formats" );
    runCommand( "-pix_fmts" );
    if (ffmpegProcess->waitForFinished(10000))
    {
        gotPixFormats( _ffmpegOutput, newVersion );
    }

    //get codecs
    emit newLog( "Loading Codecs" );
    runCommand( "-codecs" );
    if (ffmpegProcess->waitForFinished(10000))
    {
        gotCodecs( _ffmpegOutput, newVersion );
    }

    //get muxers
    emit newLog( "Loading Muxers" );
    runCommand( "-formats" );
    if (ffmpegProcess->waitForFinished(10000))
    {
        gotMuxers( _ffmpegOutput, newVersion );
    }

    //get long help
    emit newLog( "Loading Documentation" );
    runCommand( "-h long" );
    if (ffmpegProcess->waitForFinished(3000))
    {
        _longHelp = _ffmpegOutput;
    }

    //get help
    _help = settings.value("ffmpeg/help","").toString();
    runCommand( "-h" );
    if (ffmpegProcess->waitForFinished(3000))
    {
        _help = _ffmpegOutput;
    }

    _version = newVersion;
    settings.setValue("ffmpeg/version",_version);

#endif
    _status = MediaUtils::Waiting;
}

QList<FFMuxer *> FFmpeg::muxers()
{
    return _muxers;
}

FFMuxer *FFmpeg::muxer(QString name)
{
    foreach(FFMuxer *muxer,_muxers)
    {
        if (muxer->name() == name)
        {
            return muxer;
        }
    }
    return nullptr;
}

FFCodec *FFmpeg::muxerDefaultCodec(FFMuxer *muxer, FFCodec::Ability ability)
{
    FFCodec *videoCodec = muxer->defaultVideoCodec();
    FFCodec *audioCodec = muxer->defaultAudioCodec();

    //return
    if (ability == FFCodec::Video) return videoCodec;
    if (ability == FFCodec::Audio) return audioCodec;

    return nullptr;
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
        if (codec->name() == name) return codec;
    }
    return nullptr;
}

FFCodec *FFmpeg::audioEncoder(QString name)
{
    foreach(FFCodec *codec,_audioEncoders)
    {
        if (codec->name() == name) return codec;
    }
    return nullptr;
}

FFPixFormat *FFmpeg::pixFormat(QString name)
{
    foreach(FFPixFormat *pf,_pixFormats)
    {
        if (pf->name() == name) return pf;
    }
    return nullptr;
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
    QStringList args("-i");
    args << QDir::toNativeSeparators(mediaPath);
    ffmpegProcess->setArguments(args);
    ffmpegProcess->start(QIODevice::ReadOnly);
    if (ffmpegProcess->waitForFinished(3000))
    {
        return _ffmpegOutput;
    }
    return "";
}

void FFmpeg::stdError()
{
    QString output = ffmpegProcess->readAllStandardError();
    readyRead(output);
}

void FFmpeg::stdOutput()
{
    QString output = ffmpegProcess->readAllStandardOutput();
    readyRead(output);
}

void FFmpeg::errorOccurred(QProcess::ProcessError e)
{
    QString error;
    if (e == QProcess::FailedToStart)
    {
        error = "Failed to start process.";
    }
    else if (e == QProcess::Crashed)
    {
        error = "Process just crashed.";
    }
    else if (e == QProcess::Timedout)
    {
        error = "Process operation timed out.";
    }
    else if (e == QProcess::WriteError)
    {
        error = "Process write Error.";
    }
    else if (e == QProcess::ReadError)
    {
        error = "Cannot read process output.";
    }
    else if (e == QProcess::UnknownError)
    {
        error = "An unknown process error occured.";
    }

    emit newLog( error, LogUtils::Critical );
    _lastErrorMessage = error;
    _status = MediaUtils::Error;
}

QString FFmpeg::lastErrorMessage() const
{
    return _lastErrorMessage;
}

MediaUtils::Status FFmpeg::status() const
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

    //if the version is different, get from the output
    if (newVersion != _version)
    {
        emit newLog("Updating muxers list.");

        QStringList muxers = output.split("\n");
        QRegularExpression re("[D. ]E (\\w+)\\s+(.+)");

        //remove the previous codecs
        settings.remove("ffmpeg/muxers");
        //create the new array
        settings.beginWriteArray("ffmpeg/muxers");

        for (int i = 0 ; i < muxers.count() ; i++)
        {
            QString muxer = muxers[i];
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

                //save to settings
                settings.setArrayIndex(i);
                settings.setValue("name", name);
                settings.setValue("prettyName", prettyName);

                //get default codecs
                QStringList args("-h");
                args << "muxer=" + m->name();
                ffmpegProcess->setArguments(args);
                ffmpegProcess->start(QIODevice::ReadOnly);

                if (ffmpegProcess->waitForFinished(3000))
                {
                    QStringList lines = _ffmpegOutput.split("\n");

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
    }
    else //other wise, get from settings
    {
        emit newLog("Getting muxers from stored list.");

        //open array
        int arraySize = settings.beginReadArray("ffmpeg/muxers");

        for (int i = 0; i < arraySize; i++)
        {
            settings.setArrayIndex(i);

            QString name = settings.value("name").toString();
            QString prettyName = settings.value("prettyName").toString();

            emit newLog("Loading muxer:" + name);
            if (name == "") continue;

            FFMuxer *m = new FFMuxer(name,prettyName,this);
            _muxers << m;

            //get default codecs
            QString defltVideoCodec = settings.value("defaultVideoCodec").toString();
            m->setDefaultVideoCodec(videoEncoder( defltVideoCodec ));

            QString defltAudioCodec = settings.value("defaultAudioCodec").toString();
            m->setDefaultAudioCodec(audioEncoder( defltAudioCodec ));

            QString extensions = settings.value("extensions").toString();
            m->setExtensions(extensions.split(","));
        }

        settings.endArray();
    }

    //add image sequences
    QStringList extensions;

    emit newLog("Image sequences muxers...");

    FFMuxer *muxer = new FFMuxer("bmp","Bitmap Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("bmp"));
    muxer->setDefaultVideoCodec(videoEncoder("bmp"));
    _muxers << muxer;

    muxer = new FFMuxer("dpx","DPX Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("dpx"));
    muxer->setDefaultVideoCodec(videoEncoder("dpx"));
    _muxers << muxer;

    muxer = new FFMuxer("mjpeg","JPEG Sequence");
    muxer->setSequence(true);
    extensions << "jpg" << "jpeg";
    muxer->setExtensions(extensions);
    extensions.clear();
    muxer->setDefaultVideoCodec(videoEncoder("mjpeg"));
    _muxers << muxer;

    muxer = new FFMuxer("ljpeg","Lossless JPEG Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("ljpg"));
    muxer->setDefaultVideoCodec(videoEncoder("ljpeg"));
    _muxers << muxer;

    muxer = new FFMuxer("pam","PAM (Portable AnyMap) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("pam"));
    muxer->setDefaultVideoCodec(videoEncoder("pam"));
    _muxers << muxer;

    muxer = new FFMuxer("pbm","PBM (Portable BitMap) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("pbm"));
    muxer->setDefaultVideoCodec(videoEncoder("pbm"));
    _muxers << muxer;

    muxer = new FFMuxer("pcx","PC Paintbrush PCX Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("pcx"));
    muxer->setDefaultVideoCodec(videoEncoder("pcx"));
    _muxers << muxer;

    muxer = new FFMuxer("pgm","PGM (Portable GrayMap) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("pgm"));
    muxer->setDefaultVideoCodec(videoEncoder("pgm"));
    _muxers << muxer;

    muxer = new FFMuxer("pgmyuv","PGMYUV (Portable GrayMap YUV) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("pgmyuv"));
    muxer->setDefaultVideoCodec(videoEncoder("pgmyuv"));
    _muxers << muxer;

    muxer = new FFMuxer("png","PNG (Portable Network Graphics) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("png"));
    muxer->setDefaultVideoCodec(videoEncoder("png"));
    _muxers << muxer;

    muxer = new FFMuxer("ppm","PPM (Portable PixelMap) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("ppm"));
    muxer->setDefaultVideoCodec(videoEncoder("ppm"));
    _muxers << muxer;

    muxer = new FFMuxer("sgi","SGI Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("sgi"));
    muxer->setDefaultVideoCodec(videoEncoder("sgi"));
    _muxers << muxer;

    muxer = new FFMuxer("targa","TARGA (Truevision Targa) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("tga"));
    muxer->setDefaultVideoCodec(videoEncoder("targa"));
    _muxers << muxer;

    muxer = new FFMuxer("tiff","TIFF Sequence");
    muxer->setSequence(true);
    extensions << "tif" << "tiff";
    muxer->setExtensions(extensions);
    extensions.clear();
    muxer->setDefaultVideoCodec(videoEncoder("tiff"));
    _muxers << muxer;

    muxer = new FFMuxer("jpeg2000","JPEG 2000 Sequence");
    muxer->setSequence(true);
    extensions << "jp2" << "j2k";
    muxer->setExtensions(extensions);
    extensions.clear();
    muxer->setDefaultVideoCodec(videoEncoder("jpeg2000"));
    _muxers << muxer;

    muxer = new FFMuxer("xwd","XWD (X Window Dump) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("xwd"));
    muxer->setDefaultVideoCodec(videoEncoder("xwd"));
    _muxers << muxer;

    muxer = new FFMuxer("xbm","XBM (X BitMap) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("xbm"));
    muxer->setDefaultVideoCodec(videoEncoder("xbm"));
    _muxers << muxer;


    emit newLog("Sorting muxers...");
    std::sort(_muxers.begin(),_muxers.end(),muxerSorter);
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
        emit newError("FFmpeg executable binary not found.\nYou can download it at http://ffmpeg.org");
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

    //if the version is different, get from the output
    if (newVersion != _version)
    {
        emit newLog("Updating codec list.");

        QStringList codecs = output.split("\n");
        QRegularExpression re("([D.])([E.])([VAS])([I.])([L.])([S.]) (\\w+) +([^\\(\\n]+)");

        //remove the previous codecs
        settings.remove("ffmpeg/codecs");
        //create the new array
        settings.beginWriteArray("ffmpeg/codecs");

        for (int i = 0 ; i < codecs.count() ; i++)
        {
            QString codec = codecs[i];

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
                settings.setArrayIndex(i);
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
                    QStringList args("-h");
                    if (co->isEncoder()) args << "encoder=" + co->name();
                    else args << "decoder=" + co->name();
                    ffmpegProcess->setArguments(args);
                    ffmpegProcess->start(QIODevice::ReadOnly);
                    if (ffmpegProcess->waitForFinished(3000))
                    {
                        QStringList lines = _ffmpegOutput.split("\n");
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
                                    for( int j = 0; j < pixFmts.length(); j++ )
                                    {
                                        QString pixFmt = pixFmts[j].trimmed();

                                        FFPixFormat *pf = pixFormat(pixFmt);
                                        if (pf == nullptr) continue;
                                        co->addPixFormat(pf);
                                        bool deflt = pixFmt == defaultPF;
                                        if ( deflt ) co->setDefaultPixFormat(pf);

                                        settings.setArrayIndex(j);
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
    }
    else //other wise, get from settings
    {
        emit newLog("Getting codecs from stored list.");

        //open array
        int arraySize = settings.beginReadArray("ffmpeg/codecs");

        for (int i = 0; i < arraySize; i++)
        {
            settings.setArrayIndex(i);

            QString codecName = settings.value("codecName").toString();
            QString codecPrettyName = settings.value("codecPrettyName").toString();
            FFCodec *co = new FFCodec(codecName,codecPrettyName,this);

            emit newLog("Loading codec: " + codecName);

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
                    if (pf == nullptr) continue;

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

    //if the version is different, get from the output
    if (newVersion != _version)
    {
        QStringList pixFmts = output.split("\n");
        QRegularExpression re("([I.])([O.])([H.])([P.])([B.]) (\\w+) +(\\d+) +(\\d+)");

        //remove the previous formats
        settings.remove("ffmpeg/pixFmts");
        //create the new array
        settings.beginWriteArray("ffmpeg/pixFmts");

        for (int i = 0 ; i < pixFmts.count() ; i++)
        {
            QString pixFmt = pixFmts[i];

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
                settings.setArrayIndex(i);
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
    else //other wise, get from settings
    {
        //open array
        int arraySize = settings.beginReadArray("ffmpeg/pixFmts");

        for (int i = 0; i < arraySize; ++i)
        {
            settings.setArrayIndex(i);


            QString name = settings.value("name").toString();
            int numComponents = settings.value("numComponents").toInt();
            int bpp = settings.value("bpp").toInt();

            FFPixFormat *pf = new FFPixFormat(name, "", numComponents, bpp);

            emit newLog("Loading pixel format: " + name);

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

    emit newLog("Sorting pixel formats...");
    std::sort(_pixFormats.begin(),_pixFormats.end(),ffSorter);

}

void FFmpeg::readyRead(QString output)
{
    emit newLog( output, LogUtils::Debug );
    _ffmpegOutput += output;
}

