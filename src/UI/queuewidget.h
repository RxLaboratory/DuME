#ifndef QUEUEWIDGET_H
#define QUEUEWIDGET_H

#include "ui_queuewidget.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "inputwidget.h"
#include "outputwidget.h"
#include "Renderer/medialist.h"

class QueueWidget : public QWidget, private Ui::QueueWidget
{
    Q_OBJECT

public:
    explicit QueueWidget(FFmpeg *ffmpeg, QWidget *parent = nullptr);
    QList<MediaInfo *> getInputMedia();
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
    void on_inputTab_tabCloseRequested(int index);
    void on_inputTab_tabBarClicked(int index);
    void log(QString log, LogUtils::LogType lt = LogUtils::Information );

    void on_splitter_splitterMoved();

private:
    QSettings _settings;
    FFmpeg *_ffmpeg;
    QList<OutputWidget*> outputWidgets;
    QList<InputWidget*> inputWidgets;
    MediaList *_inputMedias;

    void addInput();
    void addOutput();
};

#endif // QUEUEWIDGET_H
