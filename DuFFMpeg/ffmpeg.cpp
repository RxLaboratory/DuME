#include "ffmpeg.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

FFmpeg::FFmpeg(QString path,QObject *parent) : QObject(parent)
{
    _status = Waiting;

    _lastErrorMessage = "";
    _lastError = QProcess::UnknownError;

    _ffmpeg = new QProcess(this);
    setBinaryFileName(path);

    _currentFrame = 0;
    _startTime = QTime(0,0,0);
    _outputSize = 0.0;
    _outputBitrate = 0;
    _encodingSpeed = 0.0;

    //Connect process
    connect(_ffmpeg,SIGNAL(readyReadStandardError()),this,SLOT(stdError()));
    connect(_ffmpeg,SIGNAL(readyReadStandardOutput()),this,SLOT(stdOutput()));
    connect(_ffmpeg,SIGNAL(started()),this,SLOT(started()));
    connect(_ffmpeg,SIGNAL(finished(int)),this,SLOT(finished()));
    connect(_ffmpeg,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(errorOccurred(QProcess::ProcessError)));

#ifdef QT_DEBUG
    qDebug() << "FFmpeg - Initialization";
#endif
    init();
}

bool FFmpeg::setBinaryFileName(QString path)
{
    if(QFile(path).exists())
    {
        _ffmpeg->setProgram(path);
        init();
        emit binaryChanged();
        return true;
    }
    else
    {
        _status = Error;
        emit newOutput("FFmpeg executable binary not found.\nYou can download it at http://ffmpeg.org");
        _lastErrorMessage = "FFmpeg executable binary not found.\nYou can download it at http://ffmpeg.org";
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
    //get codecs
    _ffmpeg->setArguments(QStringList("-codecs"));
    _ffmpeg->start(QIODevice::ReadOnly);
    if (_ffmpeg->waitForFinished(10000))
    {
        gotCodecs(_ffmpegOutput);
    }

    //get muxers
    _ffmpeg->setArguments(QStringList("-formats"));
    _ffmpeg->start(QIODevice::ReadOnly);
    if (_ffmpeg->waitForFinished(10000))
    {
        gotMuxers(_ffmpegOutput);
    }
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

QString FFmpeg::getHelp()
{
    if (_help != "") return _help;

    //if first run, get the help
    //TODO use signals instead of waiting? it's very quick maybe not needed
    _ffmpeg->setArguments(QStringList("-h"));
    _ffmpeg->start(QIODevice::ReadOnly);
    if (_ffmpeg->waitForFinished(3000))
    {
        _help = _ffmpegOutput;
    }
    return _help;
}

QString FFmpeg::getLongHelp()
{
    if (_longHelp != "") return _longHelp;

    //if first run, get the long help
    //TODO use signals instead of waiting? it's very quick maybe not needed
    QStringList args("-h");
    args << "long";
    _ffmpeg->setArguments(args);
    _ffmpeg->start(QIODevice::ReadOnly);
    if (_ffmpeg->waitForFinished(3000))
    {
        _longHelp = _ffmpegOutput;
    }
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
        error = "Operation timed out.";
    }
    else if (e == QProcess::WriteError)
    {
        error = "Write Error.";
    }
    else if (e == QProcess::ReadError)
    {
        error = "Cannot read FFMpeg output.";
    }
    else if (e == QProcess::UnknownError)
    {
        error = "An unknown error occured.";
    }

    if (_status == Encoding)
    {
        _currentItem->setStatus(FFQueueItem::Stopped);
        _encodingHistory << _currentItem;
    }

    setStatus(Error);
    _lastErrorMessage = error;
    _lastError = e;
    emit processError(error);
}

void FFmpeg::encodeNextItem()
{
    if (_encodingQueue.count() == 0)
    {
        setStatus(Waiting);
        return;
    }

    _currentItem = _encodingQueue.takeAt(0);

    //generate arguments
    QStringList arguments("-stats");
    arguments << "-y";

    //add inputs
    foreach(FFMediaInfo *input,_currentItem->getInputMedias())
    {
        //add custom options
        arguments.append(input->ffmpegOptions());
        //add input file
        arguments << "-i" << QDir::toNativeSeparators(input->fileName());
    }
    //add outputs
    foreach(FFMediaInfo *output,_currentItem->getOutputMedias())
    {
        //add custom options
        arguments.append(output->ffmpegOptions());

        //video
        QString codec = "";
        if (output->videoCodec() != nullptr) codec = output->videoCodec()->name();


        if (output->hasVideo() && codec != "")
        {
            //codec
            arguments << "-vcodec" << codec;

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

        if (output->hasAudio() && acodec != "")
        {
            //codec
            arguments << "-acodec" << acodec;

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
        arguments << QDir::toNativeSeparators(output->fileName());
    }

    emit debugInfo("Beginning new encoding\nUsing FFmpeg commands:\n" + arguments.join(" | "));

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
            QString name = match.captured(1);
            QString prettyName = match.captured(2);
            // skip image sequence
            // TODO complete the sequences custom muxers built just after
            if (name == "image2") continue;
            FFMuxer *m = new FFMuxer(name,prettyName,this);
            _muxers << m;
            //get default codecs
            QStringList args("-h");
            args << "muxer=" + m->name();
            _ffmpeg->setArguments(args);
            _ffmpeg->start(QIODevice::ReadOnly);
            if (_ffmpeg->waitForFinished(10000))
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
    FFMuxer *muxer = new FFMuxer("image2","Bitmap Sequence");
    muxer->setType(FFMuxer::Sequence);
    muxer->setExtensions(QStringList("bmp"));
    _muxers << muxer;
    muxer = new FFMuxer("image2","DPX Sequence");
    muxer->setType(FFMuxer::Sequence);
    muxer->setExtensions(QStringList("dpx"));
    _muxers << muxer;
    muxer = new FFMuxer("image2","JPEG 2000 Sequence");
    muxer->setType(FFMuxer::Sequence);
    muxer->setExtensions(QStringList("jpeg2000"));
    _muxers << muxer;
    muxer = new FFMuxer("image2","JPEG Sequence");
    muxer->setType(FFMuxer::Sequence);
    muxer->setExtensions(QStringList("jpg"));
    _muxers << muxer;
    muxer = new FFMuxer("image2","PNG Sequence");
    muxer->setType(FFMuxer::Sequence);
    muxer->setExtensions(QStringList("png"));
    _muxers << muxer;
    muxer = new FFMuxer("image2","TIFF Sequence");
    muxer->setType(FFMuxer::Sequence);
    muxer->setExtensions(QStringList("tif"));
    _muxers << muxer;
    muxer = new FFMuxer("image2","TARGA Sequence");
    muxer->setType(FFMuxer::Sequence);
    muxer->setExtensions(QStringList("tga"));
    _muxers << muxer;


    std::sort(_muxers.begin(),_muxers.end(),muxerSorter);
}

bool codecSorter(FFCodec *c1,FFCodec *c2)
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
    FFCodec *copyVideo = new FFCodec("copy","Copy video stream",FFCodec::Video | FFCodec::Encoder | FFCodec::Lossless | FFCodec::Lossy | FFCodec::IFrame,this);
    _videoEncoders << copyVideo;
    FFCodec *copyAudio = new FFCodec("copy","Copy video stream",FFCodec::Audio | FFCodec::Encoder | FFCodec::Lossless | FFCodec::Lossy | FFCodec::IFrame,this);
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

            co->setDecoder(match.captured(1) == "D");
            co->setEncoder(match.captured(2) == "E");
            co->setVideo(match.captured(3) == "V");
            co->setAudio(match.captured(3) == "A");
            co->setIframe(match.captured(4) == "I");
            co->setLossy(match.captured(5) == "L");
            co->setLossless(match.captured(6) == "S");

            if (co->isVideo() && co->isEncoder()) _videoEncoders << co;
            else if (co->isAudio() && co->isEncoder()) _audioEncoders << co;
            else if (co->isVideo() && co->isDecoder()) _videoDecoders << co;
            else if (co->isAudio() && co->isDecoder()) _audioDecoders << co;
        }
    }

    std::sort(_videoEncoders.begin(),_videoEncoders.end(),codecSorter);
    std::sort(_audioEncoders.begin(),_audioEncoders.end(),codecSorter);
}

void FFmpeg::readyRead(QString output)
{
    emit newOutput(output);

    _ffmpegOutput = _ffmpegOutput + output;

    QRegularExpression reProgress("frame= *(\\d+) fps= *(\\d+).+ L?size= *(\\d+)kB time=(\\d\\d:\\d\\d:\\d\\d.\\d\\d) bitrate= *(\\d+).\\d+kbits\\/s.* speed= *(\\d+.\\d*)x");
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
        int duration = 0;
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
                int remaining = elapsed*duration/_currentFrame - elapsed;
                _timeRemaining = QTime(0,0,0).addSecs(remaining);
            }
        }
        emit progress();
    }

}
