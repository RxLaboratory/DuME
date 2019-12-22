#ifndef ABSTRACTRENDERER_H
#define ABSTRACTRENDERER_H

#include <QObject>
#include <QTime>
#include <QProcess>
#include <QTimer>

/**
 * @brief The AbstractRenderer class is the base class for all renderers: ffmpeg, after effects, blender...
 */
class AbstractRenderer : public QObject
{
    Q_OBJECT
public:
    explicit AbstractRenderer(QObject *parent = nullptr);

    // RENDERING PROGRESS INFO
    /**
     * @brief currentFrame The frame being rendered
     * @return
     */
    int currentFrame() const;
    /**
     * @brief startTime The start time of the rendering process
     * @return
     */
    QTime startTime() const;
    /**
     * @brief outputSize The expected output file size
     * @return
     */
    double outputSize() const;
    /**
     * @brief outputBitrate The output bitrate
     * @return
     */
    int outputBitrate() const;
    /**
     * @brief encodingSpeed The rendering speed, compared to the media duration
     * @return
     */
    double encodingSpeed() const;
    /**
     * @brief timeRemaining The expected remaining time to finish the rendering
     * @return
     */
    QTime timeRemaining() const;

    // OUTPUT FILE INFO
    /**
     * @brief outputFileName The output file name.
     * @return
     */
    QString outputFileName() const;
    /**
     * @brief setOutputFileName Sets the output file name.
     * @param outputFileName
     */
    void setOutputFileName(const QString &outputFileName);

    // MANAGE THE RENDERING PROCESS
    /**
     * @brief setStopCommand Sets the command to send to the renderer to stop it, if any.
     * @param stopCommand
     * @default ""
     */
    void setStopCommand(const QString &stopCommand);
    /**
     * @brief setPauseCommand Sets the command to send to the renderer to pause it, if any.
     * @param pauseCommand
     */
    void setPauseCommand(const QString &pauseCommand);
    /**
     * @brief start Starts a rendering process
     * @param arguments The arguments to pass to the renderer
     */
    void start( QStringList arguments );
    /**
     * @brief stop Stops the current process(es)
     * @param timeout Kills the process after timeout if it does not respond to the stop commands. In milliseconds.
     */
    void stop(int timeout = 10000);
    /**
     * @brief pause Pauses the process, if it is possible
     */
    void pause();
    /**
     * @brief numThreads The number of threads to use when launching the rendering process
     * @return
     */
    int numThreads() const;
    /**
     * @brief setNumThreads Sets the number of threads to use
     * @param numThreads
     */
    void setNumThreads(int numThreads);

signals:
    /**
     * @brief newError Emitted when a blocking error occurs. Contains the description of the error.
     */
    void newError(QString);
    /**
     * @brief newLog Emitted when some debug infos are available
     */
    void newLog(QString);
    /**
     * @brief progress Emitted each time the transcoding process outputs new stats
     */
    void progress();

private slots:
    // gets the output from the render process(es)
    void stdError();
    void stdOutput();
    void started();
    void finished();
    void errorOccurred(QProcess::ProcessError e);
    // kills all render processes
    void killRenderProcesses();

private:
    // The process(es)
    QList<QProcess *> _renderProcesses;
    QString _binaryFileName;

    // RENDERING PROCESS

    // the output fileName
    QString _outputFileName;
    // the output folder
    QString _outputPath;
    // the current frame being renderered.
    int _currentFrame;
    // the starttime of the rendering process
    QTime _startTime;
    // the output size of the rendered file
    double _outputSize;
    // the output bitrate of the renderered file
    int _outputBitrate;
    // the speed of the encoding compared to the speed of the video
    double _encodingSpeed;
    // the time remaining before rendering completion
    QTime _timeRemaining;

    // CONFIGURATION

    QString _stopCommand;
    QString _pauseCommand;
    int _numThreads;

    // METHODS

    //updates the current frame and computes remaining time, etc
    void setCurrentFrame(int currentFrame);
    //Called when the process outputs something on stdError or stdOutput
    void readyRead(QString output);
    //Launches a new process
    void launchProcess(QStringList arguments );
};

#endif // ABSTRACTRENDERER_H
