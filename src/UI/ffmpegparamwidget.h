#ifndef FFMPEGPARAMWIDGET_H
#define FFMPEGPARAMWIDGET_H

#include "ui_ffmpegparamwidget.h"

class FFmpegParamWidget : public QWidget, private Ui::FFmpegParamWidget
{
    Q_OBJECT

public:
    explicit FFmpegParamWidget(QWidget *parent = nullptr);
    QString param();
    QString value();
    void setParam(QString p);
    void setValue(QString v);
signals:
    void changed();

};

#endif // FFMPEGPARAMWIDGET_H
