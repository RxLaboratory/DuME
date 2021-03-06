#include "lutconverterwidget.h"

LutConverterWidget::LutConverterWidget(QWidget *parent) :
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

#ifdef Q_OS_MAC
    ocioNotFoundLabel->setText("OCIO's ociobakelut tool not found. Please set its path in the settings. You can install it with Homebrew: brew install opencolorio");
#endif

    connect( outputFormatBox, SIGNAL(currentIndexChanged(int)), this, SLOT(outputFormatChanged(int)));
    connect( lutBrowseButton, SIGNAL(clicked()), this, SLOT(inputBrowse()));
    connect( convertButton, SIGNAL(clicked()), this, SLOT(convert()));
    connect(OcioLutBakerInfo::instance(), SIGNAL(valid(bool)), this, SLOT(ocioLutBakerIsValid(bool)));

    ocioLutBakerIsValid( OcioLutBakerInfo::instance()->isValid() );
}

void LutConverterWidget::outputFormatChanged(int index)
{
    iccOptionsWidget->setVisible( outputFormatBox->itemData(index).toString() == "icc.icc" );
}

void LutConverterWidget::inputBrowse()
{
    QString filters = "LUT files (*.3dl *.ccc *.cc *.cdl *.clf *.ctf *.csp *.cub *.cube *.lut *.hdl *.icc *.icm *.pf *.look *.mga *.m3d *.spi1d *.spi3d *.spimtx *.vf";
#ifdef Q_OS_LINUX
    filters += " *.3DL *.CCC *.CC *.CDL *.CLF *.CTF *.CSP *.CUB *.CUBE *.LUT *.HDL *.ICC *.ICM *.PF *.LOOK *.MGA *.M3D *.SPI1D *.SPI3D *.SPIMTX *.VF";
#endif
    filters += ")";
    filters += ";;Discreet / After Effects / Autodesk (*.3dl *.3DL)";
    filters += ";;ASC CDL ColorCorrectionCollection (*.ccc *.CCC)";
    filters += ";;ASC CDL ColorCorrection (*.cc *.CC *.cdl *.CDL)";
    filters += ";;Academy/ASC Common LUT Format (*.clf *.CLF)";
    filters += ";;Autodesk Color Transform Format (*.ctf *.CTF)";
    filters += ";;Cinespace (*.csp *.CSP)";
    filters += ";;Truelight (*.cub *.CUB)";
    filters += ";;Iridas / Resolve (*.cube *.CUBE)";
    filters += ";;Discreet legacy 1D-LUT / Houdini (*.lut *.LUT)";
    filters += ";;Houdini (*.hdl *.hdl)";
    filters += ";;ICC profile format (*.icc *.icm *.pf *.ICC *.ICM *.PF)";
    filters += ";;Iridas Look (*.look *.LOOK)";
    filters += ";;Pandora (*.mga *.m3d *.MGA *.M3D)";
    filters += ";;Imageworks 1D (*.spi1d *.SPI1D)";
    filters += ";;Imageworks 3D (*.spi3d *.SPI3D)";
    filters += ";;Imageworks Matrix (*.spimtx *.SPIMTX)";
    filters += ";;Inventor (*.vf *.VF)";
    filters += ";;All Files (*.*)";
    QString path = QFileDialog::getOpenFileName(this,"Load LUT file", QDir::homePath(), filters);

    if (path == "") return;
    inputEdit->setText(path);
}

void LutConverterWidget::convert()
{
    QString filters = outputFormatBox->currentText();
    filters += ";;All Files (*.*)";
    QString outputPath = QFileDialog::getSaveFileName(this,"Save LUT as...", "", filters);
    if (outputPath == "") return;

    QStringList arguments;
    QString inputPath = inputEdit->text();
    if (!QFileInfo::exists(inputPath)) return;

    QString l = "--lut";
    if (invertBox->isChecked()) l = "--invlut";

    arguments << l << inputPath;

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

void LutConverterWidget::ocioLutBakerIsValid(bool valid)
{
    this->setEnabled(valid);
    ocioNotFoundLabel->setVisible(!valid);
}
