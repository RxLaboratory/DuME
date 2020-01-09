#ifndef STREAMMAP_H
#define STREAMMAP_H


class StreamReference
{
public:
    StreamReference(int inputMediaId = 0, int streamId = -1);

    int mediaId() const;
    void setMediaId(int mediaId);

    int streamId() const;
    void setStreamId(int streamId);

private:
    int _mediaId;
    int _streamId;
};

#endif // STREAMMAP_H
