#ifndef RENDERER_H
#define RENDERER_H

#include <QObject>
#include <QDebug>

#include "aerenderer.h"
#include "ffmpeg.h"

/**
 * @brief The Renderer class handles the render queue and render processes (ffmpeg, After Effects, Blender...)
 */
class RendererQueue : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Renderer Constructs the main renderer.
     * @param ffmpeg The FFmpeg transcoder
     * @param aeRenderer The After Effects Renderer
     * @param parent The parent QObject
     */
    explicit RendererQueue( FFmpeg *ffmpeg, AERenderer *aeRenderer, QObject *parent = nullptr);
    ~RendererQueue();

    /**
     * @brief The Status enum Used to describe the current status of the renderer
     */
    enum Status { Initializing, Waiting, FFmpegEncoding, Error, Other, AERendering, BlenderRendering, FramesConversion, Cleaning };
    Q_ENUM(Status)

    /**
     * @brief status Returns the current status of the renderer
     * @return The status
     */
    Status status() const;

    /**
     * @brief getCurrentInputInfos Gets the item currently being encoded
     * @return The queue item
     */
    FFQueueItem *getCurrentItem();
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
     * @brief stop Stops the current process
     * @param timeout Kills the process after timeout if it does not respond. In milliseconds.
     */
    void stop(int timeout = 10000);

    MediaInfo *loadPreset(QString json);
    MediaInfo *loadPresetFromFile(QString jsonFileName);



signals:
    /**
     * @brief statusChanged Emitted when the Renderer status changes.
     */
    void statusChanged(RendererQueue::Status);
    /**
     * @brief newLog Emitted when some debug infos/logs are available
     */
    void newLog(QString);
    /**
     * @brief newError Emitted when a blocking error occurs. Contains the description of the error.
     */
    void newError(QString);

    // === QUEUE ===
    /**
     * @brief encodingStarted Emitted when FFmpeg starts an encoding process, with infos about the input media
     */
    void encodingStarted( FFQueueItem* );
    /**
     * @brief encodingFinished Emitted when the encoding finishes or is stopped
     */
    void encodingFinished( FFQueueItem* );

private slots:
    // removes temp files, cache, restores AE templates...
    void postRenderCleanUp();
    // changes the current status (and emits statusChanged)
    void setStatus(Status st);
    // logs a message
    void log(QString message);
    // logs an error
    void error(QString message);
    // finished current item rendering/transcoding
    void finished();
    void encodeNextItem();

    // === ffmpeg ===

    // logs ffmpeg debug output
    void ffmpegLog(QString message);
    // when ffmpeg encounters an error
    void ffmpegError(QString message);

    // === Ae ===

    // finished ae rendering
    void finishedAe();
    // launch after effects rendering
    void renderAep(MediaInfo *input, bool audio = false);

private:

    // ======= GENERAL =============

    // The application settings
    QSettings settings;
    // The current status
    Status _status;
    // A timer to keep track of the processes
    QTimer *timer;

    // ======= QUEUE =============

    // The items remaining to encode
    QList<FFQueueItem *> _encodingQueue;
    // All the items previously encoded
    QList<FFQueueItem *> _encodingHistory;
    // The item currently encoding
    FFQueueItem *_currentItem;

    // ========== FFMPEG ============

    // The FFmpeg transcoder
    FFmpeg *_ffmpeg;

    // ======= AFTER EFFECTS ========

    // The After Effects renderer
    AERenderer *_aeRenderer;

    // ======== DEBUG and LOGS =======

};

#endif // RENDERER_H
