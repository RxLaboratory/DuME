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
    enum BitrateUnit { Bits, Kbps, Mbps};
    Q_ENUM(BitrateUnit)

    /**
     * @brief The unit used for sizes
     */
    enum SizeUnit { Bytes, KB, MB, GB};
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
