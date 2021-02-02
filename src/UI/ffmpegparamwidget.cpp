#include "ffmpegparamwidget.h"

FFmpegParamWidget::FFmpegParamWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    connect(paramEdit, &QLineEdit::textEdited, this, &FFmpegParamWidget::changed);
    connect(valueEdit, &QLineEdit::textEdited, this, &FFmpegParamWidget::changed);
}

QString FFmpegParamWidget::param()
{
    return paramEdit->text();
}

QString FFmpegParamWidget::value()
{
    return valueEdit->text();
}

void FFmpegParamWidget::setParam(QString p)
{
    paramEdit->setText(p);
}

void FFmpegParamWidget::setValue(QString v)
{
    valueEdit->setText(v);
}

