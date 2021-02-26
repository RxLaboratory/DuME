#ifndef OCIOSETTINGSWIDGET_H
#define OCIOSETTINGSWIDGET_H

#include "ui_ociosettingswidget.h"

#include <QSettings>
#include <QFileDialog>

#include "OCIO/ociolutbakerinfo.h"

class OcioSettingsWidget : public QWidget, private Ui::OcioSettingsWidget
{
    Q_OBJECT

public:
    explicit OcioSettingsWidget(QWidget *parent = nullptr);

private slots:
    void ociobakelut_binaryChanged();
    void ociobakelutBrowseButton_clicked();
    void ociobakelutEdit_editingFinished();

private:
    QSettings _settings;
};

#endif // OCIOSETTINGSWIDGET_H
