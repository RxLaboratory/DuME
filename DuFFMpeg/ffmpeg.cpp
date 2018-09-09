#include "ffmpeg.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif


FFmpeg::FFmpeg(QString path,QObject *parent) : FFObject(parent)
{
    _status = Waiting;
    _lastErrorMessage = "";
    _lastError = QProcess::UnknownError;
    _debugBaseMessage = "";

    _ffmpeg = new QProcess(this);

    _aerender = new QProcess(this);

    _aeInfo = new AERender(this);

    _currentFrame = 0;
    _startTime = QTime(0,0,0);
    _outputSize = 0.0;
    _outputBitrate = 0;
    _encodingSpeed = 0.0;

    _aerenderOutput = "";
    _ffmpegOutput = "";

    //Connect process
    connect(_ffmpeg,SIGNAL(readyReadStandardError()),this,SLOT(stdError()));
    connect(_ffmpeg,SIGNAL(readyReadStandardOutput()),this,SLOT(stdOutput()));
    connect(_ffmpeg,SIGNAL(started()),this,SLOT(started()));
    connect(_ffmpeg,SIGNAL(finished(int)),this,SLOT(finished()));
    connect(_ffmpeg,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(errorOccurred(QProcess::ProcessError)));

    connect(_aerender,SIGNAL(readyReadStandardError()),this,SLOT(stdErrorAE()));
    connect(_aerender,SIGNAL(readyReadStandardOutput()),this,SLOT(stdOutputAE()));
    connect(_aerender,SIGNAL(started()),this,SLOT(startedAE()));
    connect(_aerender,SIGNAL(finished(int)),this,SLOT(finishedAE()));
    connect(_aerender,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(errorOccurredAE(QProcess::ProcessError)));

    //TODO auto find ffmpeg if no settings or path invalid
    QSettings settings;

    if (path == "") setBinaryFileName(settings.value("ffmpeg/path","ffmpeg.exe").toString(), false);
    else setBinaryFileName(path, false);

}

bool FFmpeg::setBinaryFileName(QString path, bool initialize)
{
    if(QFile(path).exists())
    {
        _ffmpeg->setProgram(path);
        if (initialize) init();
        emit binaryChanged();
        return true;
    }
    else
    {
        setStatus(Error);
        emit newOutput("FFmpeg executable binary not found.\nYou can download it at http://ffmpeg.org");
        _lastErrorMessage = "FFmpeg executable binary not found.\nYou can download it at http://ffmpeg.org";
        debug("FFmpeg executable binary not found.\nYou can download it at http://ffmpeg.org");
        return false;
    }
}

bool FFmpeg::setAERenderFileName(QString path)
{
    if(QFile(path).exists())
    {
        _aerender->setProgram(path);
        return true;
    }
    else
    {
        setStatus(Error);
        emit newOutput("AERender executable binary not found.\nYou have to install Adobe After Effects to be able to render After Effects compositions.");
        _lastErrorMessage = "AERender executable binary not found.\nYou have to install Adobe After Effects to be able to render After Effects compositions.";
        return false;
    }
}

void FFmpeg::runCommand(QString commands)
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
    runCommand(commandList);
}

void FFmpeg::runCommand(QStringList commands)
{
    _ffmpeg->setArguments(commands);
    _ffmpeg->start();
}

void FFmpeg::init()
{   
#if INIT_FFMPEG

    //get pixFormats
    _debugBaseMessage = "FFmpeg initialization | Loading pixel Formats";
    debug();
    _ffmpeg->setArguments(QStringList("-pix_fmts"));
    _ffmpeg->start(QIODevice::ReadOnly);
    if (_ffmpeg->waitForFinished(10000))
    {
        gotPixFormats(_ffmpegOutput);
    }

    //get codecs
    _debugBaseMessage = "FFmpeg initialization | Loading codecs";
    debug();
    _ffmpeg->setArguments(QStringList("-codecs"));
    _ffmpeg->start(QIODevice::ReadOnly);
    if (_ffmpeg->waitForFinished(10000))
    {
        gotCodecs(_ffmpegOutput);
    }

    //get muxers
    _debugBaseMessage = "FFmpeg initialization | Loading muxers";
    debug();
    _ffmpeg->setArguments(QStringList("-formats"));
    _ffmpeg->start(QIODevice::ReadOnly);
    if (_ffmpeg->waitForFinished(10000))
    {
        gotMuxers(_ffmpegOutput);
    }

    //get long help
    _debugBaseMessage = "FFmpeg initialization | Loading documentation";
    debug();
    QStringList args("-h");
    args << "long";
    _ffmpeg->setArguments(args);
    _ffmpeg->start(QIODevice::ReadOnly);
    if (_ffmpeg->waitForFinished(3000))
    {
        _longHelp = _ffmpegOutput;
    }

    //get help
    _ffmpeg->setArguments(QStringList("-h"));
    _ffmpeg->start(QIODevice::ReadOnly);
    if (_ffmpeg->waitForFinished(3000))
    {
        _help = _ffmpegOutput;
    }

#endif

    //After Effects
    initAe();
}

