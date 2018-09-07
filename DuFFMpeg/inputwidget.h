#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include "ui_inputwidget.h"

#include <QFileDialog>
#include <QSettings>

#include "ffmpeg.h"

class InputWidget : public QWidget, private Ui::InputWidget
{
    Q_OBJECT

public:
    explicit InputWidget(FFmpeg *ff,QWidget *parent = nullptr);
    FFMediaInfo *getMediaInfo();

signals:
    void newMediaLoaded(FFMediaInfo *);

private slots:
    void on_inputBrowseButton_clicked();
    void on_addParamButton_clicked();

    void on_frameRateButton_toggled(bool checked);
    void on_frameRateBox_activated(const QString &arg1);
    void on_frameRateEdit_valueChanged(double arg1);
private:
    FFmpeg *ffmpeg;
    FFMediaInfo *_mediaInfo;
    QList<QLineEdit *> _customParamEdits;
    QList<QLineEdit *> _customValueEdits;
    void updateOptions();
};

#endif // INPUTWIDGET_H
