#include "ffsampleformat.h"

FFSampleFormat::FFSampleFormat(QString name, QString prettyName, int depth, QObject *parent):
    FFBaseObject(name, prettyName, parent)
{
    _depth = depth;
}

FFSampleFormat::FFSampleFormat(QString name, int depth, QObject *parent):
    FFBaseObject(name, parent)
{
    _depth = depth;
}


int FFSampleFormat::depth()
{
    return _depth;
}

FFSampleFormat *FFSampleFormat::getDefault(QObject *parent)
{
    return new FFSampleFormat( "", "Default", 0, parent );
}
