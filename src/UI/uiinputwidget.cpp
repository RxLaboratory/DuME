#include "uiinputwidget.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

UIInputWidget::UIInputWidget(FFmpeg *ff, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _mediaInfo = new MediaInfo( ff, this);

    //populate color transfer
    trcBox->addItem("Linear", "linear");
    trcBox->addItem("Log", "log");
    trcBox->addItem("Log square root", "log_sqrt");
    trcBox->addItem("sRGB", "iec61966_2_1");
    trcBox->addItem("xvYCC (Extended-gamut YCC)", "iec61966_2_4");
    trcBox->addItem("BT.709", "bt709");
    trcBox->addItem("BT.470 M (Gamma 2.2)", "gamma22");
    trcBox->addItem("BT.470 BG (Gamma 2.8)", "gamma28");
    trcBox->addItem("BT.1361", "bt1361");
    trcBox->addItem("BT.2020 10 bit", "bt2020_10bit");
    trcBox->addItem("BT.2020 12 bit", "BT.2020_12bit");
    trcBox->addItem("SMPTE 170 M", "smpte170m");
    trcBox->addItem("SMPTE 240 M", "smpte240m");
    trcBox->addItem("SMPTE ST 2084", "smpte2084");
    trcBox->addItem("SMPTE ST 428-1", "smpte428_1");
    trcBox->addItem("Gamma", "gamma");

    trcBox->setCurrentIndex(0);

    updateOptions();
}

MediaInfo *UIInputWidget::getMediaInfo()
{
    //update custom params before returning
    _mediaInfo->clearFFmpegOptions();

    for (int i = 0 ; i < _customParamEdits.count() ; i++)
    {
        QString param = _customParamEdits[i]->text();
        if (param != "")
        {
            QStringList option(param);
            option << _customValueEdits[i]->text();
            _mediaInfo->addFFmpegOption(option);
        }
    }

    return _mediaInfo;
}

void UIInputWidget::openFile(QString file)
{
    QSettings settings;
    if (file == "") return;
    file = QDir::toNativeSeparators( file );

    //Text
    QString mediaInfoString = "Media information";

    QFileInfo fileInfo(file);
    _mediaInfo->updateInfo( fileInfo );

    //update UI
    inputEdit->setText( file );
    //keep in settings
    settings.setValue("input/path", fileInfo.path() );

    if ( _mediaInfo->isAep() )
    {
        _mediaInfo->setAepNumThreads(threadsBox->value());
        if (fileInfo.suffix() == "aep") mediaInfoString += "\n\nAfter Effects project.";
        if (fileInfo.suffix() == "aet") mediaInfoString += "\n\nAfter Effects template.";
        if (fileInfo.suffix() == "aepx") mediaInfoString += "\n\nAfter Effects XML project.";
    }

    mediaInfoString += "\n\nContainers: " + _mediaInfo->extensions().join(",");

    if (_mediaInfo->duration() != 0.0)
    {
        QTime duration(0,0,0);
        duration = duration.addSecs( int( _mediaInfo->duration() ) );
        mediaInfoString += "\nDuration: " + duration.toString("hh:mm:ss.zzz");
    }
    else if (_mediaInfo->isImageSequence())
    {
        mediaInfoString += "\nDuration: " + QString::number(  _mediaInfo->frames().count() ) + " frames";
        mediaInfoString += "\nStart Frame Number: " + QString::number( _mediaInfo->startNumber() );
    }

    qint64 size = _mediaInfo->size( );
    mediaInfoString += "\nSize: " + MediaUtils::sizeString( size );

    if ( !_mediaInfo->isAep() )
    {
        qint64 bitrate = _mediaInfo->bitrate();
        mediaInfoString += "\nGlobal bitrate: " + MediaUtils::bitrateString( bitrate );

        mediaInfoString += "\nContains video: ";
        if (_mediaInfo->hasVideo()) mediaInfoString += "yes";
        else mediaInfoString += "no";
        mediaInfoString += "\nContains audio: ";
        if (_mediaInfo->hasAudio()) mediaInfoString += "yes";
        else mediaInfoString += "no";
        mediaInfoString += "\nImage sequence: ";
        if (_mediaInfo->isImageSequence()) mediaInfoString += "yes";
        else mediaInfoString += "no";

        if (_mediaInfo->hasVideo())
        {
            mediaInfoString += "\n\nVideo codec: ";
            if(_mediaInfo->videoCodec() != nullptr)
            {
                mediaInfoString += _mediaInfo->videoCodec()->prettyName();
            }
            mediaInfoString += "\nResolution: " + QString::number(_mediaInfo->videoWidth()) + "x" + QString::number(_mediaInfo->videoHeight());
            mediaInfoString += "\nVideo Aspect: " + QString::number( int( _mediaInfo->videoAspect()*100+0.5 ) / 100.0) + ":1";
            mediaInfoString += "\nFramerate: " + QString::number(_mediaInfo->videoFramerate()) + " fps";
            qint64 bitrate = _mediaInfo->videoBitrate( );
            if (bitrate != 0) mediaInfoString += "\nBitrate: " + MediaUtils::bitrateString(bitrate);
            mediaInfoString += "\nPixel Aspect: " + QString::number( int(_mediaInfo->pixAspect()*100+0.5)/ 100.0) + ":1";
            if (_mediaInfo->pixFormat() != nullptr)
            {
                mediaInfoString += "\nPixel Format: " + _mediaInfo->pixFormat()->prettyName();
                if (_mediaInfo->pixFormat()->hasAlpha()) mediaInfoString += "\nAlpha: yes";
                else mediaInfoString += "\nAlpha: no";
            }
        }

        if (_mediaInfo->hasAudio())
        {
            mediaInfoString += "\n\nAudio codec: ";
            if(_mediaInfo->audioCodec() != nullptr)
            {
                mediaInfoString += _mediaInfo->audioCodec()->prettyName();
            }
            mediaInfoString += "\nSampling rate: " + QString::number(_mediaInfo->audioSamplingRate()) + " Hz";
            if (_mediaInfo->audioChannels() != "")
            {
                mediaInfoString += "\nChannels: " + _mediaInfo->audioChannels();
            }
            int abitrate = int( _mediaInfo->audioBitrate( ) );
            if (abitrate != 0) mediaInfoString += "\nBitrate: " + MediaUtils::bitrateString(abitrate);
        }
    }

    mediaInfosText->setText(mediaInfoString);

    updateOptions();

    emit newMediaLoaded(_mediaInfo);
}

