#ifndef FFMPEG_H
#define FFMPEG_H

#include <QObject>

#include <QProcess>
#include <QTime>
#include <QDir>
#include <algorithm>

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
    enum Status { Waiting, Encoding, Error, Other };
    Q_ENUM(Status)

    /**
     * @brief getEncoders Gets the list of encoders supported the current version of FFmpeg
     * @param reload Forces reloading the list from the ffmpeg binary
     * @return The codec list
     */
    QList<FFCodec *> getEncoders(bool reload = false);
    /**
     * @brief getVideoEncoders Gets the list of video encoders supported by the current version of FFmpeg
     * @param reload Forces reloading the list from the ffmpeg binary
     * @return The video codec list
     */
    QList<FFCodec *> getVideoEncoders(bool reload = false);
    /**
     * @brief getVideoEncoders Gets the list of audio encoders supported by the current version of FFmpeg
     * @param reload Forces reloading the list from the ffmpeg binary
     * @return The audio codec list
     */
    QList<FFCodec *> getAudioEncoders(bool reload = false);
    /**
     * @brief getCodec Gets a video encoder using its name
     * @param name The name of the codec
     * @return A pointer to the codec
     */
    FFCodec *getVideoEncoder(QString name);
    /**
     * @brief getCodec Gets an audio encoder using its name
     * @param name The name of the codec
     * @return A pointer to the codec
     */
    FFCodec *getAudioEncoder(QString name);
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
     * @brief getMediaInfo Gets the information for the media
     * @param mediaPath The path to the media file
     * @return The information returned by FFmpeg
     */
    QString getMediaInfoString(QString mediaPath);
    /**
     * @brief getCurrentFrame Gets the number of the latest encoded frame
     * @return The number of the latest encoded frame
     */
    int getCurrentFrame();
    /**
     * @brief getStartTime Gets the time when the latest encoding process started
     * @return The time
     */
    QTime getStartTime();
    /**
     * @brief getElapsedTime Gets the time elapsed since the encoding started
     * @return The time elapsed
     */
    QTime getElapsedTime();
    /**
     * @brief getOutputSize Gets the current size of the output file being encoded
     * @param The unit of the size
     * @return The size
     */
    double getOutputSize(FFMediaInfo::SizeUnit unit = FFMediaInfo::Bytes);
    /**
     * @brief getOutputBitrate Gets the average bitrate of the output file being encoded
     * @param unit The unit of the bitrate
     * @return The bitrate
     */
    double getOutputBitrate(FFMediaInfo::BitrateUnit unit = FFMediaInfo::Bits);
    /**
     * @brief getEncodingSpeed Gets the speed of the current encoding
     * @return The speed
     */
    double getEncodingSpeed();
    /**
     * @brief getTimeRemaining Gets the estimated time remaining before the encoding process finishes
     * @return The time remaining
     */
    QTime getTimeRemaining();
    /**
     * @brief getCurrentInputInfos Gets the item currently being encoded
     * @return The queue item
     */
    FFQueueItem *getCurrentItem();
    /**
     * @brief getLastError Gets the last error that occured
     * @return The error
     */
    QProcess::ProcessError getLastError();
    /**
     * @brief getLastErrorMessage Gets a human readable description of the last error
     * @return The description
     */
    QString getLastErrorMessage();
    /**
     * @brief getStatus Gets the current status of FFmpeg
     * @return The status
     */
    Status getStatus();
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
    /**
     * @brief stop Stops the current FFmpeg process
     * @param timeout Kills the process after timeout if it does not respond. In milliseconds.
     */
    void stop(int timeout = 10000);

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
    void encodingFinished(FFQueueItem*);
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
    void statusChanged(FFmpeg::Status);
    /**
     * @brief binaryChanged Emitted when the path to the binary has been changed
     */
    void binaryChanged();

public slots:   
    /**
     * @brief setBinaryFileName Sets the path to the FFmpeg binary
     * @param path The path to the binary executable file
     * @return true if the exe is found
     */
    bool setBinaryFileName(QString path);
    /**
     * @brief runCommand Runs FFmpeg with the commands
     * @param commands The arguments, space separated. Use double quotes for any argument containing spaces
     */
    void runCommand(QString commands);
    /**
     * @brief runCommand Runs FFmpeg with the commands
     * @param commands The arguments
     */
    void runCommand(QStringList commands);

private slots:
    //FFmpeg signals
    void stdError();
    void stdOutput();
    void started();
    void finished();
    void errorOccurred(QProcess::ProcessError e);

    //Queue
    void encodeNextItem();

    //self
    void setStatus(Status st);

private:
    //=== About FFmpeg ===
    /**
     * @brief ffmpeg The process used to handle the binary
     */
    QProcess *_ffmpeg;
    /**
     * @brief videoEncoders The list of the encoders supported by the current version of FFmpeg
     */
    QList<FFCodec *> _videoEncoders;
    /**
     * @brief audioEncoders The list of the encoders supported by the current version of FFmpeg
     */
    QList<FFCodec *> _audioEncoders;
    QList<FFCodec *> _videoDecoders;
    QList<FFCodec *> _audioDecoders;
    /**
     * @brief help The FFmpeg help returned by the -h command
     */
    QString _help;
    /**
     * @brief longHelp The longer FFmpeg help returned by the -h long command
     */
    QString _longHelp;
    /**
     * @brief ffmpegOutput The complete output of the latest ffmpeg process until it has finished
     */
    QString _ffmpegOutput;
    /**
     * @brief status FFmpeg current status
     */
    Status _status;
    /**
     * @brief lastError The last error that occured
     */
    QProcess::ProcessError _lastError;
    /**
     * @brief lastErrorMessage The human readable description of the last error
     */
    QString _lastErrorMessage;
    //=== Current Encoding ===
    int _currentFrame;
    QTime _startTime;
    double _outputSize;
    int _outputBitrate;
    double _encodingSpeed;
    QTime _timeRemaining;
    //=== Queue ===
    /**
     * @brief encodingQueue The queue of items to be encoded
     */
    QList<FFQueueItem *> _encodingQueue;
    /**
     * @brief encodingHistory The list of items which has been encoded
     */
    QList<FFQueueItem *> _encodingHistory;
    /**
     * @brief currentItem The item currently being encoded
     */
    FFQueueItem *_currentItem;
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
    //bool codecSorter(FFCodec *c1, FFCodec *c2);
};

#endif // FFMPEG_H
