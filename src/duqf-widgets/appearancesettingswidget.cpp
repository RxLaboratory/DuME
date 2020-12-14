#include "appearancesettingswidget.h"

AppearanceSettingsWidget::AppearanceSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _freezeUI = true;

    // List available styles
    styleComboBox->addItem("Default",":/styles/default");
    styleComboBox->addItems(QStyleFactory::keys());
    styleComboBox->addItem("Custom...","");

    // Load settings
    settings.beginGroup("appearance");

    int toolButtonStyle = settings.value("toolButtonStyle", 2).toInt();
    toolsComboBox->setCurrentIndex(toolButtonStyle);

    QString style = settings.value("style", "Default").toString();
    styleComboBox->setCurrentText(style);

    QString font = settings.value("font", "Ubuntu").toString();
    fontComboBox->setCurrentFont(QFont(font));

    _freezeUI = false;
}

void AppearanceSettingsWidget::on_toolsComboBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;

    Qt::ToolButtonStyle style = Qt::ToolButtonTextUnderIcon;
    if (index == 0) style = Qt::ToolButtonIconOnly;
    else if (index == 1) style = Qt::ToolButtonTextOnly;
    else if (index == 2) style = Qt::ToolButtonTextUnderIcon;
    else if (index == 3) style = Qt::ToolButtonTextBesideIcon;

    settings.setValue("toolButtonStyle", index);

    DuUI::setToolButtonStyle(style);
}


void AppearanceSettingsWidget::on_styleComboBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;

    QString cssFile = ":/styles/default";
    QString style = "Default";

    if(index == styleComboBox->count() - 1)
    {
        QString cssFileName = QFileDialog::getOpenFileName(this,"Open a stylesheet file","","CSS (*.css);;Text files (*.txt);;All files (*.*)");
        QFile checkFile(cssFileName);
        if (checkFile.exists())
        {
            cssFile = cssFileName;
        }
        else cssFile = settings.value("cssFile",cssFile).toString();
    }
    else
    {
        style = styleComboBox->currentText();
        cssFile = styleComboBox->currentData().toString();
    }

    settings.setValue("style", style);
    settings.setValue("cssFile", cssFile);

    if (cssFile != "")
    {
        DuUI::updateCSS(cssFile);
    }
    else
    {
        DuUI::updateCSS("");
        qApp->setStyle(QStyleFactory::create(style));
    }
}

void AppearanceSettingsWidget::on_fontComboBox_currentFontChanged(const QFont &f)
{
    if (_freezeUI) return;

    settings.setValue("font", f.family());

    qApp->setFont(f);
    DuUI::updateCSS(settings.value("cssFile",":/styles/default").toString());
}
