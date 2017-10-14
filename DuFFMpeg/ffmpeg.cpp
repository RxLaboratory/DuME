#include "ffmpeg.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

FFmpeg::FFmpeg(QString path,QObject *parent) : QObject(parent)
{
    ffmpeg = new QProcess(this);
    ffmpeg->setProgram(path);

    currentFrame = 0;
    startTime = QTime(0,0,0);
    outputSize = 0.0;
    outputBitrate = 0;
    encodingSpeed = 0.0;

    status = Waiting;

    //Connect process
    connect(ffmpeg,SIGNAL(readyReadStandardError()),this,SLOT(stdError()));
    connect(ffmpeg,SIGNAL(readyReadStandardOutput()),this,SLOT(stdOutput()));
    connect(ffmpeg,SIGNAL(started()),this,SLOT(started()));
    connect(ffmpeg,SIGNAL(finished(int)),this,SLOT(finished()));
    connect(ffmpeg,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(errorOccurred(QProcess::ProcessError)));
}

void FFmpeg::setBinaryFileName(QString path)
{
    ffmpeg->setProgram(path);
}

QList<FFCodec> FFmpeg::getEncoders(bool reload)
{
#ifdef QT_DEBUG
    qDebug() << "FFmpeg - Getting Encoders";
#endif
    if (audioEncoders.count() == 0 || videoEncoders.count() == 0 || reload)
    {
        //TODO use signals instead of waiting? it's very quick maybe not needed
        ffmpeg->setArguments(QStringList("-codecs"));
        ffmpeg->start(QIODevice::ReadOnly);
        if (ffmpeg->waitForFinished(10000))
        {
            gotCodecs(ffmpegOutput);
        }
    }

    QList<FFCodec> encoders = videoEncoders;
    encoders.append(audioEncoders);

    return encoders;
}

QList<FFCodec> FFmpeg::getVideoEncoders(bool reload)
{
    if (reload || videoEncoders.count() == 0)
    {
        getEncoders(reload);
    }
    return videoEncoders;
}

QList<FFCodec> FFmpeg::getAudioEncoders(bool reload)
{
    if (reload || audioEncoders.count() == 0)
    {
        getEncoders(reload);
    }
    return audioEncoders;
}

QString FFmpeg::getHelp()
{
    if (help != "") return help;

    //if first run, get the help
    //TODO use signals instead of waiting? it's very quick maybe not needed
    ffmpeg->setArguments(QStringList("-h"));
    ffmpeg->start(QIODevice::ReadOnly);
    if (ffmpeg->waitForFinished(3000))
    {
        help = ffmpegOutput;
    }
    return help;
}

QString FFmpeg::getLongHelp()
{
    if (longHelp != "") return longHelp;

    //if first run, get the long help
    //TODO use signals instead of waiting? it's very quick maybe not needed
    QStringList args("-h");
    args << "long";
    ffmpeg->setArguments(args);
    ffmpeg->start(QIODevice::ReadOnly);
    if (ffmpeg->waitForFinished(3000))
    {
        longHelp = ffmpegOutput;
    }
    return longHelp;
}

FFMediaInfo *FFmpeg::getMediaInfo(QString mediaPath)
{
    QStringList args("-i");
    args << QDir::toNativeSeparators(mediaPath);
    ffmpeg->setArguments(args);
    ffmpeg->start(QIODevice::ReadOnly);
    if (ffmpeg->waitForFinished(3000))
    {
        FFMediaInfo *info = new FFMediaInfo(ffmpegOutput,this);
        return info;
    }
}

int FFmpeg::getCurrentFrame()
{
    return currentFrame;
}

QTime FFmpeg::getStartTime()
{
    return startTime;
}

QTime FFmpeg::getElapsedTime()
{
    return QTime(0,0,0).addSecs(startTime.elapsed() / 1000);
}

double FFmpeg::getOutputSize(FFMediaInfo::SizeUnit unit)
{
    double s = outputSize;
    if (unit == FFMediaInfo::KB) s = s/1024;
    if (unit == FFMediaInfo::MB) s = s/1024/1024;
    return s;
}

double FFmpeg::getOutputBitrate(FFMediaInfo::BitrateUnit unit)
{
    double bitrate = outputBitrate;
    if (unit == FFMediaInfo::Kbps) bitrate = bitrate/1024;
    if (unit == FFMediaInfo::Mbps) bitrate = bitrate/1024/1024;
    return bitrate;
}

double FFmpeg::getEncodingSpeed()
{
    return encodingSpeed;
}

QTime FFmpeg::getTimeRemaining()
{
    return timeRemaining;
}

FFQueueItem *FFmpeg::getCurrentItem()
{
    return currentItem;
}

void FFmpeg::encode()
{
    if (status == Encoding) return;
    setStatus(Encoding);

    //launch first item
    encodeNextItem();
}

void FFmpeg::encode(FFQueueItem *item)
{
    encodingQueue << item;
    encode();
}

void FFmpeg::encode(QList<FFQueueItem*> list)
{
    encodingQueue.append(list);
    encode();
}

void FFmpeg::encode(FFMediaInfo *input, QList<FFMediaInfo *> outputs)
{
    FFQueueItem *item = new FFQueueItem(input,outputs,this);
    encodingQueue << item;
    encode();
}

