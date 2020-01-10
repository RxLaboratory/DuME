#ifndef FFPROFILE_H
#define FFPROFILE_H

#include <QObject>

#include "FFmpeg/ffbaseobject.h"

class FFProfile : public FFBaseObject
{
public:
    FFProfile(QString name, QString prettyName = "", QObject *parent = nullptr);
};

#endif // FFPROFILE_H
