#ifndef AERENDER_H
#define AERENDER_H

#include <QObject>
#include <QDir>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QProcess>
#include <algorithm>
#include <QSettings>
#include <QThread>

#include "aerenderprocess.h"

/**
 * @brief The AERenderer class handles After Effects rendering, with all versions of After Effects available on the system.
 */
class AERenderer : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief AERenderer Constructs the After Effects renderer
     * @param parent
     */
    AERenderer(QObject *parent = nullptr);
    /**
     * @brief versions All the After Effects renderers available on this system
     * @return The processes for each available version of After Effects
     */
    QList<AERenderProcess *> versions() const;
    /**
     * @brief setCurrentAERenderProcess Sets the process (Ae version) to use, based on the settings of the application
     */
    void setCurrentAERenderProcess();
    /**
     * @brief setCurrentAERenderProcess Sets the process (Ae version) to use
     * @param name The name of the process to use.
     */
    void setCurrentAERenderProcess(QString name);

    void start(QStringList arguments, QStringList audioArguments = QStringList(), int numThreads = 1, bool useQueue = false);

signals:
    /**
     * @brief newOutput Emitteed when FFmpeg outputs on stderr or stdoutput
     */
    void newOutput(QString);
    /**
     * @brief newError Emitted when a blocking error occurs. Contains the description of the error.
     */
    void newError(QString);
    /**
     * @brief newLog Emitted when some debug infos are available
     */
    void newLog(QString);
    /**
     * @brief finished Emitted when the rendering process is finished
     */
    void finished();

public slots:


private slots:
    // Initializes the renderer, gets all available versions on the system
    void init();
    // A process has finished rendering
    void threadFinished();

private:

    // === ATTRIBUTES ===

    // The list of After Effects renderer found in the system
    QList<AERenderProcess *> _versions;
    // The version currently used
    AERenderProcess *currentAERenderProcess;
    // The processes currently rendering
    QList<QProcess *> _renderProcesses;

    // === METHODS ===

    // Finds all versions of After Effects installed on the system
    void findAeVersions(QString dir);
    // launch an Ae render process
    void launchRenderProcess(QStringList args);
};

#endif // AERENDER_H