void FFmpeg::initAe()
{
#if INIT_AE

    QSettings settings;
    //get AERender
    _debugBaseMessage = "";
    debug("Adobe After Effects Renderer initialization...");

    _aeInfo->init();

    //load After Effects
    QList<AERenderObject *> aeRenders = _aeInfo->versions();
    if (aeRenders.count() > 0)
    {
        if (settings.value("aerender/useLatest",true).toBool())
        {
            settings.setValue("aerender/path",QVariant(aeRenders.last()->path()));
        }
        else
        {
            QString preferredVersion = settings.value("aerender/version","Custom").toString();
            bool found = false;
            if (preferredVersion != "Custom")
            {
                for (int i = 0; i < aeRenders.count(); i++)
                {
                    if (aeRenders[i]->name() == preferredVersion)
                    {
                        settings.setValue("aerender/path",QVariant(aeRenders[i]->path()));
                        found = true;
                        break;
                    }
                }
                //if not found, use latest
                if (!found)
                {
                    settings.setValue("aerender/path",QVariant(aeRenders.last()->path()));
                }
            }
        }
        if (settings.value("aerender/path","") == "")
        {
            debug("Adobe After Effects Renderer not found. You may have to install Adobe After Effects to render After Effects compositions.");
        }
        else
        {
            debug("Adobe After Effects Renderer found at \n" + settings.value("aerender/path","").toString());
        }

        setAERenderFileName(settings.value("aerender/path","").toString());
    }

#endif
}

QList<FFMuxer *> FFmpeg::getMuxers()
{
    return _muxers;
}

FFMuxer *FFmpeg::getMuxer(QString name)
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

FFCodec *FFmpeg::getMuxerDefaultCodec(FFMuxer *muxer, FFCodec::Ability ability)
{
    FFCodec *videoCodec = muxer->defaultVideoCodec();
    FFCodec *audioCodec = muxer->defaultAudioCodec();

    //return
    if (ability == FFCodec::Video) return videoCodec;
    if (ability == FFCodec::Audio) return audioCodec;

    return nullptr;
}

FFCodec *FFmpeg::getMuxerDefaultCodec(QString name, FFCodec::Ability ability)
{
    return getMuxerDefaultCodec(getMuxer(name),ability);
}

QList<FFCodec *> FFmpeg::getEncoders()
{
    QList<FFCodec *> encoders = _videoEncoders;
    encoders.append(_audioEncoders);

    return encoders;
}

QList<FFCodec *> FFmpeg::getVideoEncoders()
{
    return _videoEncoders;
}

QList<FFCodec *> FFmpeg::getAudioEncoders()
{
    return _audioEncoders;
}

FFCodec *FFmpeg::getVideoEncoder(QString name)
{
    foreach(FFCodec *codec,_videoEncoders)
    {
        if (codec->name() == name) return codec;
    }
    return nullptr;
}

FFCodec *FFmpeg::getAudioEncoder(QString name)
{
    foreach(FFCodec *codec,_audioEncoders)
    {
        if (codec->name() == name) return codec;
    }
    return nullptr;
}

FFPixFormat *FFmpeg::getPixFormat(QString name)
{
    foreach(FFPixFormat *pf,_pixFormats)
    {
        if (pf->name() == name) return pf;
    }
    return nullptr;
}

QString FFmpeg::getHelp()
{
    return _help;
}

QString FFmpeg::getLongHelp()
{
    return _longHelp;
}

FFMediaInfo *FFmpeg::getMediaInfo(QString mediaPath)
{
    QString infoString = getMediaInfoString(mediaPath);
    if (infoString == "") return new FFMediaInfo("",this);
    FFMediaInfo *info = new FFMediaInfo(infoString,this);
    return info;
}

QString FFmpeg::getMediaInfoString(QString mediaPath)
{
    QStringList args("-i");
    args << QDir::toNativeSeparators(mediaPath);
    _ffmpeg->setArguments(args);
    _ffmpeg->start(QIODevice::ReadOnly);
    if (_ffmpeg->waitForFinished(3000))
    {
        return _ffmpegOutput;
    }
    return "";
}

int FFmpeg::getCurrentFrame()
{
    return _currentFrame;
}

QTime FFmpeg::getStartTime()
{
    return _startTime;
}

QTime FFmpeg::getElapsedTime()
{
    return QTime(0,0,0).addSecs(_startTime.elapsed() / 1000);
}

double FFmpeg::getOutputSize(FFMediaInfo::SizeUnit unit)
{
    double s = _outputSize;
    if (unit == FFMediaInfo::KB) s = s/1024;
    if (unit == FFMediaInfo::MB) s = s/1024/1024;
    return s;
}

double FFmpeg::getOutputBitrate(FFMediaInfo::BitrateUnit unit)
{
    double bitrate = _outputBitrate;
    if (unit == FFMediaInfo::Kbps) bitrate = bitrate/1024;
    if (unit == FFMediaInfo::Mbps) bitrate = bitrate/1024/1024;
    return bitrate;
}

double FFmpeg::getEncodingSpeed()
{
    return _encodingSpeed;
}

QTime FFmpeg::getTimeRemaining()
{
    return _timeRemaining;
}

FFQueueItem *FFmpeg::getCurrentItem()
{
    return _currentItem;
}

