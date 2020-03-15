#ifndef MEDIAINFO_H
#define MEDIAINFO_H

#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <QDir>
#include <QTemporaryDir>
#include <QtDebug>
#include <QTime>

#include "FFmpeg/ffmpeg.h"
#include "Renderer/audioinfo.h"
#include "Renderer/videoinfo.h"
#include "Renderer/streamreference.h"
#include "utils.h"

class MediaInfo : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief DuMediaInfo Constructs an empty MediaInfo
     * @param parent The QObject parent
     */
    explicit MediaInfo( QObject *parent = nullptr );

    /**
     * @brief DuMediaInfo Constructs a new MediaInfo for the given file.
     * @param mediaFile The media file or a JSON preset. For a frame sequence, it can be any frame from the sequence
     * @param parent The QObject parent
     */
    explicit MediaInfo( QFileInfo mediaFile, QObject *parent = nullptr );

    /**
     * @brief updateInfo Updates all the information for this media file
     * @param mediaFilePath The media file. For a frame sequence, it can be any frame from the sequence
     */
    void update(QFileInfo mediaFile, bool silent = false);
    void copyFrom(MediaInfo *other, bool updateFilename = false, bool silent = false);

    /**
     * @brief reInit Reinit all to default values
     */
    void reInit(bool removeFileName = true, bool silent = false);

    QString getDescription();

    //presets
    QString exportPreset();
    void exportPreset(QString jsonPath);
    void loadPreset(QFileInfo presetFilePath, bool silent = false);

    //general
    void setId(int value);
    int id() const;
    void setMuxer(FFMuxer *muxer, bool silent = false);
    void setMuxer(QString name, bool silent = false);
    FFMuxer *muxer() const;
    QStringList extensions() const;
    FFCodec *defaultVideoCodec() const;
    FFCodec *defaultAudioCodec() const;
    FFPixFormat *defaultPixFormat() const;
    bool hasVideo();
    bool hasAudio();
    bool isSequence();
    qint64 bitrate() const;
    void setContainer(QStringList container, bool silent = false);
    void setDuration(double duration, bool silent = false);
    double duration();
    void setFileName(QString fileName, bool silent = false);
    QString fileName();
    void setSize(double size, MediaUtils::SizeUnit unit = MediaUtils::Bytes, bool silent = false);
    qint64 size();
    void setLoop(int loop, bool silent = false);
    int loop() const;

    //sequence
    void setStartNumber(int startNumber, bool silent = false);
    int startNumber() const;
    void setFrames(const QStringList &frames, bool silent = false);
    QStringList frames() const;
    QString ffmpegSequenceName() const;

    //alpha
    bool hasAlpha();
    bool canHaveAlpha() const;
    void setAlpha(bool alpha, bool silent = false);

    //custom params
    QList<QStringList> ffmpegOptions();
    void setFFmpegOptions(QList<QStringList> options, bool silent = false);
    void addFFmpegOption(QStringList option, bool silent = false);
    void removeFFmpegOptions(QString optionName, bool silent = false);
    void clearFFmpegOptions(bool silent = false);

    //aep
    void setAep(bool isAep, bool silent = false);
    void setAepNumThreads(int aepNumThreads, bool silent = false);
    void setAepCompName(const QString &aepCompName, bool silent = false);
    void setAepRqindex(int aepRqindex, bool silent = false);
    void setAeUseRQueue(bool aeUseRQueue, bool silent = false);
    bool isAep() const;
    QString aepCompName() const;
    int aepNumThreads() const;
    int aepRqindex() const;
    bool aeUseRQueue() const;

    //cache
    void setCacheDir(QTemporaryDir *cacheDir, bool silent = false);
    QTemporaryDir *cacheDir() const;

    //streams
    void addAudioStream(AudioInfo *stream, bool silent = false);
    AudioInfo *takeAudioStream(int index = 0, bool silent = false);
    void clearAudioStreams(bool silent = false);
    void addVideoStream(VideoInfo *stream, bool silent = false);
    VideoInfo *takeVideoStream(int index = 0, bool silent = false);
    void clearVideoStreams(bool silent = false);
    QList<VideoInfo *> videoStreams() const;
    QList<AudioInfo *> audioStreams() const;

    //maps
    QList<StreamReference> maps() const;
    void addMap(int mediaId, int streamId, bool silent = false);
    void removeMap(int index, bool silent = false);
    void removeAllMaps(bool silent = false);
    void setMap(int mapIndex, int mediaId, int streamId, bool silentt = false);
    void setMapMedia(int mapIndex, int mediaId, bool silentt = false);
    void setMapStream(int mapIndex, int streamId, bool silentt = false);

    //stream setters
    //video
    void setVideoQuality(int value, int id = -1, bool silent = false);
    void setVideoEncodingSpeed(int value, int id = -1, bool silent = false);
    void setVideoProfile(QString value, int id = -1, bool silent = false);
    void setVideoProfile(FFProfile *value, int id = -1, bool silent = false);
    void setVideoLevel(QString value, int id = -1, bool silent = false);
    void setVideoTuning(QString value, int id = -1, bool silent = false);
    void setVideoBitrateType(QString type, int id = -1, bool silent = false);
    void setPixAspect(float value, int id = -1, bool silent = false);
    void setPixFormat(QString value, int id = -1, bool silent = false);
    void setPixFormat(FFPixFormat *value, int id = -1, bool silent = false);
    void setVideoBitrate(qint64 value, int id = -1, bool silent = false);
    void setFramerate(double value, int id = -1, bool silent = false);
    void setHeight(int value, int id = -1, bool silent = false);
    void setWidth(int value, int id = -1, bool silent = false);
    void setVideoCodec(QString value, int id = -1, bool silent = false);
    void setVideoCodec(FFCodec *value, int id = -1, bool silent = false);
    void setVideoLanguage(QString value, int id = -1, bool silent = false);
    void setColorPrimaries(QString value, int id = -1, bool silent = false);
    void setColorTRC(QString value, int id = -1, bool silent = false);
    void setColorSpace(QString value, int id = -1, bool silent = false);
    void setColorRange(QString value, int id = -1, bool silent = false);
    void setColorProfile(QString profile, int id = -1, bool silent = false);
    void setPremultipliedAlpha(bool value, int id = -1, bool silent = false);
    //audio
    void setSamplingRate(int value, int id = -1, bool silent = false);
    void setChannels(QString value, int id = -1, bool silent = false);
    void setAudioBitrate(qint64 value, int id = -1, bool silent = false);
    void setAudioCodec(QString value, int id = -1, bool silent = false);
    void setAudioCodec(FFCodec *value, int id = -1, bool silent = false);
    void setAudioLanguage(QString value, int id = -1, bool silent = false);

