#ifndef FFMPEG_H
#define FFMPEG_H

#include <QObject>

#include <QProcess>
#include <QTime>

#include "ffmpegcodec.h"
#include "mediainfo.h"

class FFmpeg : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief FFmpeg Constructs the FFmpeg manager
     * @param path The path to the FFmpeg binary executable
     * @param parent The parent QObject
     */
    explicit FFmpeg(QString path, QObject *parent = nullptr);
    /**
     * @brief getEncoders Gets the list of encoders supported the current version of FFmpeg
     * @return The codec list
     */
    QList<FFmpegCodec> getEncoders();
    /**
     * @brief getVideoEncoders Gets the list of video encoders supported by the current version of FFmpeg
     * @return The video codec list
     */
    QList<FFmpegCodec> getVideoEncoders();
    /**
     * @brief getVideoEncoders Gets the list of audio encoders supported by the current version of FFmpeg
     * @return The audio codec list
     */
    QList<FFmpegCodec> getAudioEncoders();
    /**
     * @brief getHelp Gets the help text of FFmpeg
     * @return The documentation
     */
    QString getHelp();
    /**
     * @brief getLongHelp Gets the longer help of FFmpeg
     * @return The longer version of the documentation
     */
    QString getLongHelp();

signals:
    /**
     * @brief newOutput Emitteed when FFmpeg outputs on stderr or stdoutput
     */
    void newOutput(QString);
    /**
     * @brief encodingStarted Emitted when FFmpeg starts an encoding process, with infos about the input media
     */
    void encodingStarted(MediaInfo*);
    /**
     * @brief progress Emitted each time the transcoding process outputs new stats
     */
    void progress();


public slots:

private slots:
    //FFmpeg signals
    void stdError();
    void stdOutput();
    void started();
    void finished();
    void errorOccurred(QProcess::ProcessError e);

private:
    //=== About FFmpeg ===
    /**
     * @brief ffmpeg The process used to handle the binary
     */
    QProcess *ffmpeg;
    /**
     * @brief videoEncoders The list of the encoders supported by the current version of FFmpeg
     */
    QList<FFmpegCodec> videoEncoders;
    /**
     * @brief audioEncoders The list of the encoders supported by the current version of FFmpeg
     */
    QList<FFmpegCodec> audioEncoders;
    /**
     * @brief help The FFmpeg help returned by the -h command
     */
    QString help;
    /**
     * @brief longHelp The longer FFmpeg help returned by the -h long command
     */
    QString longHelp;
    /**
     * @brief ffmpegOutput The complete output of the latest ffmpeg process until it has finished
     */
    QString ffmpegOutput;
    //=== Current Encoding ===
    int currentFrame;
    QTime startTime;
    double outputSize;
    int outputBitrate;
    double encodingSpeed;
    QTime timeRemaining;
    MediaInfo *inputInfos;
    //=== Process outputs ===
    /**
     * @brief ffmpeg_gotCodecs Parses the codec list
     * @param output The output of the FFmpeg process with the codecs list
     */
    void gotCodecs(QString output);
    /**
     * @brief readyRead Called when FFmpeg outputs somehting on stdError or stdOutput
     * @param The output from FFmpeg
     */
    void readyRead(QString output);
};

#endif // FFMPEG_H
