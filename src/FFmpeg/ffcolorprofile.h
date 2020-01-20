#ifndef FFCOLORPROFILE_H
#define FFCOLORPROFILE_H

#include <QObject>

#include "FFmpeg/ffbaseobject.h"


class FFColorProfile : public FFBaseObject
{
    Q_OBJECT
public:
    FFColorProfile(QString name, QString prettyName, FFBaseObject *primaries, FFBaseObject *trc, FFBaseObject *space, FFBaseObject *range, QObject *parent = nullptr);
    FFBaseObject *primaries() const;
    FFBaseObject *trc() const;
    FFBaseObject *space() const;
    FFBaseObject *range() const;
private:
    FFBaseObject *_primaries;
    FFBaseObject *_trc;
    FFBaseObject *_space;
    FFBaseObject *_range;
};

#endif // FFCOLORPROFILE_H
