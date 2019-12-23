#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QFile>
#include <QObject>


class FileUtils
{
public:
    FileUtils();
    static void setReadWrite(QString path);
    static void setReadWrite(QFile *file);
    static void move(QString from, QString to);
    static void copy(QString from, QString to);
    static void remove(QString path);
};

class MediaUtils
{
    Q_OBJECT

public:
    MediaUtils();
    virtual ~MediaUtils();

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
};

#endif // UTILS_H
