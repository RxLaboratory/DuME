#include "ffcolorprofile.h"

FFColorProfile::FFColorProfile(QString name, QString prettyName, FFBaseObject *primaries, FFBaseObject *trc, FFBaseObject *space, FFBaseObject *range, QObject *parent):
    FFBaseObject(name, prettyName, parent)
{
    _primaries = primaries;
    _trc = trc;
    _space = space;
    _range = range;
}

FFBaseObject *FFColorProfile::primaries() const
{
    return _primaries;
}

FFBaseObject *FFColorProfile::trc() const
{
    return _trc;
}

FFBaseObject *FFColorProfile::space() const
{
    return _space;
}

FFBaseObject *FFColorProfile::range() const
{
    return _range;
}
