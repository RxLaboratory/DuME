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
#include <QMimeData>
#include <QDebug>
#include <QDesktopServices>

#include "FFmpeg/ffmpeg.h"
#include "AfterEffects/aftereffects.h"
#include "Renderer/renderqueue.h"
#include "utils.h"

#include "toolbarspacer.h"
#include "settingswidget.h"
#include "queuewidget.h"
#include "rainboxui.h"
#include "version.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int argc, char *argv[], FFmpeg *ff, QWidget *parent = nullptr);

public slots:

    void maximize(bool max);
    void maximize();

private slots:
    // FFMPEG
    void ffmpegLog(QString l, LogUtils::LogType lt = LogUtils::Information);
    void ffmpegConsole( QString c);
    void ffmpegValid(bool valid);
    void ffmpegStatus(MediaUtils::RenderStatus status);

    // AE
    void aeLog(QString l, LogUtils::LogType lt = LogUtils::Information);
    void aeConsole( QString c);

    // Queue
    void progress();
    void renderQueueStatusChanged(MediaUtils::RenderStatus status);

    // CONSOLE
    void log(QString log,LogUtils::LogType type = LogUtils::Information);

    // UI EVENTS
    void on_ffmpegCommandsEdit_returnPressed();
    void on_ffmpegCommandsButton_clicked();

    // ACTIONS
    void on_actionGo_triggered();
    void on_actionStop_triggered();
    void on_actionSettings_triggered(bool checked);
    void on_actionBug_report_triggered();
    void on_actionChat_triggered();

    void on_actionForum_triggered();

    void on_actionHelp_triggered();

private:
    /**
     * @brief go Launches the transcoding process
     */
    void go();
    /**
     * @brief settings The application settings
     */
    QSettings *settings;

    // ====== UI ========

    QueueWidget *queueWidget;
    /**
     * @brief reInitCurrentProgress Initializes the current progress bar and infos
     */
    void reInitCurrentProgress();
    /**
     * @brief statusLabel The status shown in the status bar
     */
    QLabel *statusLabel;
    // Window Buttons
    QToolButton *quitButton;
    QToolButton *maximizeButton;
    QToolButton *minimizeButton;
    QMenu *helpMenu;
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
     * @brief ffmpeg The FFmpeg information and utils
     */
    FFmpeg *_ffmpeg;

    // ===== AE =====
    /**
     * @brief ae The After Effects information and utils
     */
    AfterEffects *_ae;

    // ===== RENDERQUEUE =====
    /**
     * @brief _renderQueue The render queue
     */
    RenderQueue *_renderQueue;

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
};

#endif // MAINWINDOW_H
