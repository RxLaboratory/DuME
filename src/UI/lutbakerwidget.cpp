#include "lutbakerwidget.h"

LutBakerWidget::LutBakerWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    iccOptionsWidget->setVisible(false);

    outputFormatBox->setItemData(0, "flame.3dl");
    outputFormatBox->setItemData(1, "lustre.3dl");
    outputFormatBox->setItemData(2, "cinespace.csp");
    outputFormatBox->setItemData(3, "houdini.lut");
    outputFormatBox->setItemData(4, "iridas_itx.itx");
    outputFormatBox->setItemData(5, "truelight.cub");
    outputFormatBox->setItemData(6, "icc.icc");

    ocioConfigEdit->setText( _settings.value("ociobakelut/configPath", "").toString() );

#ifdef Q_OS_MAC
    ocioNotFoundLabel->setText("OCIO's ociobakelut tool not found. Please set its path in the settings. You can install it with Homebrew: brew install opencolorio");
#endif

    connect( outputFormatBox, SIGNAL(currentIndexChanged(int)), this, SLOT(outputFormatChanged(int)));
    connect( ocioBrowseButton, SIGNAL(clicked()), this, SLOT(ocioBrowse()));
    connect( bakeButton, SIGNAL(clicked()), this, SLOT(bake()));
    connect(OcioLutBakerInfo::instance(), SIGNAL(valid(bool)), this, SLOT(ocioLutBakerIsValid(bool)));

    ocioLutBakerIsValid( OcioLutBakerInfo::instance()->isValid() );
}

void LutBakerWidget::outputFormatChanged(int index)
{
    iccOptionsWidget->setVisible( outputFormatBox->itemData(index).toString() == "icc.icc" );
}

void LutBakerWidget::ocioBrowse()
{
    QString path = QFileDialog::getOpenFileName(this,"Select the ocio.config file.", "", "OCIO (*.ocio *.OCIO);;All Files (*.*)");
    if (path == "") return;
    ocioConfigEdit->setText(path);
    _settings.setValue("ociobakelut/configPath", path);
}

void LutBakerWidget::bake()
{
    QString filters = outputFormatBox->currentText();
    filters += ";;All Files (*.*)";
    QString outputPath = QFileDialog::getSaveFileName(this,"Save LUT as...", "", filters);
    if (outputPath == "") return;

    QStringList arguments;
    QString ocioConfig = ocioConfigEdit->text();
    if (ocioConfig != "" && ocioConfig != "$OCIO") arguments << "--iconfig" << ocioConfig;
    arguments << "--inputspace" << inputEdit->text();
    arguments << "--outputspace" << outputEdit->text();
    if (shaperEdit->text() != "") arguments << "--shaperspace" << shaperEdit->text();
    if (looksEdit->text() != "") arguments << "--looks" << looksEdit->text();

    QStringList outputFormat = outputFormatBox->currentData().toString().split(".");
    QString format = outputFormat[0];
    arguments << "--format" << format;

    if (format == "icc")
    {
        arguments << "--whitepoint" << iccWhitePointBox->text();
        if (iccProfileEdit->text() != "") arguments << "--displayicc" << iccProfileEdit->text();
        if (iccDescriptionEdit->text() != "") arguments << "--displayicc" << iccDescriptionEdit->text();
        if (iccCopyrightEdit->text() != "") arguments << "--copyright" << iccCopyrightEdit->text();
    }

    QString ext = outputFormat[1];
    if (!outputPath.endsWith(ext)) outputPath += "." + ext;
    arguments << outputPath;

    ProcessUtils::runIndependantProcess(OcioLutBakerInfo::instance()->binary(), arguments);
}

void LutBakerWidget::ocioLutBakerIsValid(bool valid)
{
    this->setEnabled(valid);
    ocioNotFoundLabel->setVisible(!valid);
}
