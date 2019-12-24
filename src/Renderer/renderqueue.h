#ifndef RENDERER_H
#define RENDERER_H

#include <QObject>
#include <QDebug>
#include <QTimer>

#include "FFmpeg/ffmpegrenderer.h"
#include "AfterEffects/aerenderer.h"
#include "FFmpeg/ffmpeg.h"
#include "AfterEffects/aftereffects.h"

#include "queueitem.h"

/**
 * @brief The Renderer class handles the render queue and render processes (ffmpeg, After Effects, Blender...)
 */
class RenderQueue : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Renderer Constructs the main renderer.
     * @param ffmpeg The FFmpeg transcoder
     * @param aeRenderer The After Effects Renderer
     * @param parent The parent QObject
     */
    explicit RenderQueue( FFmpeg *ffmpeg, AfterEffects *afterEffects, QObject *parent = nullptr);
    ~RenderQueue();

    /**
     * @brief status Returns the current status of the renderer
     * @return The status
     */
    MediaUtils::Status status() const;

    /**
     * @brief getCurrentInputInfos Gets the item currently being encoded
     * @return The queue item
     */
    QueueItem *getCurrentItem();
    /**
     * @brief encode Launches the encoding of the current queue
     */
    void encode();
    /**
     * @brief encode Launches the encoding of the given item
     * @param item The item to encode
     */
    void encode(QueueItem *item);
    /**
     * @brief encode Launches the encoding of the given list of items
     * @param list The items to be encoded
     */
    void encode(QList<QueueItem*> list);
    /**
     * @brief encode Launches the encoding of the given input media with multiple outputs
     * @param input The input media
     * @param outputs The list of output medias
     */
    void encode(MediaInfo *input,QList<MediaInfo*> outputs);
    /**
     * @brief encode Launches the encoding of the given input to the given output
     * @param input The input media
     * @param output The output media
     */
    void encode(MediaInfo *input, MediaInfo *output);
    /**
     * @brief addQueueItem Adds an item to the encoding queue
     * @param item
     * @return The item id
     */
    int addQueueItem(QueueItem *item);
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
    QueueItem *takeQueueItem(int id);
    /**
     * @brief clearQueue Clears the current queue and deletes the items
     */
    void clearQueue();
    /**
     * @brief stop Stops the current process
     * @param timeout Kills the process after timeout if it does not respond. In milliseconds.
     */
    void stop(int timeout = 10000);

signals:
    /**
     * @brief statusChanged Emitted when the Renderer status changes.
     */
    void statusChanged(MediaUtils::Status);
    /**
     * @brief newLog Emitted when some debug logs are available
     */
    void newLog( QString, LogUtils::LogType lt = LogUtils::Information );

    // === QUEUE ===
    /**
     * @brief encodingStarted Emitted when the queue starts an encoding process, with infos about the input media
     */
    void encodingStarted( QueueItem* );
    /**
     * @brief encodingFinished Emitted when the encoding finishes or is stopped
     */
    void encodingFinished();
    /**
     * @brief progress Emitted when some progression information is available
     */
    void progress();

private slots:
    // removes temp files, cache, restores AE templates...
    void postRenderCleanUp();
    // changes the current status (and emits statusChanged)
    void setStatus(MediaUtils::Status st);

    // finished current item rendering/transcoding
    void finished();
    // encodes the next item in the queue
    void encodeNextItem();

    // === ffmpeg ===

    // logs ffmpeg output
    void ffmpegLog(QString message, LogUtils::LogType lt = LogUtils::Information );
    void ffmpegFinished();
    void ffmpegProgress();
    // launch ffmpeg transcoding
    void renderFFmpeg(QueueItem *item );

    // === Ae ===

    // logs ae output
    void aeLog(QString message, LogUtils::LogType lt = LogUtils::Information );
    void aeProgress();
    void aeFinished();
    // launch after effects rendering
    void renderAep( MediaInfo *input, bool audio = false );

private:

    // ======= GENERAL =============

    // The application settings
    QSettings settings;
    // The current status
    MediaUtils::Status _status;
    // A timer to keep track of the processes
    QTimer *timer;

    // ======= QUEUE =============

    // The items remaining to encode
    QList<QueueItem *> _encodingQueue;
    // All the items previously encoded
    QList<QueueItem *> _encodingHistory;
    // The item currently encoding
    QueueItem *_currentItem;

    // ========== FFMPEG ============

    // The FFmpeg transcoder info
    FFmpeg *_ffmpeg;
    // The FFmpeg transcoder
    FFmpegRenderer *_ffmpegRenderer;

    // ======= AFTER EFFECTS ========

    // The After Effects renderer info
    AfterEffects *_ae;
    // The After Effects renderer
    AERenderer *_aeRenderer;

    // ======= RENDERING PROCESS ========

    // the number of frames to render
    int _numFrames;
    // the framerate of the video
    double _frameRate;
    // the current frame being renderered.
    int _currentFrame;
    // the starttime of the rendering process
    QTime _startTime;
    // the current output size of the rendered file in Bytes
    double _outputSize;
    // the output bitrate of the renderered file in bps
    double _outputBitrate;
    // the expected output size
    double _expectedSize;
    // the speed of the encoding compared to the speed of the video
    double _encodingSpeed;
    // the time remaining before rendering completion
    QTime _timeRemaining;
    // the elapsed time
    QTime _elapsedTime;

};

#endif // RENDERER_H
