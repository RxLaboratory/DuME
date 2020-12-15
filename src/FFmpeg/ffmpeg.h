#ifndef FFMPEG_H
#define FFMPEG_H

// When debugging, it may be useful to output the codec list being loaded at startup. Uncomment this line
//#define FFMPEG_VERBOSE_DEBUG

#include <QSettings>
#include <QObject>
#include <QDir>
#include <QtDebug>
#include <QCoreApplication>

#include "duqf-utils/utils.h"

#include "Renderer/abstractrendererinfo.h"

#include "ffcodec.h"
#include "ffmuxer.h"
#include "ffpixformat.h"
#include "ffprofile.h"
#include "ffcolorprofile.h"
#include "ffsampleformat.h"

class FFmpeg : public AbstractRendererInfo
{
    Q_OBJECT
public:
    /**
     * @brief FFmpeg Constructs the FFmpeg manager. Note: This constructor does not initializes ffmpeg, you have to run init() before using it.
     * @param path The path to the FFmpeg binary executable
     * @param parent The parent QObject
     */
    explicit FFmpeg(QString path = "", QObject *parent = nullptr);
    ~FFmpeg();

    /**
     * @brief defaultObject The default base object, empty name and "Default" base name
     * @return
     */
    FFBaseObject *defaultObject();
    /**
     * @brief getMuxers Gets the list of available muxers
     * @return
     */
    QList<FFMuxer *> muxers(bool encodeOnly = true);
    /**
     * @brief getMuxer Retrieves a muxer with its name
     * @param name
     * @return
     */
    FFMuxer *muxer(QString nameOrExtension);
    /**
     * @brief getMuxerDefaultCodec Checks what is the default codec for this muxer
     * @param muxer
     * @param ability
     * @return
     */
    FFCodec *muxerDefaultCodec(FFMuxer *muxer, FFCodec::Ability ability = FFCodec::Video);
    /**
     * @brief getMuxerDefaultCodec Checks what is the default codec for this muxer
     * @param name The name of the muxer
     * @param ability
     * @return
     */
    FFCodec *muxerDefaultCodec(QString name, FFCodec::Ability ability = FFCodec::Video);
    /**
     * @brief getEncoders Gets the list of encoders supported the current version of FFmpeg
     * @return The codec list
     */
    QList<FFCodec *> encoders();
    /**
     * @brief getVideoEncoders Gets the list of video encoders supported by the current version of FFmpeg
     * @return The video codec list
     */
    QList<FFCodec *> videoEncoders();
    /**
     * @brief getVideoEncoders Gets the list of audio encoders supported by the current version of FFmpeg
     * @return The audio codec list
     */
    QList<FFCodec *> audioEncoders();
    /**
     * @brief getInputPixFormats Gets the list of input pixel formats supported by the current version of FFmpeg
     * @return The pixel formats list
     */
    QList<FFPixFormat *> pixFormats();
    /**
     * @brief Gets the list of available audio sample formats
     * @return The sample formats list
     */
    QList<FFSampleFormat *> sampleFormats() const;
    /**
     * @brief getCodec Gets a video encoder using its name
     * @param name The name of the codec
     * @return A pointer to the codec
     */
    FFCodec *videoEncoder(QString name);
    /**
     * @brief getCodec Gets an audio encoder using its name
     * @param name The name of the codec
     * @return A pointer to the codec
     */
    FFCodec *audioEncoder(QString name);
    /**
     * @brief getCodec Gets a video encoder using its name
     * @param name The name of the codec
     * @return A pointer to the codec
     */
    FFCodec *videoDecoder(QString name);
    /**
     * @brief getCodec Gets an audio encoder using its name
     * @param name The name of the codec
     * @return A pointer to the codec
     */
    FFCodec *audioDecoder(QString name);
    /**
     * @brief Gets a pixel format using its name
     * @param name The name of the pixel format
     * @return A pointer to the pixel format
     */
    FFPixFormat *pixFormat(QString name);
    /**
     * @brief Gets an audio sample format using its name
     * @param name The name of the sample format
     * @return A pointer to the sample format
     */
    FFSampleFormat *sampleFormat(QString name);
    /**
     * @brief profile Gets a profile unsing its name
     * @param name The name of the profile
     * @return  A pointer to the profile
     */
    FFProfile *profile(QString name);
    //Colors
    FFBaseObject *colorTRC(QString name);
    FFBaseObject *colorPrimary(QString name);
    FFBaseObject *colorSpace(QString name);
    FFBaseObject *colorRange(QString name);
    FFColorProfile *colorProfile(QString name);
    QList<FFBaseObject *> colorPrimaries() const;
    QList<FFBaseObject *> colorTRCs() const;
    QList<FFBaseObject *> colorSpaces() const;
    QList<FFBaseObject *> colorRanges() const;
    QList<FFColorProfile *> colorProfiles() const;
    // Tunings
    FFBaseObject *tuning(QString name);
    QList<FFBaseObject *> tunings() const;
    /**
     * @brief getHelp Gets the help text of FFmpeg
     * @return The documentation
     */
    QString help();
    /**
     * @brief getLongHelp Gets the longer help of FFmpeg
     * @return The longer version of the documentation
     */
    QString longHelp();
    /**
     * @brief analyseMedia Gets the information for the media
     * @param mediaPath The path to the media file
     * @return The information returned by FFmpeg
     */
    QString analyseMedia(QString mediaPath);
    /**
     * @brief getVersion Gets the current ffmpeg version
     * @return
     */
    QString version() const;
    /**
     * @brief status The current FFmpeg Status
     * @return
     */
    MediaUtils::RenderStatus status() const;

signals:
    void progress(int);
    void progressMax(int);

public slots:
    /**
     * @brief setBinaryFileName Sets the path to the FFmpeg binary
     * @param path The path to the binary executable file
     * @return true if the exe is found
     */
    bool setBinary(QString path, bool initialize = true);
    void init();

private:

