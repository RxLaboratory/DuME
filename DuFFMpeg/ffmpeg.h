#ifndef FFMPEG_H
#define FFMPEG_H

#include "ffobject.h"

#include <QProcess>
#include <QTime>
#include <QDir>
#include <algorithm>
#include <QSettings>
#include <QTemporaryDir>
#include <QStandardPaths>
#include <QTimer>
#include <QThread>

#include "ffcodec.h"
#include "mediainfo.h"
#include "ffqueueitem.h"
#include "ffmuxer.h"
#include "ffpixformat.h"
#include "aerenderer.h"

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
     * @brief The Status enum Used to describe the current status of ffmpeg
     */
    enum Status { Waiting, Encoding, Error, Other, AERendering, Cleaning };
    Q_ENUM(Status)

    QList<FFMuxer *> getMuxers();
    FFMuxer *getMuxer(QString name);
    FFCodec *getMuxerDefaultCodec(FFMuxer *muxer, FFCodec::Ability ability = FFCodec::Video);
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
    double getOutputSize(MediaInfo::SizeUnit unit = MediaInfo::Bytes);
    /**
     * @brief getOutputBitrate Gets the average bitrate of the output file being encoded
     * @param unit The unit of the bitrate
     * @return The bitrate
     */
    double getOutputBitrate(MediaInfo::BitrateUnit unit = MediaInfo::Bits);
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
     * @brief start Starts an encoding process
     * @param arguments The arguments to pass to ffmpeg
     */
    void start( QStringList arguments );
    /**
     * @brief stop Stops the current process
     * @param timeout Kills the process after timeout if it does not respond. In milliseconds.
     */
    void stop(int timeout = 10000);

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
     * @brief progress Emitted each time the transcoding process outputs new stats
     */
    void progress();

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
    void started();
    void errorOccurred(QProcess::ProcessError e);

private:

    // === ATTRIBUTES ===

    QSettings settings;
    // The ffmpeg process
    QProcess *ffmpegProcess;
    // The ffmpeg version
    QString _version;

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
    QList<FFMuxer *> _muxers;
    QList<FFPixFormat *> _pixFormats;
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
