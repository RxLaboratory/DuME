#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QProcess>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QScrollBar>

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
private slots:
    // FFMPEG
    void ffmpeg_stdError();
    void ffmpeg_stdOutput();
    void ffmpeg_readyRead(QString ffmpegOutput);
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
    void on_actionGo_triggered();
    void on_actionStop_triggered();

private:
    bool isReady();

    // ====== UI ========
    /**
     * @brief statusLabel The status shown in the status bar
     */
    QLabel *statusLabel;

    // ===== FFMPEG ======
    /**
     * @brief ffmpegPath The path to the ffmpeg executable
     */
    QString ffmpegPath;
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
     */
    int ffmpegRunningType;
};

#endif // MAINWINDOW_H
