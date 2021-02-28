#ifndef LUTCONVERTERWIDGET_H
#define LUTCONVERTERWIDGET_H

#include "ui_lutconverterwidget.h"

#include <QFileDialog>
#include <QProcess>

#include "duqf-utils/utils.h"
#include "OCIO/ociolutbakerinfo.h"

class LutConverterWidget : public QWidget, private Ui::LutConverterWidget
{
    Q_OBJECT

public:
    explicit LutConverterWidget(QWidget *parent = nullptr);

private slots:
    void outputFormatChanged(int index);
    void inputBrowse();
    void convert();
    void ocioLutBakerIsValid(bool valid);
};

#endif // LUTCONVERTERWIDGET_H