void UIInputWidget::openFile(QUrl file)
{
    openFile(file.toLocalFile());
}

void UIInputWidget::on_inputBrowseButton_clicked()
{
    QSettings settings;
    QString inputPath = QFileDialog::getOpenFileName(this,"Select the media file to transcode",settings.value("input/path","").toString());
    if (inputPath == "") return;
    openFile(inputPath);
}

void UIInputWidget::on_inputEdit_editingFinished()
{
    //check if file exists, try to read url
    QUrl test(inputEdit->text());
    if (!test.isEmpty())
    {
        if (test.isValid())
        {
            inputEdit->setText(test.toLocalFile());
        }
    }

    openFile(inputEdit->text());
}

void UIInputWidget::on_addParamButton_clicked()
{
    //add a param and a value
    QLineEdit *customParam = new QLineEdit(this);
    customParam->setPlaceholderText("-param");
    customParam->setMinimumWidth(100);
    customParam->setMaximumWidth(100);
    //the value edit
    QLineEdit *customValue = new QLineEdit(this);
    customValue->setPlaceholderText("Value");
    //add to layout and lists
    customParamsLayout->insertRow(customParamsLayout->rowCount(),customParam,customValue);
    _customParamEdits << customParam;
    _customValueEdits << customValue;
}

void UIInputWidget::on_frameRateButton_toggled(bool checked)
{
    frameRateBox->setEnabled(checked);
    frameRateEdit->setEnabled(checked);
    if (checked)
    {
        _mediaInfo->setVideoFramerate( frameRateEdit->value() );
    }
    else
    {
        _mediaInfo->setVideoFramerate( 24.0 );
    }
}

void UIInputWidget::on_trcButton_toggled(bool checked)
{
    trcBox->setEnabled(checked);
    if (checked)
    {
        _mediaInfo->setTrc(trcBox->currentData().toString());
    }
    else
    {
        _mediaInfo->setTrc("");
    }
    emit newMediaLoaded(_mediaInfo);
}

void UIInputWidget::on_frameRateBox_activated(const QString &arg1)
{
    if (arg1 != "Custom")
    {
        QString num = frameRateBox->currentText().replace(" fps","");
        frameRateEdit->setValue(num.toDouble());
    }
}

void UIInputWidget::on_frameRateEdit_valueChanged(double arg1)
{
    //update mediainfo
    _mediaInfo->setVideoFramerate(frameRateEdit->value());
    emit newMediaLoaded(_mediaInfo);
    //look for corresponding value
    for (int i = 1 ; i < frameRateBox->count() ; i++)
    {
        QString num = frameRateBox->itemText(i).replace(" fps","");
        if (int( num.toDouble() * 100 ) == int( arg1 * 100) )
        {
            frameRateBox->setCurrentIndex(i);
            return;
        }
    }
    frameRateBox->setCurrentIndex(0);
}

