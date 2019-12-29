#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QFile>
#include <QObject>


class FileUtils
{
public:
    FileUtils();
    static void setReadWrite(QString path)
    {
        QFile file(path);
        if (file.exists())
        {
            setReadWrite(&file);
        }
    }
    static void setReadWrite(QFile *file)
    {
        file->setPermissions(QFile::ReadOther | QFile::WriteOther | QFile::ReadUser | QFile::WriteUser | QFile::ReadOwner | QFile::WriteOwner);
    }
    static void move(QString from, QString to)
    {
        QFile origin(from);
        origin.rename(to);
        setReadWrite(&origin);
    }
    static void copy(QString from, QString to)
    {
        QFile origin(from);
        origin.copy(to);
        setReadWrite(to);
    }
    static void remove(QString path)
    {
        QFile file(path);
        setReadWrite(&file);
        file.remove();
    }
};

class MediaUtils
{
    Q_OBJECT

public:
    MediaUtils() {}
    virtual ~MediaUtils() {}

    /**
     * @brief The unit used for bitrates
     */
    enum BitrateUnit { bps, kbps, Mbps};
    Q_ENUM(BitrateUnit)

    /**
     * @brief The unit used for sizes
     */
    enum SizeUnit { Bytes, kB, MB, GB};
    Q_ENUM(SizeUnit)

    /**
     * @brief The Status enum Used to describe the current status of the renderer
     */
    enum Status { Initializing, Waiting, Launching, Encoding, FramesConversion, FFmpegEncoding, AERendering, BlenderRendering, Cleaning, Finished, Stopped, Error, Other };
    Q_ENUM(Status)

    /**
     * @brief isEncoding Checks if the current status is a busy (encoding or pre/post steps for encoding) status.
     * @param status
     * @return
     */
    static bool isBusy(MediaUtils::Status status)
    {
        if ( status == Launching ) return true;
        else if ( status == Encoding ) return true;
        else if ( status == FramesConversion ) return true;
        else if ( status == FFmpegEncoding ) return true;
        else if ( status == AERendering ) return true;
        else if ( status == BlenderRendering ) return true;
        else if ( status == Cleaning ) return true;

        return false;
    }

    /**
     * @brief statusString Converts the status as a human readable string to be used in the UI
     * @param status
     * @return
     */
    static QString statusString(MediaUtils::Status status)
    {
        if ( status == Initializing ) return "Initializing...";
        else if ( status == Waiting ) return "Ready";
        else if ( status == Launching ) return "Launching...";
        else if ( status == Encoding ) return "Running...";
        else if ( status == FramesConversion ) return "Conerting frames...";
        else if ( status == FFmpegEncoding ) return "FFmpeg encoding process...";
        else if ( status == AERendering ) return "After Effects rendering process...";
        else if ( status == BlenderRendering ) return "Blender rendering process...";
        else if ( status == Cleaning ) return "Cleaning...";
        else if ( status == Finished ) return "Process finished!";
        else if ( status == Stopped ) return "Process has been stopped.";
        else if ( status == Error ) return "An error has occured.";
        else return "Unknown status";
    }
    /**
     * @brief sizeString converts a size in bytes to a human-readable string
     * @param size
     * @return
     */
    static QString sizeString(qint64 size)
    {
        QString sizeText = "";

        if ( size < 1024*1024*10.0 )
        {
            size = size / 1024;
            sizeText = QString::number( int(size*100)/100) + " kB";
        }
        else if ( size < 1024*1024*1024*10.0 )
        {
            size = size / 1024 / 1024;
            sizeText = QString::number( int(size*100)/100) + " MB";
        }
        else
        {
            size = size / 1024 / 1024 / 1024;
            sizeText = QString::number( int(size*100)/100) + " GB";
        }
        return sizeText;
    }

    static QString bitrateString(qint64 bitrate)
    {
        QString sizeText = "";

        if ( bitrate < 1024*10.0 )
        {
            sizeText = QString::number( int(bitrate*100)/100) + " bps";
        }
        else if ( bitrate < 1024*1024*10.0 )
        {
            bitrate = bitrate / 1024;
            sizeText = QString::number( int(bitrate*100)/100) + " kbps";
        }
        else if ( bitrate < 1024*1024*1024*10.0 )
        {
            bitrate = bitrate / 1024 / 1024;
            sizeText = QString::number( int(bitrate*100)/100) + " Mbps";
        }
        else
        {
            bitrate = bitrate / 1024 / 1024 / 1024;
            sizeText = QString::number( int(bitrate*100)/100) + " Gbps";
        }
        return sizeText;
    }

    /**
     * @brief convertBitrate Converts a bitrate from bps to another unit.
     * @param to
     * @return
     */
    static double convertFromBps( qint64 value, BitrateUnit to )
    {
        if (to == kbps) return value / 1000;
        if (to == Mbps) return value / 1000000;
        return value;
    }

    /**
     * @brief convertBitrate Converts a file size from Bytes to another unit.
     * @param to
     * @return
     */
    static double convertFromBytes( qint64 value, SizeUnit to )
    {
        if (to == kB) return value / 1024;
        if (to == MB) return value / 1024 / 1024;
        if (to == GB) return value / 1024 / 1024 / 1024;
        return value;
    }

    /**
     * @brief convertBitrate Converts a bitrate to bps from another unit.
     * @param to
     * @return
     */
    static qint64 convertToBps( qint64 value, BitrateUnit from )
    {
        if (from == kbps) return value * 1000;
        if (from == Mbps) return value * 1000000;
        return value;
    }

    /**
     * @brief convertBitrate Converts a file size to Bytes from another unit.
     * @param to
     * @return
     */
    static qint64 convertToBytes( qint64 value, SizeUnit from )
    {
        if (from == kB) return value * 1024;
        if (from == MB) return value * 1024 * 1024;
        if (from == GB) return value * 1024 * 1024 * 1024;
        return value;
    }
};

class LogUtils
{
    Q_OBJECT

public:
    LogUtils() {}
    virtual ~LogUtils() {}

    /**
     * @brief The LogType enum Log level for printing the debug log
     */
    enum LogType { Debug, Information, Warning, Critical, Fatal };
    Q_ENUM(LogType)
};

#endif // UTILS_H
