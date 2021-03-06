#include "ociosettingswidget.h"

OcioSettingsWidget::OcioSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    //needed for macos
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    connect(ociobakelutBrowseButton, &QToolButton::clicked, this, &OcioSettingsWidget::ociobakelutBrowseButton_clicked);
    connect(ociobakelutEdit, &QLineEdit::editingFinished, this, &OcioSettingsWidget::ociobakelutEdit_editingFinished);
    connect(OcioLutBakerInfo::instance(), &AbstractRendererInfo::binaryChanged, this, &OcioSettingsWidget::ociobakelut_binaryChanged);

    ociobakelut_binaryChanged();
}

void OcioSettingsWidget::ociobakelut_binaryChanged()
{
    QSignalBlocker b(ociobakelutEdit);
    ociobakelutEdit->setText(OcioLutBakerInfo::instance()->binary());
}

void OcioSettingsWidget::ociobakelutBrowseButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"Select the ociobakelut executable binary");
    if (path == "") return;
    ociobakelutEdit->setText(path);
    QSignalBlocker b(ociobakelutEdit);
    OcioLutBakerInfo::instance()->setBinary( path );
}

void OcioSettingsWidget::ociobakelutEdit_editingFinished()
{
    OcioLutBakerInfo::instance()->setBinary( ociobakelutEdit->text() );
}
