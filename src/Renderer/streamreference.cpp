#include "streamreference.h"

StreamReference::StreamReference( int inputMediaId, int streamId )
{
    _mediaId = inputMediaId;
    _streamId = streamId;
}

int StreamReference::mediaId() const
{
    return _mediaId;
}

void StreamReference::setMediaId(int mediaId)
{
    _mediaId = mediaId;
}

int StreamReference::streamId() const
{
    return _streamId;
}

void StreamReference::setStreamId(int streamId)
{
    _streamId = streamId;
}
