#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "duqf-app/app-version.h"
#include "duqf-app/app-style.h"
#include "duqf-widgets/toolbarspacer.h"
#include "duqf-widgets/settingswidget.h"
#include "duqf-widgets/aboutdialog.h"
#include "duqf-widgets/appearancesettingswidget.h"
#include "duqf-widgets/duqfdocktitle.h"
#include "duqf-nodeview/duqfnodewidget.h"

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
#include <QMessageBox>

#include "AfterEffects/aftereffects.h"
#include "Renderer/renderqueue.h"
#include "Renderer/presetmanager.h"
#include "lutbakerwidget.h"
#include "lutconverterwidget.h"

#include "UI/ffmpegsettingswidget.h"
#include "UI/aesettingswidget.h"
#include "UI/cachesettingswidget.h"
#include "UI/ociosettingswidget.h"
#include "UI/queuewidget.h"
#include "UI/consolewidget.h"
#include "UI/rqueuewidget.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QStringList args, QWidget *parent = nullptr);

    void setPropertiesDockWidget(QWidget *w, QString title = "Properties", QString icon = ":/icons/property");

private slots:
    void duqf_bugReport();
    void duqf_forum();
    void duqf_chat();
    void duqf_doc();
    void duqf_settings(bool checked = true);
    void duqf_closeSettings();
    void duqf_reinitSettings();
    void duqf_about();

    /**
     * @brief go Launches the transcoding process
     */
    void go();

    // FFMPEG
    void ffmpegValid(bool valid);
    void ffmpegStatus(MediaUtils::RenderStatus status);

    // Queue
    void progress();
    void renderQueueStatusChanged(MediaUtils::RenderStatus status);

    // Cache
    void cacheSizeChanged(qint64 size);
    void openCacheDir();

    // LOGS
    void log(QString log,DuQFLog::LogType type = DuQFLog::Information);

    // Tools
    void closeTools();

    // ACTIONS
    void on_actionStop_triggered();
    void on_actionGoQuit_triggered();
    void on_actionAbout_FFmpeg_triggered();
    void on_actionTools_triggered(bool checked);

    // GENERAL
    void quit(bool force = false);

private:
    // ========= RxOT UI ==============
    /**
     * @brief initUi Called once to build the default RxOT UI
     */
    void duqf_initUi();
    /**
     * @brief duqf_setStyle Called once to set the UI Style after all ui have been created
     */
    void duqf_setStyle();
    /**
     * @brief Is the tool bar currently clicked or not
     */
    bool duqf_toolBarClicked;
    /**
     * @brief Drag position
     * Used for drag n drop feature
     */
    QPoint duqf_dragPosition;
    QToolButton *duqf_maximizeButton;
    QToolButton *duqf_settingsButton;
    AboutDialog *duqf_aboutDialog;
    bool duqf_dontSaveSettings = false;
    QSettings settings;
    SettingsWidget *settingsWidget;
    QLabel *title;
    QMenu *helpMenu;

    // ====== UI ========

    QToolButton *_cacheButton;
    QToolButton *ui_consoleButton;
    QueueWidget *queueWidget;
    /**
     * @brief reInitCurrentProgress Initializes the current progress bar and infos
     */
    void reInitCurrentProgress();
    /**
     * @brief statusLabel The status shown in the status bar
     */
    QLabel *statusLabel;
    /**
     * @brief goButton The button in the toolbar to launch the queue
     */
    QToolButton *goButton;

    FFmpegSettingsWidget *ffmpegSettingsWidget;
    AESettingsWidget *aeSettingsWidget;
    CacheSettingsWidget *cacheSettingsWidget;
    OcioSettingsWidget *ocioSettingsWidget;

    // Docks
    QDockWidget *ui_consoleDock;
    QDockWidget *ui_rQueueDock;
    QDockWidget *ui_propertiesDockWidget;
    DuQFDockTitle *ui_propertiesTitle;

    // ===== RENDERQUEUE =====
    /**
     * @brief _renderQueue The render queue
     */
    RenderQueue *renderQueue;
    bool autoQuit;

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
};

#endif // MAINWINDOW_H
