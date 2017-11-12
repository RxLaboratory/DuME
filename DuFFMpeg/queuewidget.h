#ifndef QUEUEWIDGET_H
#define QUEUEWIDGET_H

#include "ui_queuewidget.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "inputwidget.h"
#include "outputwidget.h"

class QueueWidget : public QWidget, private Ui::QueueWidget
{
    Q_OBJECT

public:
    explicit QueueWidget(FFmpeg *ff, QWidget *parent = 0);
    FFMediaInfo *getInputMedia();
    QList<FFMediaInfo *> getOutputMedia();

public slots:
    void presetsPathChanged(QString path);

private slots:
    void on_outputTab_tabCloseRequested(int index);
    void on_outputTab_tabBarClicked(int index);

private:
    FFmpeg *ffmpeg;
    QSettings _settings;

    QList<OutputWidget*> outputWidgets;
    QList<InputWidget*> inputWidgets;

    void addOutput();

};

#endif // QUEUEWIDGET_H
