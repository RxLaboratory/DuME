#include "ffmpeg.h"

FFmpeg::FFmpeg(QString path,QObject *parent) : QObject(parent)
{
    ffmpeg = new QProcess(this);
    ffmpeg->setProgram(path);
    getEncoders();
    getHelp();
    getLongHelp();
}

QList<FFmpegCodec> FFmpeg::getEncoders()
{
    if (audioEncoders.count() == 0 || videoEncoders.count() == 0)
    {
        //TODO use signals instead of waiting? it's very quick maybe not needed
        ffmpeg->setArguments(QStringList("-encoders"));
        ffmpeg->start(QIODevice::ReadOnly);
        if (ffmpeg->waitForFinished(3000))
        {
            gotCodecs(ffmpeg->readAll());
        }
    }

    QList<FFmpegCodec> encoders = videoEncoders;
    encoders.append(audioEncoders);

    return encoders;
}

QList<FFmpegCodec> FFmpeg::getVideoEncoders()
{
    if (videoEncoders.count() > 0)
    {
        return videoEncoders;
    }
    getEncoders();
    return videoEncoders;
}

QList<FFmpegCodec> FFmpeg::getAudioEncoders()
{
    if (audioEncoders.count() > 0)
    {
        return audioEncoders;
    }
    getEncoders();
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
        help = ffmpeg->readAll();
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
        longHelp = ffmpeg->readAll();
    }
    return longHelp;
}

void FFmpeg::gotCodecs(QString output)
{
    videoEncoders.clear();
    audioEncoders.clear();
    //get codecs
    QStringList codecs = output.split("\n");
    for (int i = 10 ; i < codecs.count() ; i++)
    {
        QString codec = codecs[i];

        //TODO adjust regexp to detect if encoding and/or decoding

        //if video encoding
        QRegExp reVideo("[\\.D]EV[\\.I][\\.L][\\.S] ([\\S]+)\\s+([^\\(\\n]+)");
        if (reVideo.indexIn(codec) != -1)
        {
            QString codecName = reVideo.cap(1);
            QString codecPrettyName = reVideo.cap(2);
            FFmpegCodec co(codecName,codecPrettyName,true);
            videoEncoders << co;
        }
        //if audio encoding
        QRegExp reAudio("[\\.D]EA[\\.I][\\.L][\\.S] ([\\S]+)\\s+([^\\(\\n]+)");
        if (reAudio.indexIn(codec) != -1)
        {
            QString codecName = reAudio.cap(1);
            QString codecPrettyName = reAudio.cap(2);
            FFmpegCodec co(codecName,codecPrettyName,false);
            audioEncoders << co;
        }
    }
}