    // === ATTRIBUTES ===
    // The ffmpeg version
    QString _version;

    // The list of video encoders
    QList<FFCodec *> _videoEncoders;
    // The list of audio encoders
    QList<FFCodec *> _audioEncoders;
    // The list of video decoders
    QList<FFCodec *> _videoDecoders;
    // The list of audio decoders
    QList<FFCodec *> _audioDecoders;
    // The default coder
    FFCodec *_defaultCodec;
    // The list of muxers
    QList<FFMuxer *> _encodeMuxers;
    QList<FFMuxer *> _decodeMuxers;
    QList<FFMuxer *> _muxers;
    // The default muxer
    FFMuxer *_defaultMuxer;
    // The list of pixel formats
    QList<FFPixFormat *> _pixFormats;
    // The list of audio sample formats
    QList<FFSampleFormat *> _sampleFormats;
    // The default pixel formats
    FFPixFormat *_defaultPixFormat;
    // The default audio sample format
    FFSampleFormat *_defaultSampleFormat;
    // The list of profiles
    QList<FFProfile *> _profiles;
    // The list of color profiles
    QList<FFBaseObject *> _colorPrimaries;
    QList<FFBaseObject *> _colorTRCs;
    QList<FFBaseObject *> _colorSpaces;
    QList<FFBaseObject *> _colorRanges;
    // The list of color profiles
    QList<FFColorProfile *> _colorProfiles;
    // The list of fine tunings
    QList<FFBaseObject *> _tunings;
    // A default object
    FFBaseObject *_defaultObject;
    // The help
    QString _help;
    // The documentation
    QString _longHelp;

    // The progression of init
    int _progressMax;
    int _prevMax;
    int _currentProgress;

    //=== Process outputs ===
    /**
     * @brief ffmpeg_gotVersion Parses the version
     * @param output The output of the FFmpeg process with the muxers list
     */
    QString gotVersion(QString output);
    /**
     * @brief ffmpeg_gotCodecs Parses the muxers list
     * @param output The output of the FFmpeg process with the muxers list
     * @param newVersion The version of ffmpeg from which tt output comes from
     */
    void gotMuxers(QString output, QString newVersion);
    /**
     * @brief ffmpeg_gotCodecs Parses the codec list
     * @param output The output of the FFmpeg process with the codecs list
     * @param newVersion The version of ffmpeg from which tt output comes from
     */
    void gotCodecs(QString output, QString newVersion);
    /**
     * @brief ffmpeg_gotPixFormats Parses the pix formats list
     * @param output The output of the FFmpeg process with the codecs list
     * @param newVersion The version of ffmpeg from which the output comes from
     */
    void gotPixFormats(QString output, QString newVersion);
    /**
     * @brief Parses the sample formats list
     * @param output The output of the FFmpeg process with the sample formats list
     * @param newVersion The version of ffmpeg from which the output comes from
     */
    void gotSampleFormats(QString output, QString newVersion);

    // Is the process available and working
    bool _valid;

};

#endif // FFMPEG_H