signals:
    /**
     * @brief changed Emitted when some parameters have been changed.
     */
    void changed();

public slots:

private slots:
    void streamChanged();

private:
    // ========== ATTRIBUTES ==============

    int _id;

    // GENERAL

    /**
     * @brief _extensions The list of file extensions this media can use
     */
    QStringList _extensions;
    /**
     * @brief _muxer The muxer of this media file
     */
    FFMuxer *_muxer;
    /**
     * @brief _duration The duration in seconds
     */
    double _duration;
    /**
     * @brief _size The file size
     */
    qint64 _size;
    /**
     * @brief _bitrate The global bitrate
     */
    qint64 _bitrate;
    /**
     * @brief _fileName The filename of the media
     */
    QString _fileName;
    /**
     * @brief _frames For a frame sequence, the list of all the frames filenames
     */
    QStringList _frames;
    /**
     * @brief _loop The number of loops to be encoded (-1 for inifite, available only with some specific formats like GIF)
     */
    int _loop;
    /**
     * @brief _startNumber The number of the first frame
     */
    int _startNumber;

    QList<VideoInfo *> _videoStreams;
    QList<AudioInfo *> _audioStreams;
    QList<StreamReference> _maps;

    // GENERAL Encoding/decoding parameters

    /**
     * @brief _cacheDir A pointer to the cache for temporary transcoding and other files for this media
     */
    QTemporaryDir *_cacheDir;

    // FFMPEG Encoding/decoding

    /**
     * @brief _ffmpegOptions The custom options to be used by ffmpeg when decoding or encoding this media
     */
    QList<QStringList> _ffmpegOptions;
    /**
     * @brief _sequenceName The name of the file sequence as used by ffmpeg
     */
    QString _ffmpegSequenceName;

    // AFTER EFFECTS Encoding/decoding

    /**
     * @brief _isAep Wether this is an After Effects project (aep, aet or aepx)
     */
    bool _isAep;
    /**
     * @brief _aepNumThreads The number of threads to use when rendering this mediq with After Effects
     */
    int _aepNumThreads;
    /**
     * @brief _aepCompName The name of the composition in this After Effects project to render
     */
    QString _aepCompName;
    /**
     * @brief _aepRqindex The index of the render queue item in this After Effects project to render
     */
    int _aepRqindex;
    /**
     * @brief _aeUseRQueue Wether to launch the render queue when rendering the After Effects project, or render a specific composition or renderqueue item.
     */
    bool _aeUseRQueue;

    // ======== METHODS =============

    /**
     * @brief loadSequence Loads all the frames of the frame sequence
     */
    void loadSequence();

};

#endif // MEDIAINFO_H