QProcess::ProcessError FFmpeg::getLastError()
{
    return _lastError;
}

QString FFmpeg::getLastErrorMessage()
{
    return _lastErrorMessage;
}

FFmpeg::Status FFmpeg::getStatus()
{
    return _status;
}

void FFmpeg::encode()
{
    if (_status == Encoding) return;
    setStatus(Encoding);

    //launch first item
    encodeNextItem();
}

void FFmpeg::encode(FFQueueItem *item)
{
    _encodingQueue << item;
    encode();
}

void FFmpeg::encode(QList<FFQueueItem*> list)
{
    _encodingQueue.append(list);
    encode();
}

void FFmpeg::encode(FFMediaInfo *input, QList<FFMediaInfo *> outputs)
{
    FFQueueItem *item = new FFQueueItem(input,outputs,this);
    _encodingQueue << item;
    encode();
}

void FFmpeg::encode(FFMediaInfo *input, FFMediaInfo *output)
{
    FFQueueItem *item = new FFQueueItem(input,output,this);
    _encodingQueue << item;
    encode();
}

int FFmpeg::addQueueItem(FFQueueItem *item)
{
    _encodingQueue.append(item);
    return _encodingQueue.count()-1;
}

void FFmpeg::removeQueueItem(int id)
{
    FFQueueItem *i = _encodingQueue.takeAt(id);
    delete i;
}

FFQueueItem *FFmpeg::takeQueueItem(int id)
{
    return _encodingQueue.takeAt(id);
}

void FFmpeg::clearQueue()
{
    while(_encodingQueue.count() > 0)
    {
        removeQueueItem(0);
    }
}

void FFmpeg::stop(int timeout)
{
    if (_ffmpeg->state() == QProcess::NotRunning) return;
    _ffmpeg->write("q\n");
    if (!_ffmpeg->waitForFinished(timeout))
    {
        _ffmpeg->kill();
    }
    setStatus(Waiting);
}

void FFmpeg::stdError()
{
    QString output = _ffmpeg->readAllStandardError();
    readyRead(output);
}

void FFmpeg::stdOutput()
{
    QString output = _ffmpeg->readAllStandardOutput();
    readyRead(output);
}

void FFmpeg::stdErrorAE()
{
    QString output = _aerender->readAllStandardError();
    readyReadAE(output);
}

void FFmpeg::stdOutputAE()
{
    QString output = _aerender->readAllStandardOutput();
    readyReadAE(output);
}

void FFmpeg::started()
{
    _ffmpegOutput = "";
}

void FFmpeg::finished()
{
    if (_status == Encoding)
    {
        _currentItem->setStatus(FFQueueItem::Finished);
        emit encodingFinished(_currentItem);
        //move to history
        _encodingHistory << _currentItem;
        encodeNextItem();
    }
    else
    {
        setStatus(Waiting);
    }
}

void FFmpeg::startedAE()
{
    _aerenderOutput = "";
}

void FFmpeg::finishedAE()
{
    if (_status == AERendering)
    {
        //TODO update currentitem with rendered frames, and set it to render
        //re-launch item

       FFMediaInfo *input = _currentItem->getInputMedias()[0];
       QTemporaryDir *aeTempDir = input->aepTempDir();
       input->setAepTempDir(nullptr);
       delete aeTempDir;

       debug("After Effects Render process successfully finished");
    }
    else
    {
        setStatus(Waiting);
    }
}

void FFmpeg::errorOccurred(QProcess::ProcessError e)
{
    QString error;
    if (e == QProcess::FailedToStart)
    {
        error = "Failed to start FFMpeg.";
    }
    else if (e == QProcess::Crashed)
    {
        error = "FFmpeg just crashed.";
    }
    else if (e == QProcess::Timedout)
    {
        error = "FFmpeg operation timed out.";
    }
    else if (e == QProcess::WriteError)
    {
        error = "FFmpeg write Error.";
    }
    else if (e == QProcess::ReadError)
    {
        error = "Cannot read FFMpeg output.";
    }
    else if (e == QProcess::UnknownError)
    {
        error = "An unknown FFmpeg error occured.";
    }

    if (_status == Encoding)
    {
        _currentItem->setStatus(FFQueueItem::Stopped);
        _encodingHistory << _currentItem;
    }

    setStatus(Error);
    _lastError = e;
    emit processError(error);
}

void FFmpeg::errorOccurredAE(QProcess::ProcessError e)
{
    QString error;
    if (e == QProcess::FailedToStart)
    {
        error = "Failed to start AERender.";
    }
    else if (e == QProcess::Crashed)
    {
        error = "AERender just crashed.";
    }
    else if (e == QProcess::Timedout)
    {
        error = "AERender operation timed out.";
    }
    else if (e == QProcess::WriteError)
    {
        error = "AERender write Error.";
    }
    else if (e == QProcess::ReadError)
    {
        error = "Cannot read AERender output.";
    }
    else if (e == QProcess::UnknownError)
    {
        error = "An unknown AERender error occured.";
    }

    if (_status == Encoding)
    {
        _currentItem->setStatus(FFQueueItem::Stopped);
        _encodingHistory << _currentItem;
    }

    setStatus(Error);
    _lastError = e;
    emit processError(error);
}

