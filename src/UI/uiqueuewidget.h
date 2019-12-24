#ifndef QUEUEWIDGET_H
#define QUEUEWIDGET_H

#include "ui_queuewidget.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "uiinputwidget.h"
#include "uioutputwidget.h"

class UIQueueWidget : public QWidget, private Ui::QueueWidget
{
    Q_OBJECT

public:
    explicit UIQueueWidget(FFmpeg *ffmpeg, QWidget *parent = nullptr);
    MediaInfo *getInputMedia();
    QList<MediaInfo *> getOutputMedia();
    void addInputFile(QString file);
    void addInputFile(QUrl file);

signals:
    /**
     * @brief console general messages to be displayed in the UI by MainWindow
     */
    void newLog( QString, LogUtils::LogType lt = LogUtils::Information );

public slots:
    void presetsPathChanged();

private slots:
    void on_outputTab_tabCloseRequested(int index);
    void on_outputTab_tabBarClicked(int index);
    void log(QString log, LogUtils::LogType lt = LogUtils::Information );

private:
    QSettings _settings;
    FFmpeg *_ffmpeg;
    QList<UIOutputWidget*> outputWidgets;
    QList<UIInputWidget*> inputWidgets;

    void addOutput();

};

#endif // QUEUEWIDGET_H
