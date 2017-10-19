#ifndef FFMUXER_H
#define FFMUXER_H

#include <QObject>

#include "ffcodec.h"

class FFMuxer : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief The muxer type
     */
    enum Type { AudioOnly, VideoOnly, AudioVideo, Sequence};
    Q_ENUM(Type)

    explicit FFMuxer(QObject *parent = nullptr);
    explicit FFMuxer(QString name, QObject *parent = nullptr);
    explicit FFMuxer(QString name, QString prettyName,QObject *parent = nullptr);
    explicit FFMuxer(QString name, QString prettyName,Type type, QObject *parent = nullptr);

    /**
     * @brief defaultVideoCodec Gets the default video codec for this muxer
     * @return The default codec or nullptr if this muxer does not have a default codec
     */
    FFCodec * defaultVideoCodec() const;
    void setDefaultVideoCodec(FFCodec *defaultVideoCodec);

    /**
     * @brief defaultVideoCodec Gets the default audio codec for this muxer
     * @return The default codec or nullptr if this muxer does not have a default codec
     */
    FFCodec * defaultAudioCodec() const;
    void setDefaultAudioCodec(FFCodec *defaultAudioCodec);

    QString name() const;
    void setName(const QString &name);

    QString prettyName() const;
    void setPrettyName(const QString &prettyName);

    bool isAudio();
    bool isVideo();
    bool isSequence();
    void setType(const Type &type);

    QStringList extensions() const;
    void setExtensions(const QStringList &extensions);

signals:

public slots:

private:
    FFCodec *_defaultVideoCodec;
    FFCodec *_defaultAudioCodec;
    QString _name;
    QString _prettyName;
    Type _type;
    QStringList _extensions;

    void checkType();
};

#endif // FFMUXER_H