void FFmpeg::encodeNextItem()
{
    QSettings settings;

    if (_encodingQueue.count() == 0)
    {
        setStatus(Waiting);
        return;
    }

    _currentItem = _encodingQueue.takeAt(0);

    //Check if there are AEP to render
    foreach(FFMediaInfo *input,_currentItem->getInputMedias())
    {
        if (input->isAep())
        {
            QTemporaryDir *aeTempDir = new QTemporaryDir(settings.value("aerender/cache","").toString());
            input->setAepTempDir(aeTempDir);

            QStringList arguments("-project");
            arguments <<  QDir::toNativeSeparators(input->fileName());
            if (input->aepCompName() != "")
            {
                arguments << "-comp" << input->aepCompName();
            }
            else if (input->aepRqindex() > 0)
            {
                arguments << "-rqindex" << QString::number(input->aepRqindex());
            }
            else
            {
                arguments << "-rqindex" << "1";
            }

            arguments << "-OMtemplate" << "DuFFmpeg";
            arguments << "-RStemplate" << "DuFFmpeg";

            QString tempPath = QDir::toNativeSeparators(aeTempDir->path()) + "\\" + "Duffmpeg_[#####]";

            arguments << "-output" << tempPath;

            debug("Beginning After Effects rendering\nUsing aerender commands:\n" + arguments.join(" | "));
            debug("After Effects outputs to " + tempPath);

            //launch
            _aerender->setArguments(arguments);
            _aerender->start(QIODevice::ReadWrite);

            _currentItem->setStatus(FFQueueItem::InProgress);
            _startTime = QTime::currentTime();

            setStatus(AERendering);
            emit encodingStarted(_currentItem);

            return;
        }
    }

    //if all aep are rendered, launch ffmpeg process

    //generate arguments
    QStringList arguments("-stats");
    arguments << "-y";

    //add inputs
    foreach(FFMediaInfo *input,_currentItem->getInputMedias())
    {
        QString inputFileName = input->fileName();
        //add custom options
        foreach(QStringList option,input->ffmpegOptions())
        {
            arguments << option[0];
            if (option.count() > 1)
            {
                if (option[1] != "") arguments << option[1];
            }
        }
        //add sequence options
        if (input->isImageSequence())
        {
            arguments << "-framerate" << QString::number(input->videoFramerate());
            arguments << "-start_number" << QString::number(input->startNumber());
            inputFileName = convertSequenceName(inputFileName);
        }
        //add trc
        if (input->trc() != "")
        {
            arguments << "-apply_trc" << input->trc();
        }
        //add input file
        arguments << "-i" << QDir::toNativeSeparators(inputFileName);
    }
    //add outputs
    foreach(FFMediaInfo *output,_currentItem->getOutputMedias())
    {
        //muxer
        QString muxer = "";
        if (output->muxer() != nullptr)
        {
            muxer = output->muxer()->name();
            if (output->muxer()->isSequence()) muxer = "image2";
        }
        if (muxer != "")
        {
            arguments << "-f" << muxer;
        }

        //add custom options
        foreach(QStringList option,output->ffmpegOptions())
        {
            arguments << option[0];
            if (option.count() > 1)
            {
                if (option[1] != "") arguments << option[1];
            }
        }

        //video
        QString codec = "";
        if (output->videoCodec() != nullptr) codec = output->videoCodec()->name();


        if (output->hasVideo())
        {
            //codec
            if (codec != "") arguments << "-vcodec" << codec;

            if (codec != "copy")
            {
                //bitrate
                int bitrate = output->videoBitrate();
                if (bitrate != 0)
                {
                    arguments << "-b:v" << QString::number(bitrate);

                }

                //size
                int width = output->videoWidth();
                int height = output->videoHeight();
                if (width != 0 && height != 0)
                {
                    arguments << "-s" << QString::number(width) + "x" + QString::number(height);
                }

                //framerate
                double framerate = output->videoFramerate();
                if (framerate != 0.0)
                {
                    arguments << "-r" << QString::number(framerate);
                }

                //loop (gif)
                if (codec == "gif")
                {
                    int loop = output->loop();
                    arguments << "-loop" << QString::number(loop);
                }

                //profile
                int profile = output->videoProfile();
                if (profile > -1)
                {
                    arguments << "-profile" << QString::number(profile);
                }

                //quality (h264)
                int quality = output->videoQuality();
                if (codec == "h264" && quality > 0 )
                {
                    quality = 100-quality;
                    //adjust to CRF values
                    if (quality < 10)
                    {
                        //convert to range 0-15 // visually lossless
                        quality = quality*15/10;
                    }
                    else if (quality < 25)
                    {
                        //convert to range 15-21 // very good
                        quality = quality-10;
                        quality = quality*6/15;
                        quality = quality+15;
                    }
                    else if (quality < 50)
                    {
                        //convert to range 22-28 // good
                        quality = quality-25;
                        quality = quality*6/25;
                        quality = quality+21;
                    }
                    else if (quality < 75)
                    {
                        //convert to range 29-34 // bad
                        quality = quality-50;
                        quality = quality*6/25;
                        quality = quality+28;
                    }
                    else
                    {
                        //convert to range 35-51 // very bad
                        quality = quality-75;
                        quality = quality*17/25;
                        quality = quality+34;
                    }
                    arguments << "-crf" << QString::number(quality);
                }

                //start number (sequences)
                if (muxer == "image2")
                {
                    int startNumber = output->startNumber();
                    arguments << "-start_number" << QString::number(startNumber);
                }

                //pixel format
                QString pixFmt = "";
                if (output->pixFormat() != nullptr) pixFmt = output->pixFormat()->name();
                //set default for h264 to yuv420 (ffmpeg generates 444 by default which is not standard)
                if (pixFmt == "" && codec == "h264") pixFmt = "yuv420p";
                if (pixFmt != "") arguments << "-pix_fmt" << pixFmt;

                //b-pyramids
                //set as none to h264: not really useful (only on very static footage), but has compatibility issues
                if (codec == "h264") arguments << "-x264opts" << "b_pyramid=0";

                //unpremultiply
                bool unpremultiply = !output->premultipliedAlpha();
                if (unpremultiply) arguments << "-vf" << "unpremultiply=inplace=1";
            }
        }
        else
        {
            //no video
            arguments << "-vn";
        }

        //audio
        QString acodec = "";
        if (output->audioCodec() != nullptr) acodec = output->audioCodec()->name();

        if (output->hasAudio())
        {
            //codec
            if (acodec != "") arguments << "-acodec" << acodec;

            if (acodec != "copy")
            {
                //bitrate
                int bitrate = output->audioBitrate();
                if (bitrate != 0)
                {
                    arguments << "-b:a" << QString::number(output->audioBitrate());
                }

                //sampling
                int sampling = output->audioSamplingRate();
                if (sampling != 0)
                {
                    arguments << "-ar" << QString::number(sampling);
                }
            }
        }
        else
        {
            //no audio
            arguments << "-an";
        }

        //file
        QString outputPath = QDir::toNativeSeparators(output->fileName());

        //if sequence, digits
        if (output->muxer() != nullptr)
        {
            if (output->muxer()->isSequence())
            {
                outputPath = convertSequenceName(outputPath);
            }
        }

        arguments << outputPath;
    }

    debug("Beginning new encoding\nUsing FFmpeg commands:\n" + arguments.join(" | "));

    //launch
    _ffmpeg->setArguments(arguments);
    _ffmpeg->start(QIODevice::ReadWrite);

    _currentItem->setStatus(FFQueueItem::InProgress);
    _startTime = QTime::currentTime();
    emit  encodingStarted(_currentItem);
}

