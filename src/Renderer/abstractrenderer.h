#ifndef ABSTRACTRENDERER_H
#define ABSTRACTRENDERER_H

#include <QObject>
#include <QTime>
#include <QProcess>
#include <QTimer>
#include <QElapsedTimer>
#include <QRegularExpression>
#include <QFileInfoList>
#include <QDir>

#include "utils.cpp"

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
     * @brief setCurrentFrame Sets the current frame being rendered and updates all progress info (size, time, bitrate...)
     * @param currentFrame
     */
    void setCurrentFrame(int currentFrame);
    /**
     * @brief startTime The start time of the rendering process
     * @return
     */
    QTime startTime() const;
    /**
     * @brief outputSize The expected output file size
     * @return
     */
    double outputSize( ) const;
    /**
     * @brief outputBitrate The output bitrate
     * @return
     */
    double outputBitrate() const;
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
    /**
     * @brief elapsedTime The elapsed time since the rendering process has started
     * @return
     */
    QTime elapsedTime() const;
    /**
     * @brief numFrames The total frame count of the output media. Note: this value may be update by the renderer during the process, and can be retrieved when it's finished.
     * @return
     */
    int numFrames() const;
    /**
     * @brief frameRate The frame rate of the output media. Note: this value may be update by the renderer during the process, and can be retrieved when it's finished.
     * @return
     */
    double frameRate() const;
    /**
     * @brief expectedSize The expected size of the output file
     * @return
     */
    double expectedSize() const;
    /**
     * @brief status The rendering status
     * @return
     */
    MediaUtils::Status status() const;

    // OUTPUT FILE INFO
    /**
     * @brief outputFileName The output file name.
     * @return
     */
    QString outputFileName() const;
    /**
     * @brief setNumFrames The number of frames to render
     * @param numFrames
     */
    void setNumFrames(int numFrames);
    /**
     * @brief setOutputFileName Sets the output file name.
     * @param outputFileName
     */
    void setOutputFileName(const QString &outputFileName);
    /**
     * @brief setFrameRate The framerate of the output video
     * @param frameRate
     */
    void setFrameRate(double frameRate);

    // MANAGE THE RENDERING PROCESS
    /**
     * @brief setStopCommand Sets the command to send to the renderer to stop it, if any.
     * @param stopCommand
     * @default ""
     */
    void setStopCommand(const QString &stopCommand);
    /**
     * @brief start Starts a rendering process
     * @param arguments The arguments to pass to the renderer
     */
    void start(QStringList arguments , int numThreads = 1);
    /**
     * @brief stop Stops the current process(es)
     * @param timeout Kills the process after timeout if it does not respond to the stop commands. In milliseconds.
     */
    void stop(int timeout = 10000);

signals:
    /**
     * @brief newLog Emitted when some debug infos are available
     */
    void newLog(QString, LogUtils::LogType lt = LogUtils::Information);
    /**
     * @brief statusChanged Emitted when the current status changed.
     */
    void statusChanged( MediaUtils::Status );
    /**
     * @brief progress Emitted regularly when the renderer is rendering, to tell there are new informations about the progress
     */
    void progress();
    void console( QString );

public slots:
    // CONFIGURE RENDERER
    void setBinary(const QString &binaryFileName);
    // Changes the current status
    void setStatus(MediaUtils::Status status);

protected:

    // RENDERING PROCESS

    // the output fileName
    QString _outputFileName;
    // the number of frames to render
    int _numFrames;
    // the framerate of the video
    double _frameRate;
    // the current frame being renderered.
    int _currentFrame;
    // the starttime of the rendering process
    QTime _startTime;
    // the current output size of the rendered file in Bytes
    double _outputSize;
    // the output bitrate of the renderered file in bps
    double _outputBitrate;
    // the expected output size
    double _expectedSize;
    // the speed of the encoding compared to the speed of the video
    double _encodingSpeed;
    // the time remaining before rendering completion
    QTime _timeRemaining;
    // the elapsed time
    QTime _elapsedTime;

    //Called when the process outputs something on stdError or stdOutput. Reimplement this method to interpret the output. It has to emit progress() at the end, and can use setCurrentFrame().
    virtual void readyRead(QString output);

private slots:
    // gets the output from the render process(es)
    void processStdError();
    void processStdOutput();
    void processStarted();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processErrorOccurred(QProcess::ProcessError e);
    // kills all render processes
    void killRenderProcesses();

private:
    // The process(es)
    QList<QProcess *> _renderProcesses;
    QString _binaryFileName;
    // The status
    MediaUtils::Status _status;
    // The output
    QString _output;
    // Process the outputs
    void processOutput(QString output);
    // A timer to process outputs only if a certain amount of time has passed
    QElapsedTimer _timer;

    // CONFIGURATION

    QString _stopCommand;

    // METHODS

    //Launches a new process
    void launchProcess(QStringList arguments );
};

#endif // ABSTRACTRENDERER_H
