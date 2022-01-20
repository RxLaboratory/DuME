#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include "AfterEffects/aerenderer.h"
#include "FFmpeg/ffmpegrenderer.h"
#include "FFmpeg/ffmpeg.h"

class ConsoleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConsoleWidget(QWidget *parent = nullptr);
public slots:
    void log(QString log, DuQFLog::LogType type);
    void ffmpegConsole(QString c);
    void ffmpegLog(QString l, DuQFLog::LogType lt);
    void aeLog(QString l, DuQFLog::LogType lt);
    void aeConsole(QString c);
    void ffmpegValid(bool valid);
private slots:
    void runFFmpegCommand();
    void runAeCommand();
private:
    void setupUi();
    void connectEvents();

    QTextEdit *ui_debugEdit;
    QTextBrowser *ui_ffmpegEdit;
    QLineEdit *ui_ffmpegCommandsEdit;
    QPushButton *ui_ffmpegCommandsButton;
    QTextEdit *ui_helpEdit;
    QTextBrowser *ui_aeConsoleEdit;
    QLineEdit *ui_aeCommandsEdit;
    QPushButton *ui_aeCommandsButton;
};

#endif // CONSOLEWIDGET_H
