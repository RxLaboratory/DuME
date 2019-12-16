#include "settingswidget.h"
#include "uidropshadow.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

SettingsWidget::SettingsWidget(FFmpeg *ffmpeg,QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _ffmpeg = ffmpeg;
    _freezeUI = true;

    //Add nice shadows
    generalWidget->setGraphicsEffect(new UIDropShadow);
    aeWidget->setGraphicsEffect(new UIDropShadow);;

#ifdef Q_OS_LINUX
    QString defaultFfmpegPath = "ffmpeg";
#endif
#ifdef Q_OS_WIN
    QString defaultFfmpegPath = "ffmpeg.exe";
#endif
    ffmpegPathEdit->setText(settings.value("ffmpeg/path",defaultFfmpegPath).toString());
    userPresetsPathEdit->setText(settings.value("presets/path","").toString());

    //Temp path
    QString defaultTemp = QDir::tempPath() + "/" + qApp->applicationName() + "/";
    defaultTemp = QDir::toNativeSeparators(defaultTemp);
    QString settingsTemp = settings.value("aerender/cache",defaultTemp).toString();
    if (QDir(settingsTemp).exists() && settingsTemp != "") aeCacheEdit->setText(settingsTemp);
    else aeCacheEdit->setText(defaultTemp);

    //aerender path
    QString aerenderPath = settings.value("aerender/path","").toString();
    if (aerenderPath == "") aerenderPath = "Not Found! You may need to install Adobe After Effects.";
    aerenderPath = QDir::toNativeSeparators(aerenderPath);
    aerenderPathEdit->setText(aerenderPath);
    //ae versions
    refreshAeVersionBox();
    //select the right item
    if (settings.value("aerender/useLatest",true).toBool()) aeVersionBox->setCurrentIndex(aeVersionBox->count()-2);
    else if (settings.value("aerender/version","Latest").toString() == "Latest") aeVersionBox->setCurrentIndex(aeVersionBox->count()-2);
    else if (settings.value("aerender/version","Custom").toString() == "Custom")
    {
        aeVersionBox->setCurrentIndex(aeVersionBox->count()-1);
        aerenderPathEdit->setEnabled(true);
        aerenderBrowseButton->setEnabled(true);
    }
    else
    {
        for (int i = 0; i < aeVersionBox->count(); i++)
        {
            if (aeVersionBox->itemText(i) == settings.value("aerender/version","Latest").toString())
            {
                aeVersionBox->setCurrentIndex(i);
                break;
            }
        }
    }
    _freezeUI = false;
}

void SettingsWidget::on_ffmpegBrowseButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"Select the ffmpeg executable binary");
    if (path == "") return;
    if (settings.value("ffmpeg/path").toString() ==  path) return;
    ffmpegPathEdit->setText(path);
    settings.setValue("ffmpeg/path",ffmpegPathEdit->text());
    emit ffmpegPathChanged(path);
}

void SettingsWidget::on_ffmpegPathEdit_editingFinished()
{
    if (settings.value("ffmpeg/path").toString() == ffmpegPathEdit->text()) return;
    settings.setValue("ffmpeg/path",ffmpegPathEdit->text());
    emit ffmpegPathChanged(ffmpegPathEdit->text());
}

void SettingsWidget::on_userPresetsBrowseButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,"Select the folder containing user presets",settings.value("presets/path").toString());
    if (path == "") return;
    userPresetsPathEdit->setText(path);
    settings.setValue("presets/path",path);
    emit presetsPathChanged(path);
}

void SettingsWidget::on_userPresetsPathEdit_editingFinished()
{
    settings.setValue("presets/path",userPresetsPathEdit->text());
    emit presetsPathChanged(userPresetsPathEdit->text());
}

void SettingsWidget::on_aeVersionBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    _freezeUI = true;

    QString path = aeVersionBox->itemData(index).toString();
    path = QDir::toNativeSeparators(path);
    if (path == "Latest")
    {
        AERenderObject *ae = _ffmpeg->getAeRender()->versions().last();
        path = ae->path();
        settings.setValue("aerender/version","Latest");
        settings.setValue("aerender/useLatest",true);
        _ffmpeg->setCurrentAeRender(ae);
    }
    else if (path == "Custom")
    {
        path = settings.value("aerender/path","").toString();
        settings.setValue("aerender/version","Custom");
        settings.setValue("aerender/useLatest",false);
    }
    else
    {
        AERenderObject *ae = _ffmpeg->getAeRender()->versions().at(index);
        settings.setValue("aerender/version",QVariant());
        settings.setValue("aerender/useLatest",false);
        _ffmpeg->setCurrentAeRender(ae);
    }

    aerenderPathEdit->setText(path);
    settings.setValue("aerender/path",QVariant(path));

    aerenderBrowseButton->setEnabled(index == aeVersionBox->count()-1);
    aerenderPathEdit->setEnabled(index == aeVersionBox->count()-1);

    _freezeUI = false;
}

void SettingsWidget::on_aerenderPathEdit_textChanged(const QString &arg1)
{
    if (_freezeUI) return;
    _freezeUI = true;

    QString path = QDir::toNativeSeparators(arg1);

    AERenderObject *ae = _ffmpeg->getAeRender()->getAERenderObject(path);
    if (ae->isValid())
    {
        aerenderPathEdit->setText(path);
        _ffmpeg->setCurrentAeRender(ae);
        for( int i = 0; i < aeVersionBox->count(); i++)
        {
            if (aeVersionBox->itemData(i).toString() == ae->path())
            {
                aeVersionBox->setCurrentIndex(i);
            }
        }
    }

    _freezeUI = false;
}

void SettingsWidget::on_aerenderBrowseButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"Select the aerender executable binary",settings.value("aerender/path","").toString());
    if (path == "") return;
    if (settings.value("aerender/path").toString() == path) return;
    aerenderPathEdit->setText(path);
}

void SettingsWidget::on_aeCacheEdit_textChanged(const QString &arg1)
{
    if (_freezeUI) return;
    _freezeUI = true;
    QString path = arg1;
    if (!path.endsWith("/") || !path.endsWith("\\")) path = path + "/";
    path = QDir::toNativeSeparators(path);

    if (QDir(arg1).exists())
    {
        settings.setValue("aerender/cache",QVariant(path));
        aeCacheEdit->setText(path);
    }
    _freezeUI = false;
}

void SettingsWidget::on_aeCacheBrowseButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,"Select the aerender cache directory",settings.value("aerender/cache",aeCacheEdit->text()).toString());
    if (path == "") return;
    if (settings.value("aerender/cache").toString() == path) return;
    aeCacheEdit->setText(path);
}

void SettingsWidget::refreshAeVersionBox()
{
    _freezeUI = true;

    foreach(AERenderObject *ae, _ffmpeg->getAeRender()->versions())
    {
        aeVersionBox->addItem(ae->name(),QVariant(ae->path()));
    }
    aeVersionBox->addItem("Use latest",QVariant("Latest"));
    aeVersionBox->addItem("Custom...",QVariant("Custom"));

    _freezeUI = false;
}
