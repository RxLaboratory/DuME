#ifndef FFMPEG_H
#define FFMPEG_H

#include <QObject>

#include <QProcess>
#include <QTime>
#include <QDir>

#include "ffcodec.h"
#include "ffmediainfo.h"
#include "ffqueueitem.h"

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
     * @brief The Status enum Used to describe the current status of ffmpeg
     */
    enum Status { Waiting, Encoding, Error };
    Q_ENUM(Status)

    /**
     * @brief getEncoders Gets the list of encoders supported the current version of FFmpeg
     * @return The codec list
     */
    QList<FFCodec> getEncoders();
    /**
     * @brief getVideoEncoders Gets the list of video encoders supported by the current version of FFmpeg
     * @return The video codec list
     */
    QList<FFCodec> getVideoEncoders();
    /**
     * @brief getVideoEncoders Gets the list of audio encoders supported by the current version of FFmpeg
     * @return The audio codec list
     */
    QList<FFCodec> getAudioEncoders();
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
    /**
     * @brief getMediaInfo Gets the information for the media
     * @param mediaPath The path to the media file
     * @return All informations
     */
    FFMediaInfo *getMediaInfo(QString mediaPath);
    /**
     * @brief encode Launches the encoding of the current queue
     */
    void encode();
    /**
     * @brief encode Launches the encoding of the given item
     * @param item The item to encode
     */
    void encode(FFQueueItem *item);
    /**
     * @brief encode Launches the encoding of the given list of items
     * @param list The items to be encoded
     */
    void encode(QList<FFQueueItem*> list);
    /**
     * @brief encode Launches the encoding of the given input media with multiple outputs
     * @param input The input media
     * @param outputs The list of output medias
     */
    void encode(FFMediaInfo *input,QList<FFMediaInfo*> outputs);
    /**
     * @brief encode Launches the encoding of the given input to the given output
     * @param input The input media
     * @param output The output media
     */
    void encode(FFMediaInfo *input, FFMediaInfo *output);
    /**
     * @brief addQueueItem Adds an item to the encoding queue
     * @param item
     * @return The item id
     */
    int addQueueItem(FFQueueItem *item);
    /**
     * @brief removeQueueItem Removes the item from the encoding queue.
     * The item will be deleted
     * @param id The id of the item to remove
     */
    void removeQueueItem(int id);
    /**
     * @brief takeQueueItem Removes the item from the encoding queue and returns it.
     * @param id The id of the item to take
     * @return The item, or nullptr if not found
     */
    FFQueueItem *takeQueueItem(int id);
    /**
     * @brief clearQueue Clears the current queue and deletes the items
     */
    void clearQueue();

signals:
    /**
     * @brief newOutput Emitteed when FFmpeg outputs on stderr or stdoutput
     */
    void newOutput(QString);
    /**
     * @brief encodingStarted Emitted when FFmpeg starts an encoding process, with infos about the input media
     */
    void encodingStarted(FFQueueItem*);
    /**
     * @brief encodingFinished Emitted when the encoding finishes or is stopped
     */
    void encodingFinished();
    /**
     * @brief progress Emitted each time the transcoding process outputs new stats
     */
    void progress();
    /**
     * @brief error Emitted when an error occured with the FFmpeg process
     * Emits a human readable string
     */
    void processError(QString);
    /**
     * @brief statusChanged Emitted when FFmpeg status changes
     */
    void statusChanged(Status);

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
    QList<FFCodec> videoEncoders;
    /**
     * @brief audioEncoders The list of the encoders supported by the current version of FFmpeg
     */
    QList<FFCodec> audioEncoders;
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
    /**
     * @brief status FFmpeg current status
     */
    Status status;
    //=== Current Encoding ===
    int currentFrame;
    QTime startTime;
    double outputSize;
    int outputBitrate;
    double encodingSpeed;
    QTime timeRemaining;
    FFMediaInfo *inputInfos;
    //=== Queue ===
    /**
     * @brief encodingQueue The queue of items to be encoded
     */
    QList<FFQueueItem *> encodingQueue;
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