void FFmpeg::setStatus(Status st)
{
    _status = st;
    emit statusChanged(_status);
}

AERender *FFmpeg::getAeRender() const
{
    return _aeInfo;
}

QList<FFPixFormat *> FFmpeg::getPixFormats()
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

void FFmpeg::gotMuxers(QString output)
{
    //delete all
    qDeleteAll(_muxers);
    _muxers.clear();

    //get Muxers
    QStringList muxers = output.split("\n");
    QRegularExpression re("[D. ]E (\\w+)\\s+(.+)");

    foreach(QString muxer,muxers)
    {
        QRegularExpressionMatch match = re.match(muxer);
        if (match.hasMatch())
        {
            QString name = match.captured(1).trimmed();
            QString prettyName = match.captured(2).trimmed();

            debug(" | " + name);

            // skip image sequence
            if (name == "image2") continue;
            FFMuxer *m = new FFMuxer(name,prettyName,this);
            _muxers << m;
            //get default codecs
            QStringList args("-h");
            args << "muxer=" + m->name();
            _ffmpeg->setArguments(args);
            _ffmpeg->start(QIODevice::ReadOnly);
            if (_ffmpeg->waitForFinished(3000))
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
                        m->setDefaultVideoCodec(getVideoEncoder(videoMatch.captured(1)));
                    }

                    //audio codec
                    QRegularExpressionMatch audioMatch = reAudio.match(line);
                    if (audioMatch.hasMatch())
                    {
                        m->setDefaultAudioCodec(getAudioEncoder(audioMatch.captured(1)));
                    }

                    //extensions
                    QRegularExpressionMatch extensionsMatch = reExtensions.match(line);
                    if (extensionsMatch.hasMatch())
                    {
                        m->setExtensions(extensionsMatch.captured(1).split(","));
                    }
                }
            }
        }
    }

    //add image sequences
    QStringList extensions;

    debug(" | Image sequences...");

    FFMuxer *muxer = new FFMuxer("bmp","Bitmap Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("bmp"));
    muxer->setDefaultVideoCodec(getVideoEncoder("bmp"));
    _muxers << muxer;

    muxer = new FFMuxer("dpx","DPX Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("dpx"));
    muxer->setDefaultVideoCodec(getVideoEncoder("dpx"));
    _muxers << muxer;

    muxer = new FFMuxer("mjpeg","JPEG Sequence");
    muxer->setSequence(true);
    extensions << "jpg" << "jpeg";
    muxer->setExtensions(extensions);
    extensions.clear();
    muxer->setDefaultVideoCodec(getVideoEncoder("mjpeg"));
    _muxers << muxer;

    muxer = new FFMuxer("ljpeg","Lossless JPEG Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("ljpg"));
    muxer->setDefaultVideoCodec(getVideoEncoder("ljpeg"));
    _muxers << muxer;

    muxer = new FFMuxer("pam","PAM (Portable AnyMap) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("pam"));
    muxer->setDefaultVideoCodec(getVideoEncoder("pam"));
    _muxers << muxer;

    muxer = new FFMuxer("pbm","PBM (Portable BitMap) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("pbm"));
    muxer->setDefaultVideoCodec(getVideoEncoder("pbm"));
    _muxers << muxer;

    muxer = new FFMuxer("pcx","PC Paintbrush PCX Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("pcx"));
    muxer->setDefaultVideoCodec(getVideoEncoder("pcx"));
    _muxers << muxer;

    muxer = new FFMuxer("pgm","PGM (Portable GrayMap) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("pgm"));
    muxer->setDefaultVideoCodec(getVideoEncoder("pgm"));
    _muxers << muxer;

    muxer = new FFMuxer("pgmyuv","PGMYUV (Portable GrayMap YUV) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("pgmyuv"));
    muxer->setDefaultVideoCodec(getVideoEncoder("pgmyuv"));
    _muxers << muxer;

    muxer = new FFMuxer("png","PNG (Portable Network Graphics) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("png"));
    muxer->setDefaultVideoCodec(getVideoEncoder("png"));
    _muxers << muxer;

    muxer = new FFMuxer("ppm","PPM (Portable PixelMap) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("ppm"));
    muxer->setDefaultVideoCodec(getVideoEncoder("ppm"));
    _muxers << muxer;

    muxer = new FFMuxer("sgi","SGI Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("sgi"));
    muxer->setDefaultVideoCodec(getVideoEncoder("sgi"));
    _muxers << muxer;

    muxer = new FFMuxer("targa","TARGA (Truevision Targa) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("tga"));
    muxer->setDefaultVideoCodec(getVideoEncoder("targa"));
    _muxers << muxer;

    muxer = new FFMuxer("tiff","TIFF Sequence");
    muxer->setSequence(true);
    extensions << "tif" << "tiff";
    muxer->setExtensions(extensions);
    extensions.clear();
    muxer->setDefaultVideoCodec(getVideoEncoder("tiff"));
    _muxers << muxer;

    muxer = new FFMuxer("jpeg2000","JPEG 2000 Sequence");
    muxer->setSequence(true);
    extensions << "jp2" << "j2k";
    muxer->setExtensions(extensions);
    extensions.clear();
    muxer->setDefaultVideoCodec(getVideoEncoder("jpeg2000"));
    _muxers << muxer;

    muxer = new FFMuxer("xwd","XWD (X Window Dump) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("xwd"));
    muxer->setDefaultVideoCodec(getVideoEncoder("xwd"));
    _muxers << muxer;

    muxer = new FFMuxer("xbm","XBM (X BitMap) Sequence");
    muxer->setSequence(true);
    muxer->setExtensions(QStringList("xbm"));
    muxer->setDefaultVideoCodec(getVideoEncoder("xbm"));
    _muxers << muxer;


    debug(" | Sorting...");
    std::sort(_muxers.begin(),_muxers.end(),muxerSorter);
}

bool ffSorter(FFBaseObject *c1,FFBaseObject *c2)
{
    return c1->prettyName().toLower() < c2->prettyName().toLower();
}

void FFmpeg::gotCodecs(QString output)
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
    debug(" | video copy");
    FFCodec *copyVideo = new FFCodec("copy","Copy video stream",FFCodec::Video | FFCodec::Encoder | FFCodec::Lossless | FFCodec::Lossy | FFCodec::IFrame,this);
    _videoEncoders << copyVideo;
    debug(" | audio copy");
    FFCodec *copyAudio = new FFCodec("copy","Copy audio stream",FFCodec::Audio | FFCodec::Encoder | FFCodec::Lossless | FFCodec::Lossy | FFCodec::IFrame,this);
    _audioEncoders << copyAudio;

    //get codecs
    QStringList codecs = output.split("\n");
    QRegularExpression re("([D.])([E.])([VAS])([I.])([L.])([S.]) (\\w+) +([^\\(\\n]+)");
    for (int i = 0 ; i < codecs.count() ; i++)
    {
        QString codec = codecs[i];

        QRegularExpressionMatch match = re.match(codec);
        if (match.hasMatch())
        {
            QString codecName = match.captured(7);
            QString codecPrettyName = match.captured(8);
            FFCodec *co = new FFCodec(codecName,codecPrettyName,this);

            debug(" | " + codecName);

            co->setDecoder(match.captured(1) == "D");
            co->setEncoder(match.captured(2) == "E");
            co->setVideo(match.captured(3) == "V");
            co->setAudio(match.captured(3) == "A");
            co->setIframe(match.captured(4) == "I");
            co->setLossy(match.captured(5) == "L");
            co->setLossless(match.captured(6) == "S");

            if (co->isVideo())
            {
                //get pixel formats
                QStringList args("-h");
                if (co->isEncoder()) args << "encoder=" + co->name();
                else args << "decoder=" + co->name();
                _ffmpeg->setArguments(args);
                _ffmpeg->start(QIODevice::ReadOnly);
                if (_ffmpeg->waitForFinished(3000))
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
                                QStringList pixFmts = pixFmtMatch.captured(1).split(" ");
                                QString defaultPF = pixFmts[0];
                                pixFmts.sort();
                                foreach(QString pixFmt, pixFmts)
                                {
                                    pixFmt = pixFmt.trimmed();
                                    FFPixFormat *pf = getPixFormat(pixFmt);
                                    if (pf == nullptr) continue;
                                    co->addPixFormat(pf);
                                    if (pixFmt == defaultPF) co->setDefaultPixFormat(pf);
                                }
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

    debug(" | Sorting...");
    std::sort(_videoEncoders.begin(),_videoEncoders.end(),ffSorter);
    std::sort(_audioEncoders.begin(),_audioEncoders.end(),ffSorter);
}

void FFmpeg::gotPixFormats(QString output)
{
    //delete all
    qDeleteAll(_pixFormats);
    _pixFormats.clear();

    //get pixfmts
    QStringList pixFmts = output.split("\n");
    QRegularExpression re("([I.])([O.])([H.])([P.])([B.]) (\\w+) +(\\d+) +(\\d+)");
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

            debug(" | " + name);

            pf->setInput(match.captured(1) == "I");
            pf->setOutput(match.captured(2) == "O");
            pf->setHardware(match.captured(3) == "H");
            pf->setPaletted(match.captured(4) == "P");
            pf->setBitstream(match.captured(5) == "B");

            _pixFormats << pf;
        }
    }

    debug(" | Sorting...");
    std::sort(_pixFormats.begin(),_pixFormats.end(),ffSorter);

}

void FFmpeg::readyRead(QString output)
{
    emit newOutput("FFmpeg output: " + output);

    _ffmpegOutput = _ffmpegOutput + output;

    QRegularExpression reProgress("(?:frame= *(\\d+).*fps= *(\\d+).*)?size= *(?:(\\d+)kB)?.*time=(\\d\\d:\\d\\d:\\d\\d.\\d\\d).*bitrate= *(?:(\\d+).\\d+kbits)?.*speed= *(\\d+.\\d*)x");
    QRegularExpressionMatch match = reProgress.match(output);
    //if progress, update UI
    if (match.hasMatch())
    {
        QString frame = match.captured(1);
        QString size = match.captured(3);
        QString bitrate = match.captured(5);
        QString speed = match.captured(6);

        //frame
        _currentFrame = frame.toInt();

        //size
        int sizeKB = size.toInt();
        _outputSize = sizeKB*1024;

        //bitrate
        int bitrateKB = bitrate.toInt();
        _outputBitrate = bitrateKB*1024;

        //speed
        _encodingSpeed = speed.toDouble();

        //time remaining
        //get current input duration
        //gets the current item duration
        double duration = 0;
        foreach(FFMediaInfo *input,_currentItem->getInputMedias())
        {
            if (input->hasVideo())
            {
                duration = input->duration() * input->videoFramerate();
                break;
            }
        }
        if (duration > 0)
        {
            if (_currentFrame > 0)
            {
                int elapsed = _startTime.elapsed() / 1000;
                double remaining = elapsed*duration/_currentFrame - elapsed;
                _timeRemaining = QTime(0,0,0).addMSecs(remaining*1000);
            }
        }
        emit progress();
    }

}

void FFmpeg::readyReadAE(QString output)
{
    emit newOutput("AERender output: " + output);

    debug("AERender output: " + output);

    _aerenderOutput = _aerenderOutput + output;

    //parse

    //get current input
    FFMediaInfo *input = _currentItem->getInputMedias().at(0);

    //Duration
    QRegularExpression reDuration("PROGRESS:  Duration: (\\d):(\\d\\d):(\\d\\d):(\\d\\d)");
    QRegularExpressionMatch match = reDuration.match(output);
    if (match.hasMatch())
    {
        int h = match.captured(1).toInt();
        int m = match.captured(2).toInt();
        int s = match.captured(3).toInt();
        int i = match.captured(4).toInt();

        double duration = h*60*60 + m*60 + s + i/input->videoFramerate();

        input->setDurationH(h);
        input->setDurationM(m);
        input->setDurationS(s);
        input->setDurationI(i);
        input->setDuration(duration);
    }

    QRegularExpression reFrameRate("PROGRESS:  Frame Rate: (\\d+,\\d\\d)");
    match = reFrameRate.match(output);
    if (match.hasMatch())
    {
        double fr = match.captured(1).toDouble();

        input->setVideoFramerate(fr);

        int h = input->durationH();
        int m = input->durationM();
        double s = input->durationS();
        int i = input->durationI();

        double duration = h*60*60 + m*60 + s + i/input->videoFramerate();
        input->setDuration(duration);
    }

    QRegularExpression reProgress("PROGRESS:  \\d:\\d\\d:\\d\\d:\\d\\d \\((\\d+)\\)");
    match = reProgress.match(output);
    if (match.hasMatch())
    {
        _currentFrame = match.captured(1).toInt();
        //time remaining
        //get current input duration
        //gets the current item duration
        double duration = input->duration() * input->videoFramerate();
        if (duration > 0)
        {
            if (_currentFrame > 0)
            {
                int elapsed = _startTime.elapsed() / 1000;
                double remaining = elapsed*duration/_currentFrame - elapsed;
                _timeRemaining = QTime(0,0,0).addMSecs(remaining*1000);
            }
        }
    }

    //TODO get size and bitrate


    emit progress();

}

QString FFmpeg::convertSequenceName(QString name)
{
    //detects all {###}
    QRegularExpression regExDigits("{(#+)}");
    QRegularExpressionMatchIterator regExDigitsMatch = regExDigits.globalMatch(name);
    while (regExDigitsMatch.hasNext())
    {
         QRegularExpressionMatch match = regExDigitsMatch.next();
         //count the ###
         QString digits = match.captured(1);
         int numDigits = digits.count();
         //replace
         name.replace(match.capturedStart(),match.capturedLength(),"%" + QString::number(numDigits) + "d");
    }
    return name;
}

void FFmpeg::debug(QString message)
{
    message = _debugBaseMessage + message;
#ifdef QT_DEBUG
qDebug() << message;
#endif
emit debugInfo(message);
}

FFMediaInfo *FFmpeg::loadJson(QString json)
{
    FFMediaInfo *mediaInfo = nullptr;

    debug("Loading preset");

    QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());

    //validate file
    if (!jsonDoc.isObject())
    {
        debug("Invalid preset file");
        return mediaInfo;
    }
    QJsonObject mainObj = jsonDoc.object();
    if (mainObj.value("duffmpeg") == QJsonValue::Undefined)
    {
        debug("Invalid preset file - Cannot find Duffmpeg object");
        return mediaInfo;
    }

    //load and create mediaInfo
    QJsonObject mediaObj = mainObj.value("duffmpeg").toObject();
    QString version = mediaObj.value("version").toString();
    //TODO Check version
    debug("Preset version: " + version);


    debug("Getting media info...");
    mediaInfo = new FFMediaInfo();

    //muxer
    QJsonObject muxerObj = mediaObj.value("muxer").toObject();
    QString muxerName = muxerObj.value("name").toString();
    debug("Muxer: " + muxerName);
    mediaInfo->setMuxer(getMuxer(muxerName));
    mediaInfo->setLoop(mediaObj.value("loop").toInt());

    //video
    if (mediaObj.value("hasVideo").toBool())
    {
        mediaInfo->setVideo(true);
        QJsonObject videoObj = mediaObj.value("video").toObject();
        QString codecName = videoObj.value("codecName").toString();
        debug("Video codec: " + codecName);
        if (codecName != "default")
        {
            mediaInfo->setVideoCodec(getVideoEncoder(codecName));
        }
        mediaInfo->setVideoWidth(videoObj.value("width").toInt());
        mediaInfo->setVideoHeight(videoObj.value("height").toInt());
        mediaInfo->setVideoFramerate(videoObj.value("framerate").toDouble());
        mediaInfo->setVideoBitrate(videoObj.value("bitrate").toInt());
        mediaInfo->setVideoProfile(videoObj.value("profile").toInt());
        mediaInfo->setVideoQuality(videoObj.value("quality").toInt());
        mediaInfo->setStartNumber(videoObj.value("startNumber").toInt());
        if (!videoObj.value("premultipliedAlpha").isUndefined()) mediaInfo->setPremultipliedAlpha(videoObj.value("premultipliedAlpha").toBool());
        else mediaInfo->setPremultipliedAlpha(true);
        mediaInfo->setPixFormat(getPixFormat(videoObj.value("pixelFormat").toString()));
    }

    //audio
    if (mediaObj.value("hasAudio").toBool())
    {
        mediaInfo->setAudio(true);
        QJsonObject audioObj = mediaObj.value("audio").toObject();
        QString codecName = audioObj.value("codecName").toString();
        debug("Audio codec: " + codecName);
        if (codecName != "default")
        {
            mediaInfo->setAudioCodec(getAudioEncoder(codecName));
        }
        mediaInfo->setAudioSamplingRate(audioObj.value("sampling").toInt());
        mediaInfo->setAudioBitrate(audioObj.value("bitrate").toInt());
    }

    //options
    QJsonArray options = mediaObj.value("options").toArray();
    debug("Custom options");
    foreach(QJsonValue option,options)
    {
        QJsonObject optionObj = option.toObject();
        QStringList opt(optionObj.value("name").toString());
        opt << optionObj.value("value").toString();
        mediaInfo->addFFmpegOption(opt);
    }

    debug("FFmpeg preset loaded");

    return mediaInfo;
}

FFMediaInfo *FFmpeg::loadJsonFromFile(QString jsonFileName)
{
    FFMediaInfo *mediaInfo = nullptr;
    QFile jsonFile(jsonFileName);
    debug("Opening preset file: " + jsonFileName);
    if (jsonFile.open(QIODevice::ReadOnly))
    {
        debug("File opened");
        mediaInfo = loadJson(jsonFile.readAll());
        jsonFile.close();
    }
    return mediaInfo;
}
