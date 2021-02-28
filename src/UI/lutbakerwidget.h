#ifndef LUTBAKERWIDGET_H
#define LUTBAKERWIDGET_H

#include "ui_lutbakerwidget.h"

#include <QFileDialog>
#include <QProcess>
#include <QSettings>

#include "OCIO/ociolutbakerinfo.h"

class LutBakerWidget : public QWidget, private Ui::LutBakerWidget
{
    Q_OBJECT

public:
    explicit LutBakerWidget(QWidget *parent = nullptr);

private slots:
    void outputFormatChanged(int index);
    void ocioBrowse();
    void bake();
    void ocioLutBakerIsValid(bool valid);

private:
    QSettings _settings;
};

#endif // LUTBAKERWIDGET_H
