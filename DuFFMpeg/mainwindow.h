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
#include "queuewidget.h"

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
    /**
     * @brief ffmpeg_init Set FFmpeg binary path (using settings) and get help
     */
    void ffmpeg_init();

    // UI EVENTS
    void on_ffmpegCommandsEdit_returnPressed();
    void on_ffmpegCommandsButton_clicked();

    // CONSOLE
    void console(QString log);

    // ACTIONS
    void on_actionGo_triggered();
    void on_actionStop_triggered();
    void on_actionSettings_triggered(bool checked);

    void maximize();

private:
    /**
     * @brief settings The application settings
     */
    QSettings *settings;

    // ====== UI ========

    QueueWidget *queueWidget;

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

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MAINWINDOW_H
