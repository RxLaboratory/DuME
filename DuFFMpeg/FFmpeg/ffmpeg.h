#ifndef FFMPEG_H
#define FFMPEG_H

#include <QProcess>
#include <QSettings>

#include "ffobject.h"
#include "ffcodec.h"
#include "ffqueueitem.h"
#include "ffmuxer.h"
#include "ffpixformat.h"

class FFmpeg : public FFObject
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
     * @brief getMuxers Gets the list of available muxers
     * @return
     */
    QList<FFMuxer *> getMuxers();
    /**
     * @brief getMuxer Retrieves a muxer with its name
     * @param name
     * @return
     */
    FFMuxer *getMuxer(QString name);
    /**
     * @brief getMuxerDefaultCodec Checks what is the default codec for this muxer
     * @param muxer
     * @param ability
     * @return
     */
    FFCodec *getMuxerDefaultCodec(FFMuxer *muxer, FFCodec::Ability ability = FFCodec::Video);
    /**
     * @brief getMuxerDefaultCodec Checks what is the default codec for this muxer
     * @param name The name of the muxer
     * @param ability
     * @return
     */
    FFCodec *getMuxerDefaultCodec(QString name, FFCodec::Ability ability = FFCodec::Video);
    /**
     * @brief getEncoders Gets the list of encoders supported the current version of FFmpeg
     * @return The codec list
     */
    QList<FFCodec *> getEncoders();
    /**
     * @brief getVideoEncoders Gets the list of video encoders supported by the current version of FFmpeg
     * @return The video codec list
     */
    QList<FFCodec *> getVideoEncoders();
    /**
     * @brief getVideoEncoders Gets the list of audio encoders supported by the current version of FFmpeg
     * @return The audio codec list
     */
    QList<FFCodec *> getAudioEncoders();
    /**
     * @brief getInputPixFormats Gets the list of input pixel formats supported by the current version of FFmpeg
     * @return The pixel formats list
     */
    QList<FFPixFormat *> getPixFormats();
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
     * @brief getPixFormat Gets a pixel format using its name
     * @param name The name of the pixel format
     * @return A pointer to the pixel format
     */
    FFPixFormat *getPixFormat(QString name);
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
     * @brief analyseMedia Gets the information for the media
     * @param mediaPath The path to the media file
     * @return The information returned by FFmpeg
     */
    QString analyseMedia(QString mediaPath);
    /**
     * @brief getVersion Gets the current ffmpeg version
     * @return
     */
    QString getVersion() const;

signals:
    /**
     * @brief newOutput Emitteed when FFmpeg outputs on stderr or stdoutput
     */
    void newOutput(QString);
    /**
     * @brief newError Emitted when a blocking error occurs. Contains the description of the error.
     */
    void newError(QString);
    /**
     * @brief newLog Emitted when some debug infos are available
     */
    void newLog(QString);

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
    bool setBinaryFileName(QString path, bool initialize = true);
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
    void init();

private slots:
    //FFmpeg signals
    void stdError();
    void stdOutput();
    void errorOccurred(QProcess::ProcessError e);

private:

    // === ATTRIBUTES ===

    QSettings settings;
    // The ffmpeg process
    QProcess *ffmpegProcess;
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
    // The list of muxers
    QList<FFMuxer *> _muxers;
    // The list of pixel formats
    QList<FFPixFormat *> _pixFormats;
    // The help
    QString _help;
    // The documentation
    QString _longHelp;
    // The output from the process
    QString _ffmpegOutput;

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
     * @param newVersion The version of ffmpeg from which tt output comes from
     */
    void gotPixFormats(QString output, QString newVersion);
    /**
     * @brief readyRead Called when FFmpeg outputs somehting on stdError or stdOutput
     * @param The output from FFmpeg
     */
    void readyRead(QString output);

};

#endif // FFMPEG_H
