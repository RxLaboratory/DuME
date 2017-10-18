#ifndef FFMUXER_H
#define FFMUXER_H

#include <QObject>

class FFMuxer : public QObject
{
    Q_OBJECT
public:
    explicit FFMuxer(QObject *parent = nullptr);
    explicit FFMuxer(QString name, QObject *parent = nullptr);
    explicit FFMuxer(QString name, QString prettyName,QObject *parent = nullptr);

    QString defaultVideoCodec() const;
    void setDefaultVideoCodec(const QString &defaultVideoCodec);

    QString defaultAudioCodec() const;
    void setDefaultAudioCodec(const QString &defaultAudioCodec);

    QString name() const;
    void setName(const QString &name);

    QString prettyName() const;
    void setPrettyName(const QString &prettyName);

signals:

public slots:

private:
    QString _defaultVideoCodec;
    QString _defaultAudioCodec;
    QString _name;
    QString _prettyName;
};

#endif // FFMUXER_H
