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
    explicit InputWidget(FFmpeg *ff,QWidget *parent = 0);
    FFMediaInfo *getMediaInfo();

signals:
    void newMediaLoaded(FFMediaInfo *);

private slots:
    void on_inputBrowseButton_clicked();
    void on_addParamButton_clicked();

private:
    FFmpeg *ffmpeg;
    FFMediaInfo *_mediaInfo;
    QList<QLineEdit *> _customParamEdits;
    QList<QLineEdit *> _customValueEdits;
};

#endif // INPUTWIDGET_H
