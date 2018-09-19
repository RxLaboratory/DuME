#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QFile>


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

#endif // UTILS_H
