#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QProcess>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QScrollBar>
#include <QMouseEvent>
#include <QSettings>

#include "toolbarspacer.h"
#include "mediainfo.h"
#include "settingswidget.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
private slots:
    // FFMPEG
    void ffmpeg_stdError();
    void ffmpeg_stdOutput();
    void ffmpeg_readyRead(QString output);
    void ffmpeg_errorOccurred(QProcess::ProcessError e);
    void ffmpeg_started();
    void ffmpeg_finished();

    // CONSOLE
    void console(QString log);

    // UI
    void on_videoTranscodeButton_toggled(bool checked);
    void on_audioTranscodeButton_toggled(bool checked);
    void on_resizeButton_toggled(bool checked);
    void on_frameRateButton_toggled(bool checked);
    void on_samplingButton_toggled(bool checked);
    void on_inputBrowseButton_clicked();
    void on_outputBrowseButton_clicked();
    void on_inputEdit_textChanged(const QString &arg1);
    void on_outputEdit_textChanged(const QString &arg1);
    void on_frameRateBox_activated(const QString &arg1);
    void on_frameRateEdit_valueChanged(double arg1);
    void on_videoQualitySlider_valueChanged(int value);
    void on_videoQualitySlider_sliderReleased();
    void on_videoBitRateEdit_valueChanged(double arg1);
    void on_audioQualitySlider_sliderReleased();
    void on_audioQualitySlider_valueChanged(int value);
    void on_audioBitRateEdit_valueChanged(int arg1);
    void on_videoWidthButton_valueChanged();
    void on_videoHeightButton_valueChanged();
    //actions
    void on_actionGo_triggered();
    void on_actionStop_triggered();
    void on_actionSettings_triggered(bool checked);

    void maximize();


private:
    bool isReady();
    void aspectRatio();

    /**
     * @brief settings The application settings
     */
    QSettings *settings;
    /**
     * @brief displayMediaInfo Displays input media infos and populates input widgets
     * @param info The info to display
     */
    void displayMediaInfo(MediaInfo *info);

    // ====== UI ========

    void updateCSS(QString cssFileName);
    /**
     * @brief statusLabel The status shown in the status bar
     */
    QLabel *statusLabel;

#ifndef Q_OS_MAC
    /**
     * @brief maximizeButton Button to maximize the window
     */
    QPushButton *maximizeButton;
    /**
     * @brief minimizeButton Button to minimize the window
     */
    QPushButton *minimizeButton;
#endif
    /**
     * @brief quitButton Button to quit application
     */
    QPushButton *quitButton;
    /**
     * @brief settingsWidget The settings page
     */
    SettingsWidget *settingsWidget;
    /**
     * @brief Is the tool bar currently clicked or not
     */
    bool toolBarClicked;
    /**
     * @brief Drag position
     * Used for drag n drop feature
     */
    QPoint dragPosition;

    // ===== FFMPEG ======
    /**
     * @brief ffmpeg The ffmpeg process
     */
    QProcess *ffmpeg;
    /**
     * @brief ffmpegRunningType This value tells what the ffmpeg process is currently doing
     * -1: stopped
     * 0: transcoding
     * 1: gettings codecs
     * 2: getting help
     * 3: stopping
     * 4: getting input media infos
     */
    int ffmpegRunningType;
    /**
     * @brief ffmpegOutput The complete output of the latest ffmpeg process until it has finished
     */
    QString ffmpegOutput;
    /**
     * @brief generateArguments Generate FFmpeg arguments to launch the transcoding process
     * @param pass The number of the pass to launch
     * @return The list of arguments to pass to the ffmpeg QProcess
     */
    QStringList generateArguments(int pass);
    /**
     * @brief ffmpeg_gotMediaInfo Gets the info of the current media being processed by FFmpeg
     * @return The media info
     */
    MediaInfo *ffmpeg_gotMediaInfo();
    /**
     * @brief gotCodecs Builds the list of codecs and populates the output widgets
     * Called in ffmpeg_finished() when ffmpegRunningType is 1
     */
    void ffmpeg_gotCodecs();


protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MAINWINDOW_H