void UIInputWidget::on_trcBox_currentIndexChanged(int index)
{
    if (trcButton->isChecked())
    {
        _mediaInfo->setTrc(trcBox->itemData(index).toString());
    }
    emit newMediaLoaded(_mediaInfo);
}

void UIInputWidget::on_compButton_clicked()
{
    compEdit->setEnabled(true);
    rqindexButton->setChecked(false);
    compButton->setChecked(true);
    rqindexBox->setEnabled(false);
    aeRenderQueueButton->setChecked(false);
    _mediaInfo->setAepCompName(compEdit->text());
    _mediaInfo->setAepRqindex(0);
    _mediaInfo->setAeUseRQueue(false);
    emit newMediaLoaded(_mediaInfo);
}

void UIInputWidget::on_compEdit_textEdited(const QString &arg1)
{
    _mediaInfo->setAepCompName(arg1);
}

void UIInputWidget::on_rqindexButton_clicked()
{
    rqindexBox->setEnabled(true);
    compEdit->setEnabled(false);
    compButton->setChecked(false);
    rqindexButton->setChecked(true);
    aeRenderQueueButton->setChecked(false);
    _mediaInfo->setAepRqindex(rqindexBox->value());
    _mediaInfo->setAepCompName("");
    _mediaInfo->setAeUseRQueue(false);
    emit newMediaLoaded(_mediaInfo);
}

void UIInputWidget::on_rqindexBox_valueChanged(int arg1)
{
    _mediaInfo->setAepRqindex(arg1);
    _mediaInfo->setAepCompName("");
}

void UIInputWidget::on_aeRenderQueueButton_clicked()
{
    compEdit->setEnabled(false);
    rqindexButton->setChecked(false);
    compButton->setChecked(false);
    rqindexBox->setEnabled(false);
    aeRenderQueueButton->setChecked(true);
    _mediaInfo->setAeUseRQueue(true);
    _mediaInfo->setAepRqindex( 0 );
    _mediaInfo->setAepCompName("");
    emit newMediaLoaded(_mediaInfo);
}

void UIInputWidget::on_aeRenderQueueButton_toggled(bool checked)
{
    if (checked)
    {
        threadsButton->setChecked(false);
        threadsBox->setValue(1);
    }
    else if (!threadsButton->isChecked())
    {
        threadsBox->setValue(QThread::idealThreadCount());
    }
}

void UIInputWidget::on_threadsButton_toggled(bool checked)
{
    threadsBox->setEnabled(checked);
    if (!checked && aeRenderQueueButton->isChecked()) threadsBox->setValue(1);
    else threadsBox->setValue(QThread::idealThreadCount());
}

void UIInputWidget::on_threadsBox_valueChanged(int arg1)
{
    _mediaInfo->setAepNumThreads(arg1);
}

void UIInputWidget::updateOptions()
{
    //frame rate
    frameRateButton->hide();
    frameRateBox->hide();
    frameRateEdit->hide();
    //exr prerender
    exrPreRenderButton->hide();
    //Aep
    aeLabel->hide();
    compButton->hide();
    compEdit->hide();
    threadsBox->hide();
    threadsButton->hide();
    rqindexButton->hide();
    rqindexBox->hide();
    aeRenderQueueButton->hide();

    //get media default extension (needed to adjust some options)
    QString extension = "";
    if (_mediaInfo->extensions().count() > 0) extension = _mediaInfo->extensions()[0];

    // framerate buttons
    if (_mediaInfo->isImageSequence() || _mediaInfo->isAep())
    {
        frameRateButton->show();
        frameRateBox->show();
        frameRateEdit->show();
    }

    //trc (gamma) buttons
    if (extension == "exr_pipe" || _mediaInfo->isAep())
    {
        trcBox->setCurrentText("sRGB");
    }

    //exr prerender
    if (extension == "exr_pipe")
    {
        exrPreRenderButton->show();
    }

    if (_mediaInfo->isAep())
    {
        aeLabel->show();
        compButton->show();
        compEdit->show();
        threadsBox->show();
        threadsButton->show();
        rqindexButton->show();
        rqindexBox->show();
        rqindexButton->setChecked(true);
        compButton->setChecked(false);
        rqindexBox->setEnabled(true);
        compEdit->setEnabled(false);
        threadsButton->setChecked(false);
        //for now, using half the threads.
        //TODO: count depending on RAM (3Go per thread for example)
        threadsBox->setValue(QThread::idealThreadCount()/2);
        aeRenderQueueButton->show();
        aeRenderQueueButton->setChecked(false);
    }

    //uncheck what is hidden
    if (frameRateButton->isHidden()) frameRateButton->setChecked(false);
}
