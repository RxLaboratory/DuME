#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QFile>
#include <QObject>
#include <QRegularExpression>
#include <QDebug>
#include <QTime>
#include <QDir>
#include <QProcess>
#include <QApplication>

namespace FileUtils
{
    QString applicationTempPath();
    void setReadWrite(QFile *file);
    void setReadWrite(QString path);
    void move(QString from, QString to);
    void copy(QString from, QString to);
    void remove(QString path);
    qint64 getDirSize(QDir d);
    void openInExplorer(QString path);

    /**
     * @brief moveToTrash Moves a file to the trash, works on Linux, Mac OS, Windows.
     * If Qt < 5.15 uses a custom implementation
     * If Qt > 5.15 uses QFile::moveToTrash(QString &fileName)
     * @param fileName
     * @return
     */
    bool moveToTrash(const QString &fileName);
};

namespace MediaUtils
{
    Q_NAMESPACE
    /**
     * @brief The unit used for bitrates
     */
    enum BitrateUnit { bps, kbps, Mbps};
    Q_ENUM_NS(BitrateUnit)

    /**
     * @brief The unit used for sizes
     */
    enum SizeUnit { Bytes, kB, MB, GB};
    Q_ENUM_NS(SizeUnit)

    /**
     * @brief The BitrateType enum, either Variable Bit Rate or Constant Bit Rate
     */
    enum BitrateType { VBR, CBR };
    Q_ENUM_NS(BitrateType)

    /**
     * @brief The Status enum Used to describe the current status of the renderer
     */
    enum RenderStatus { Initializing, Waiting, Launching, Encoding, FramesConversion, FFmpegEncoding, AERendering, BlenderRendering, Cleaning, Finished, Stopped, Error, Other };
    Q_ENUM_NS(RenderStatus)
    QString RenderStatusToHumanString(RenderStatus status);
    /**
     * @brief isEncoding Checks if the current status is a busy (encoding or pre/post steps for encoding) status.
     * @param status
     * @return
     */
    bool isBusy(MediaUtils::RenderStatus status);

    /**
     * @brief The Deinterlace Parity
     */
    enum DeinterlaceParity { TopFieldFirst, BottomFieldFirst, AutoParity };
    Q_ENUM_NS(DeinterlaceParity)
    DeinterlaceParity DeinterlaceParityFromString(QString parity);
    QString DeinterlaceParityToString(DeinterlaceParity parity);

    /**
     * @brief The Motion Interpolation Modes
     */
    enum MotionInterpolationMode { NoMotionInterpolation, DuplicateFrames, BlendFrames, MCIO, MCIAO };
    Q_ENUM_NS(MotionInterpolationMode)
    MotionInterpolationMode MotionInterpolationModeFromString(QString mode);
    QString MotionInterpolationModeToString(MotionInterpolationMode mode);

    enum ResizeMode{ Letterbox, Stretch, Crop};
    Q_ENUM_NS(ResizeMode)
    ResizeMode ResizeModeFromString(QString mode);
    QString ResizeModeToString(ResizeMode mode);

    enum ColorConversionMode{ ConvertEmbed, Convert, Embed };
    Q_ENUM_NS(ColorConversionMode)
    ColorConversionMode ColorConversionModeModeFromString(QString mode);
    QString ColorConversionModeToString(ColorConversionMode mode);

    /**
     * @brief statusString Converts the status as a human readable string to be used in the UI
     * @param status
     * @return
     */
    QString statusString(MediaUtils::RenderStatus status);

    /**
     * @brief sizeString converts a size in bytes to a human-readable string
     * @param size
     * @return
     */
    QString sizeString(qint64 size);

    QString bitrateString(qint64 bitrate);

    /**
     * @brief Converts a duration in seconds to a human-readable time code HH:MM:SS
     * @param duration The duration, in seconds
     * @return The Timecode HH:MM:SS
     */
    QString durationToTimecode(double duration);

    double timecodeToDuration(QString timecode);

    /**
     * @brief convertBitrate Converts a bitrate from bps to another unit.
     * @param to
     * @return
     */
    double convertFromBps( qint64 value, BitrateUnit to );

    /**
     * @brief convertBitrate Converts a file size from Bytes to another unit.
     * @param to
     * @return
     */
    double convertFromBytes( qint64 value, SizeUnit to );

    /**
     * @brief convertBitrate Converts a bitrate to bps from another unit.
     * @param to
     * @return
     */
    qint64 convertToBps( qint64 value, BitrateUnit from );

    /**
     * @brief convertBitrate Converts a file size to Bytes from another unit.
     * @param to
     * @return
     */
    qint64 convertToBytes( qint64 value, SizeUnit from );
};

namespace NetworkUtils
{
    Q_NAMESPACE

    enum NetworkStatus { Connecting, Online, Offline };
    Q_ENUM_NS(NetworkStatus)

};

namespace RegExUtils {
    QRegularExpression getRegularExpression(QString name , QString replace = "", QString by = "");
    /**
     * @brief getRegExp Don't use this, always prefer getRegularExpression!
     * getRegExp is meant only to be used with QRegExpValidator which doesn't accept QRegularExpression
     * @param name
     * @param replace
     * @param by
     * @return
     */
    QRegExp getRegExp(QString name , QString replace = "", QString by = "");
};

namespace Interpolations {
    double linear( double val, double fromMin, double fromMax, double toMin = 0, double toMax = 100);
}

namespace ProcessUtils {
    QProcess *runProcess(QString binary, QStringList arguments = QStringList());
    void runProcess(QProcess *p, QString binary, QStringList arguments = QStringList());
    void runIndependantProcess(QString binary, QStringList arguments = QStringList());
}

#endif // UTILS_H