void FFmpeg::encode(FFMediaInfo *input, FFMediaInfo *output)
{
    FFQueueItem *item = new FFQueueItem(input,output,this);
    encodingQueue << item;
    encode();
}

int FFmpeg::addQueueItem(FFQueueItem *item)
{
    encodingQueue.append(item);
    return encodingQueue.count()-1;
}

void FFmpeg::removeQueueItem(int id)
{
    FFQueueItem *i = encodingQueue.takeAt(id);
    delete i;
}

FFQueueItem *FFmpeg::takeQueueItem(int id)
{
    return encodingQueue.takeAt(id);
}

void FFmpeg::clearQueue()
{
    while(encodingQueue.count() > 0)
    {
        removeQueueItem(0);
    }
}

void FFmpeg::stop(int timeout)
{
    if (ffmpeg->state() == QProcess::NotRunning) return;
    ffmpeg->write("q");
    if (!ffmpeg->waitForFinished(timeout))
    {
        ffmpeg->kill();
    }
    setStatus(Waiting);
}

void FFmpeg::stdError()
{
    QString output = ffmpeg->readAllStandardError();
    readyRead(output);
}

void FFmpeg::stdOutput()
{
    QString output = ffmpeg->readAllStandardOutput();
    readyRead(output);
}

void FFmpeg::started()
{
    ffmpegOutput = "";
}

void FFmpeg::finished()
{
    if (status == Encoding)
    {
        currentItem->setStatus(FFQueueItem::Finished);
        emit encodingFinished(currentItem);
        //move to history
        encodingHistory << currentItem;
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

    if (status == Encoding)
    {
        currentItem->setStatus(FFQueueItem::Stopped);
        encodingHistory << currentItem;
    }

    setStatus(Error);
    emit processError(error);
}

void FFmpeg::encodeNextItem()
{
    if (encodingQueue.count() == 0)
    {
        setStatus(Waiting);
        return;
    }

    currentItem = encodingQueue.takeAt(0);

    //generate arguments
    QStringList arguments("-stats");
    arguments << "-y";

    //add inputs
    foreach(FFMediaInfo *input,currentItem->getInputMedias())
    {
        //add custom options
        arguments.append(input->getFFmpegOptions());
        //add input file
        arguments << "-i" << QDir::toNativeSeparators(input->getFileName());
    }
    //add outputs
    foreach(FFMediaInfo *output,currentItem->getOutputMedias())
    {
        //add custom options
        arguments.append(output->getFFmpegOptions());

        //video
        QString codec = output->getVideoCodec().getName();

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
        QString acodec = output->getAudioCodec().getName();

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
    ffmpeg->setArguments(arguments);
    ffmpeg->start(QIODevice::ReadWrite);

    currentItem->setStatus(FFQueueItem::InProgress);
    startTime = QTime::currentTime();
    emit  encodingStarted(currentItem);
}

void FFmpeg::setStatus(Status st)
{
    status = st;
    emit statusChanged(status);
}

void FFmpeg::gotCodecs(QString output)
{
    videoEncoders.clear();
    audioEncoders.clear();
    //get codecs
    QStringList codecs = output.split("\n");
    for (int i = 0 ; i < codecs.count() ; i++)
    {
        QString codec = codecs[i];

        //TODO adjust regexp to detect if encoding and/or decoding

        //if video encoding
        QRegularExpression reVideo("[D.]EV[I.][L.][S.] (\\w+) +([^\\(\\n]+)");
        QRegularExpressionMatch matchVideo = reVideo.match(codec);
        if (matchVideo.hasMatch())
        {

            QString codecName = matchVideo.captured(1);
            QString codecPrettyName = matchVideo.captured(2);
            FFCodec co(codecName,codecPrettyName,true);
            videoEncoders << co;
        }
        //if audio encoding
        QRegularExpression reAudio("[D.]EA[I.][L.][S.] (\\w+) +([^\\(\\n]+)");
        QRegularExpressionMatch matchAudio = reAudio.match(codec);
        if (matchAudio.hasMatch())
        {
            QString codecName = matchAudio.captured(1);
            QString codecPrettyName = matchAudio.captured(2);
            FFCodec co(codecName,codecPrettyName,false);
            audioEncoders << co;
        }
    }
}

void FFmpeg::readyRead(QString output)
{
    emit newOutput(output);

    ffmpegOutput = ffmpegOutput + output;

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
        currentFrame = frame.toInt();

        //size
        int sizeKB = size.toInt();
        outputSize = sizeKB*1024;

        //bitrate
        int bitrateKB = bitrate.toInt();
        outputBitrate = bitrateKB*1024;

        //speed
        encodingSpeed = speed.toDouble();

        //time remaining
        //get current input duration
        //gets the current item duration
        int duration = 0;
        foreach(FFMediaInfo *input,currentItem->getInputMedias())
        {
            if (input->hasVideo())
            {
                duration = input->getDuration() * input->getVideoFramerate();
                break;
            }
        }
        if (duration > 0)
        {
            if (currentFrame > 0)
            {
                int elapsed = startTime.elapsed() / 1000;
                int remaining = elapsed*duration/currentFrame - elapsed;
                timeRemaining = QTime(0,0,0).addSecs(remaining);
            }
        }
        emit progress();
    }

}
