#ifndef AERENDEROBJECT_H
#define AERENDEROBJECT_H

#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QStandardPaths>

class AERenderObject : public QObject
{
    Q_OBJECT
public:
    AERenderObject(QString path = "", QObject *parent = nullptr);

    QString name() const;
    QString path() const;
    int mainVersion() const;
    int primaryVersion() const;
    int secondaryVersion() const;
    int buildNumber() const;
    bool isValid() const;
    QString dataPath() const;

signals:

public slots:
    void init();
    bool setDuFFmpegTemplates();
    void restoreOriginalTemplates();

private slots:
    void errorOccurredAE(QProcess::ProcessError e);
    void stdOutputAE();
    void stdErrorAE();

private:
    QString _name;
    QString _path;
    int _mainVersion;
    int _primaryVersion;
    int _secondaryVersion;
    int _buildNumber;
    QString _dataPath;

    bool _isValid;

    QProcess *_aerender;
    QString _aeOutput;

    void findDataPath();
};

#endif // AERENDEROBJECT_H
