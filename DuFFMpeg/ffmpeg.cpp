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
}

bool FFmpeg::setBinaryFileName(QString path)
{
    if(QFile(path).exists())
    {
        _ffmpeg->setProgram(path);
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

QList<FFCodec *> FFmpeg::getEncoders(bool reload)
{
#ifdef QT_DEBUG
    qDebug() << "FFmpeg - Getting Encoders";
#endif
    if (_audioEncoders.count() == 0 || _videoEncoders.count() == 0 || reload)
    {
        //TODO use signals instead of waiting? it's very quick maybe not needed
        //signals may actually be better to have a progress bar at launch of the app
        _ffmpeg->setArguments(QStringList("-codecs"));
        _ffmpeg->start(QIODevice::ReadOnly);
        if (_ffmpeg->waitForFinished(10000))
        {
            gotCodecs(_ffmpegOutput);
        }
    }

    QList<FFCodec *> encoders = _videoEncoders;
    encoders.append(_audioEncoders);

    return encoders;
}

QList<FFCodec *> FFmpeg::getVideoEncoders(bool reload)
{
    if (reload || _videoEncoders.count() == 0)
    {
        getEncoders(reload);
    }
    return _videoEncoders;
}

QList<FFCodec *> FFmpeg::getAudioEncoders(bool reload)
{
    if (reload || _audioEncoders.count() == 0)
    {
        getEncoders(reload);
    }
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
    _ffmpeg->write("q");
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
        arguments.append(input->getFFmpegOptions());
        //add input file
        arguments << "-i" << QDir::toNativeSeparators(input->getFileName());
    }
    //add outputs
    foreach(FFMediaInfo *output,_currentItem->getOutputMedias())
    {
        //add custom options
        arguments.append(output->getFFmpegOptions());

        //video
        QString codec = "";
        if (output->getVideoCodec() != nullptr) codec = output->getVideoCodec()->name();


        if (output->hasVideo() && codec != "")
        {
            //codec
            arguments << "-vcodec" << codec;

            if (codec != "copy")
            {
                //bitrate
                int bitrate = output->getVideoBitrate();
                if (bitrate != 0)
                {
                    arguments << "-b:v" << QString::number(bitrate);

                }

                //size
                int width = output->getVideoWidth();
                int height = output->getVideoHeight();
                if (width != 0 && height != 0)
                {
                    arguments << "-s" << QString::number(width) + "x" + QString::number(height);
                }

                //framerate
                double framerate = output->getVideoFramerate();
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
        if (output->getAudioCodec() != nullptr) acodec = output->getAudioCodec()->name();

        if (output->hasAudio() && acodec != "")
        {
            //codec
            arguments << "-acodec" << acodec;

            if (acodec != "copy")
            {
                //bitrate
                int bitrate = output->getAudioBitrate();
                if (bitrate != 0)
                {
                    arguments << "-b:a" << QString::number(output->getAudioBitrate());
                }

                //sampling
                int sampling = output->getAudioSamplingRate();
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
        arguments << QDir::toNativeSeparators(output->getFileName());
    }

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
    for (int i = 0 ; i < codecs.count() ; i++)
    {
        QString codec = codecs[i];

        //if video encoding
        QRegularExpression re("([D.])([E.])([VAS])([I.])([L.])([S.]) (\\w+) +([^\\(\\n]+)");
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
                duration = input->getDuration() * input->getVideoFramerate();
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
