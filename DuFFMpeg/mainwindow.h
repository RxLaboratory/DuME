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
#include <QDateTime>

#include "toolbarspacer.h"
#include "settingswidget.h"
#include "ffmpeg.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
private slots:
    // FFMPEG
    void ffmpeg_errorOccurred(QString e);
    void ffmpeg_started(FFQueueItem *item);
    void ffmpeg_finished(FFQueueItem *item);
    void ffmpeg_statusChanged(FFmpeg::Status status);
    void ffmpeg_progress();

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

    //SETTINGS
    void changeFFmpegPath(QString path);


private:
    /**
     * @brief aspectRatio Computes the aspect ratio of a video
     */
    void aspectRatio();

    /**
     * @brief settings The application settings
     */
    QSettings *settings;
    /**
     * @brief displayMediaInfo Displays input media infos and populates input widgets
     * @param info The info to display
     */
    void displayMediaInfo(FFMediaInfo *info);

    // ====== UI ========

    void updateCSS(QString cssFileName);
    /**
     * @brief reInitCurrentProgress Initializes the current progress bar and infos
     */
    void reInitCurrentProgress();
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
     * @brief ffmpeg The ffmpeg bridge
     */
    FFmpeg *ffmpeg;
    /**
     * @brief initFFmpeg Set FFmpeg binary path (using settings) and get codec list and help
     */
    void ffmpeg_init();

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MAINWINDOW_H
