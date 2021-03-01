#ifndef ABSTRACTRENDERERINFO_H
#define ABSTRACTRENDERERINFO_H

#include <QObject>
#include <QProcess>
#include <QRegularExpression>
#include <QCoreApplication>

#include "duqf-utils/utils.h"

class AbstractRendererInfo : public QObject
{
    Q_OBJECT
public:
    explicit AbstractRendererInfo(QObject *parent = nullptr);

    QString binary() const;

    QString lastErrorMessage() const;

    bool isValid() const;
signals:
    void newLog( QString, LogUtils::LogType lt = LogUtils::Information );
    void console( QString );
    void binaryChanged( QString binary );
    void valid(bool);
    void statusChanged(MediaUtils::RenderStatus);

public slots:
    bool setBinary( QString binary );
    /**
     * @brief runCommand Runs FFmpeg with the commands
     * @param commands The arguments, space separated. Use double quotes for any argument containing spaces
     */
    bool runCommand(QString commands, int timeout = 0, QIODevice::OpenModeFlag of = QIODevice::ReadOnly, bool ignoreErrors = false);
    /**
     * @brief runCommand Runs FFmpeg with the commands
     * @param commands The arguments
     */
    bool runCommand(QStringList commands, int timeout = 0, QIODevice::OpenModeFlag of = QIODevice::ReadOnly, bool ignoreErrors = false);

protected slots:
    void log( QString l, LogUtils::LogType lt = LogUtils::Information );
    //Binary signals
    void stdError();
    void stdOutput();
    void errorOccurred(QProcess::ProcessError e);

protected:
    // The process output
    QString _output;
    // The Status
    MediaUtils::RenderStatus _status;
    bool _valid;

private:
    QString _binary;
    // The process
    QProcess *_process;
    // The last error if any (used if the error happens before signals could be connected)
    QString _lastErrorMessage;

    void readyRead(QString output);

    bool _ignoreErrors;
};

#endif // ABSTRACTRENDERERINFO_H
