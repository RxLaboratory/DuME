#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include "ui_inputwidget.h"

#include <QFileDialog>
#include <QSettings>
#include <QThread>

#include "ffmpeg.h"

class InputWidget : public QWidget, private Ui::InputWidget
{
    Q_OBJECT

public:
    explicit InputWidget(FFmpeg *ff,QWidget *parent = nullptr);
    FFMediaInfo *getMediaInfo();
    void openFile(QString file);
    void openFile(QUrl file);

signals:
    void newMediaLoaded(FFMediaInfo *);

private slots:
    void on_inputBrowseButton_clicked();
    void on_addParamButton_clicked();
    void on_frameRateButton_toggled(bool checked);
    void on_frameRateBox_activated(const QString &arg1);
    void on_frameRateEdit_valueChanged(double arg1);
    void on_trcButton_toggled(bool checked);
    void on_trcBox_currentIndexChanged(int index);

    void on_inputEdit_editingFinished();

    void on_compButton_toggled(bool checked);

    void on_threadsButton_toggled(bool checked);

private:
    FFmpeg *ffmpeg;
    FFMediaInfo *_mediaInfo;
    QList<QLineEdit *> _customParamEdits;
    QList<QLineEdit *> _customValueEdits;
    void updateOptions();
};

#endif // INPUTWIDGET_H
