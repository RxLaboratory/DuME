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

    // CONSOLE
    void console(QString log);

    // UI
    void on_videoTranscodeButton_toggled(bool checked);
    void on_audioTranscodeButton_toggled(bool checked);
    void on_resizeButton_toggled(bool checked);
    void on_frameRateButton_toggled(bool checked);
    void on_samplingButton_toggled(bool checked);
    void on_channelsButton_toggled(bool checked);
    void on_inputBrowseButton_clicked();
    void on_outputBrowseButton_clicked();
    void on_inputEdit_textChanged(const QString &arg1);
    void on_outputEdit_textChanged(const QString &arg1);
    void on_actionGo_triggered();

    void on_frameRateBox_activated(const QString &arg1);

    void on_frameRateEdit_valueChanged(double arg1);

private:
    bool isReady();

    QString ffmpegPath;
    QProcess *ffmpeg;
};

#endif // MAINWINDOW_H
