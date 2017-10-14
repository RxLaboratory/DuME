#include "mainwindow.h"

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
#ifdef QT_DEBUG
    qDebug() << "Init";
#endif

    setupUi(this);

    // === SETTINGS ===
#ifdef QT_DEBUG
    qDebug() << "Init - Settings";
#endif
    QCoreApplication::setOrganizationName("Duduf");
    QCoreApplication::setOrganizationDomain("duduf.com");
    QCoreApplication::setApplicationName("DuFFmpeg");
    settings = new QSettings(this);

    // === UI SETUP ===

#ifdef QT_DEBUG
    qDebug() << "Init - UI";
#endif
    //remove right click on toolbar
    mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    //populate toolbar
    ToolBarSpacer *tbs = new ToolBarSpacer();
    mainToolBar->addWidget(tbs);
    //window buttons
#ifndef Q_OS_MAC
    // Maximize and minimize only on linux and windows
    this->setWindowFlags(Qt::FramelessWindowHint);
    maximizeButton = new QPushButton(QIcon(":/icons/maximize"),"");
    minimizeButton = new QPushButton(QIcon(":/icons/minimize"),"");
    mainToolBar->addWidget(minimizeButton);
    mainToolBar->addWidget(maximizeButton);
#endif
    quitButton = new QPushButton(QIcon(":/icons/close"),"");
    mainToolBar->addWidget(quitButton);

    //drag window
    toolBarClicked = false;
    mainToolBar->installEventFilter(this);

    //settings widget
    settingsWidget = new SettingsWidget(settings,this);
    settingsPage->layout()->addWidget(settingsWidget);

    //set style
    updateCSS(":/styles/default");


    //init UI
    consoleTabs->setCurrentIndex(0);
    mainStackWidget->setCurrentIndex(0);
    statusLabel = new QLabel("Ready");
    mainStatusBar->addWidget(statusLabel);

    //restore geometry
    settings->beginGroup("mainwindow");
    //size
    resize(settings->value("size", QSize(850, 850)).toSize());
    //position
    move(settings->value("pos", QPoint(200, 200)).toPoint());
    settings->endGroup();
    //console splitter sizes
    settings->beginGroup("consolesplitter");
    QList<int>sizes;
    sizes << settings->value("consolesize",0).toInt();
    sizes << settings->value("queuesize",100).toInt();
    consoleSplitter->setSizes(sizes);
    settings->endGroup();

    //populate sampling box
    samplingBox->addItem("8,000 Hz",QVariant(8000));
    samplingBox->addItem("11,025 Hz",QVariant(11025));
    samplingBox->addItem("16,000 Hz",QVariant(16000));
    samplingBox->addItem("22,050 Hz",QVariant(22050));
    samplingBox->addItem("32,000 Hz",QVariant(32000));
    samplingBox->addItem("44,100 Hz",QVariant(44100));
    samplingBox->addItem("48,000 Hz",QVariant(48000));
    samplingBox->addItem("88,200 Hz",QVariant(88200));
    samplingBox->addItem("96,000 Hz",QVariant(96000));

    // === FFMPEG INIT ===
#ifdef QT_DEBUG
    qDebug() << "Init - FFmpeg";
#endif
    //TODO auto find ffmpeg if no settings or path invalid
    //then save to settings
    ffmpeg = new FFmpeg(settings->value("ffmpeg/path","ffmpeg.exe").toString());
    if (ffmpeg->getStatus() == FFmpeg::Error)
    {
        console(ffmpeg->getLastErrorMessage());
    }


    // === MAP EVENTS ===
#ifdef QT_DEBUG
    qDebug() << "Init - Map Events";
#endif
    // Window management
#ifndef Q_OS_MAC
    // Windows and linux
    connect(maximizeButton,SIGNAL(clicked()),this,SLOT(maximize()));
    connect(minimizeButton,SIGNAL(clicked()),this,SLOT(showMinimized()));
#endif
    connect(quitButton,SIGNAL(clicked()),this,SLOT(close()));
    //FFmpeg
    connect(ffmpeg,SIGNAL(newOutput(QString)),this,SLOT(console(QString)));
    connect(ffmpeg,SIGNAL(processError(QString)),this,SLOT(ffmpeg_errorOccurred(QString)));
    connect(ffmpeg,SIGNAL(encodingStarted(FFQueueItem*)),this,SLOT(ffmpeg_started(FFQueueItem*)));
    connect(ffmpeg,SIGNAL(encodingFinished(FFQueueItem*)),this,SLOT(ffmpeg_finished(FFQueueItem*)));
    connect(ffmpeg,SIGNAL(statusChanged(FFmpeg::Status)),this,SLOT(ffmpeg_statusChanged(FFmpeg::Status)));
    connect(ffmpeg,SIGNAL(progress()),this,SLOT(ffmpeg_progress()));
    //settings
    connect(settingsWidget,SIGNAL(ffmpegPathChanged(QString)),this,SLOT(changeFFmpegPath(QString)));
}

void MainWindow::ffmpeg_init()
{
    ffmpeg->setBinaryFileName(settings->value("ffmpeg/path","E:/DEV SRC/DuFFMpeg/ffmpeg/ffmpeg.exe").toString());
    //get codecs
    videoCodecsBox->clear();
    audioCodecsBox->clear();
    QList<FFCodec> encoders = ffmpeg->getEncoders(true);
    foreach(FFCodec encoder,encoders)
    {
        if (encoder.isVideo())
        {
            videoCodecsBox->addItem(encoder.getPrettyName(),QVariant(encoder.getName()));
        }

        if (encoder.isAudio())
        {
            audioCodecsBox->addItem(encoder.getPrettyName(),QVariant(encoder.getName()));
        }
    }
    //get help
    helpEdit->setText(ffmpeg->getLongHelp());
}

void MainWindow::ffmpeg_errorOccurred(QString e)
{
    console(e);
    mainStatusBar->showMessage("An FFmpeg error has occured, see the console.");
}

void MainWindow::ffmpeg_started(FFQueueItem *item)
{
    //TODO disable corresponding queue widget

    reInitCurrentProgress();
    //Get input infos from first video input
    foreach(FFMediaInfo *input, item->getInputMedias())
    {
        if (input->hasVideo())
        {
            QFileInfo inputFile(input->getFileName());
            mainStatusBar->clearMessage();
            statusLabel->setText("Transcoding: " + inputFile.fileName());

            //adjust progress
            currentEncodingNameLabel->setText(inputFile.fileName());
            progressBar->setMaximum(input->getDuration() * input->getVideoFramerate());
            break;
        }
    }
}

void MainWindow::ffmpeg_finished(FFQueueItem *item)
{
    //TODO move to status changed
    actionStop->setEnabled(false);
    actionGo->setEnabled(true);

    progressBar->setValue(0);
    //TODO add item to history
}

void MainWindow::ffmpeg_statusChanged(FFmpeg::Status status)
{
    if (status == FFmpeg::Waiting)
    {
        actionGo->setEnabled(true);
        actionStop->setEnabled(false);
        mainStatusBar->clearMessage();
        statusLabel->setText("Ready.");
    }
    else if (status == FFmpeg::Encoding)
    {
        actionGo->setEnabled(false);
        actionStop->setEnabled(true);
        mainStatusBar->clearMessage();
        statusLabel->setText("Transcoding...");
    }
    else if (status == FFmpeg::Error)
    {
        actionGo->setEnabled(true);
        actionStop->setEnabled(false);
        statusLabel->setText("Ready.");
    }
}

void MainWindow::ffmpeg_progress()
{
#ifdef QT_DEBUG
    qDebug() << "== Encoding Progress ==";
#endif
    //size
    int outputSize = ffmpeg->getOutputSize(FFMediaInfo::MB);
    outputSizeLabel->setText(QString::number(outputSize) + " MB");
    //bitrate
    int bitrate = ffmpeg->getOutputBitrate(FFMediaInfo::Mbps);
    outputBitrateLabel->setText(QString::number(bitrate) + " Mbps");
    //time elapsed
    QTime elapsed = ffmpeg->getElapsedTime();
    timeLabel->setText(elapsed.toString("hh:mm:ss"));
#ifdef QT_DEBUG
    qDebug() << elapsed.toString("hh:mm:ss") << QString::number(outputSize) + " MB";
#endif
    //speed
    speedLabel->setText(QString::number(ffmpeg->getEncodingSpeed()) + "x");
    //time remaining
    QTime remaining = ffmpeg->getTimeRemaining();
    timeRemainingLabel->setText(remaining.toString("hh:mm:ss"));
    //progress bar
    progressBar->setValue(ffmpeg->getCurrentFrame());
}

void MainWindow::displayMediaInfo(FFMediaInfo *info)
{
    videoWidthButton->setValue(info->getVideoWidth());
    videoHeightButton->setValue(info->getVideoHeight());
    frameRateEdit->setValue(info->getVideoFramerate());
    int samplingRate = info->getAudioSamplingRate();
    samplingBox->setCurrentIndex(6);
    for (int i = 0 ; i < samplingBox->count() ; i++)
    {
        if (samplingRate == samplingBox->itemData(i).toInt())
        {
            samplingBox->setCurrentIndex(i);
            break;
        }
    }

    mediaInfosText->setText("Media Information:\n"+info->getFfmpegOutput());
}

void MainWindow::console(QString log)
{
    //add date
    QTime currentTime = QTime::currentTime();
    //log
    consoleEdit->setText(consoleEdit->toPlainText() + "\n" + currentTime.toString("[hh:mm:ss.zzz]: ") + log);
    // put the slider at the bottom
    consoleEdit->verticalScrollBar()->setSliderPosition(consoleEdit->verticalScrollBar()->maximum());
}

void MainWindow::on_videoTranscodeButton_toggled(bool checked)
{
    videoTranscodeWidget->setEnabled(checked);
}

void MainWindow::on_audioTranscodeButton_toggled(bool checked)
{
    audioTranscodeWidget->setEnabled(checked);
}

void MainWindow::on_resizeButton_toggled(bool checked)
{
    videoWidthButton->setEnabled(checked);
    videoHeightButton->setEnabled(checked);
    widthLabel->setEnabled(checked);
    heightLabel->setEnabled(checked);
    aspectRatioLabel->setEnabled(checked);
}

void MainWindow::on_frameRateButton_toggled(bool checked)
{
    frameRateBox->setEnabled(checked);
    frameRateEdit->setEnabled(checked);
}

void MainWindow::on_samplingButton_toggled(bool checked)
{
    samplingBox->setEnabled(checked);
}

void MainWindow::on_inputBrowseButton_clicked()
{
    //get current file path
    QFileInfo fi(inputEdit->text());
    QString inputPath = QFileDialog::getOpenFileName(this,"Select the media file to transcode",fi.path());
    if (inputPath == "") return;

    //update UI
    inputEdit->setText(inputPath);
    toolBox->setItemText(0,QFileInfo(inputPath).fileName());

    //get media infos
    FFMediaInfo *input = ffmpeg->getMediaInfo(inputPath);
    //populate UI
    videoWidthButton->setValue(input->getVideoWidth());
    videoHeightButton->setValue(input->getVideoHeight());
    frameRateEdit->setValue(input->getVideoFramerate());
    int sampling = input->getAudioSamplingRate();
    if (sampling == 8000) samplingBox->setCurrentIndex(0);
    else if (sampling == 11025) samplingBox->setCurrentIndex(1);
    else if (sampling == 16000) samplingBox->setCurrentIndex(2);
    else if (sampling == 22050) samplingBox->setCurrentIndex(3);
    else if (sampling == 32000) samplingBox->setCurrentIndex(4);
    else if (sampling == 44100) samplingBox->setCurrentIndex(5);
    else if (sampling == 48000) samplingBox->setCurrentIndex(6);
    else if (sampling == 88200) samplingBox->setCurrentIndex(7);
    else if (sampling == 96000) samplingBox->setCurrentIndex(8);
    else samplingBox->setCurrentIndex(6);

    //Text
    QString mediaInfos = "Media information";

    mediaInfos += "\n\nContainers: " + input->getContainer().join(",");
    QTime duration(0,0,0);
    duration = duration.addSecs(input->getDuration());
    mediaInfos += "\nDuration: " + duration.toString("hh:mm:ss.zzz");
    double size = input->getSize(FFMediaInfo::MB);
    int roundedSize = size*1000+0.5;
    size = roundedSize/1000;
    mediaInfos += "\nSize: " + QString::number(size) + " MB";
    mediaInfos += "\nContains video: ";
    if (input->hasVideo()) mediaInfos += "yes";
    else mediaInfos += "no";
    mediaInfos += "\nContains audio: ";
    if (input->hasAudio()) mediaInfos += "yes";
    else mediaInfos += "no";
    mediaInfos += "\nImage sequence: ";
    if (input->isImageSequence()) mediaInfos += "yes";
    else mediaInfos += "no";

    mediaInfos += "\n\nVideo codec: " + input->getVideoCodec().getPrettyName();
    mediaInfos += "\nResolution: " + QString::number(input->getVideoWidth()) + "x" + QString::number(input->getVideoHeight());
    mediaInfos += "\nFramerate: " + QString::number(input->getVideoFramerate()) + " fps";
    int bitrate = input->getVideoBitrate(FFMediaInfo::Mbps);
    mediaInfos += "\nBitrate: " + QString::number(bitrate) + " Mbps";

    mediaInfos += "\n\nAudio codec: " + input->getAudioCodec().getPrettyName();
    mediaInfos += "\nSampling rate: " + QString::number(input->getAudioSamplingRate()) + " Hz";
    int abitrate = input->getAudioBitrate(FFMediaInfo::Kbps);
    mediaInfos += "\nBitrate: " + QString::number(abitrate) + " Kbps";

    mediaInfos += "\n\nFFmpeg analysis:\n" + input->getFfmpegOutput();

    delete input;

    mediaInfosText->setText(mediaInfos);
}

void MainWindow::on_outputBrowseButton_clicked()
{
    QString outputPath = QFileDialog::getSaveFileName(this,"Output file");
    if (outputPath == "") return;
    outputEdit->setText(outputPath);
}

void MainWindow::on_inputEdit_textChanged(const QString &arg1)
{

}

void MainWindow::on_outputEdit_textChanged(const QString &arg1)
{

}

void MainWindow::on_frameRateBox_activated(const QString &arg1)
{
    if (arg1 != "Custom")
    {
        QString num = frameRateBox->currentText().replace(" fps","");
        frameRateEdit->setValue(num.toDouble());
    }
}

void MainWindow::on_frameRateEdit_valueChanged(double arg1)
{
    //look for corresponding value
    for (int i = 1 ; i < frameRateBox->count() ; i++)
    {
        QString num = frameRateBox->itemText(i).replace(" fps","");
        if (num.toDouble() == arg1)
        {
            frameRateBox->setCurrentIndex(i);
            return;
        }
    }
    frameRateBox->setCurrentIndex(0);
}

void MainWindow::on_videoQualitySlider_valueChanged(int value)
{
    if (value >= 90)
    {
        qualityLabel->setText("(Visually) Lossless | " + QString::number(value) + "%");
    }
    else if (value >= 75)
    {
        qualityLabel->setText("Very good | " + QString::number(value) + "%");
    }
    else if (value >= 50)
    {
        qualityLabel->setText("Good | " + QString::number(value) + "%");
    }
    else if (value >= 25)
    {
        qualityLabel->setText("Bad | " + QString::number(value) + "%");
    }
    else
    {
        qualityLabel->setText("Very bad | " + QString::number(value) + "%");
    }
}

void MainWindow::on_videoQualitySlider_sliderReleased()
{
    //Adjust bitrate
    //TODO Must depend on resolution (and bpc)
    //for now, values for full HD / Very good means at least 24Mbps (H.264 blu ray)
    //TODO adjust depending on codec
    int value = videoQualitySlider->value();
    double bitrate = value;
    bitrate = bitrate*24/75;
    videoBitRateEdit->setValue(bitrate);
}

void MainWindow::on_videoBitRateEdit_valueChanged(double arg1)
{
    //Adjust quality
    //TODO Must depend on resolution (and bpc)
    //for now, values for full HD / Very good means at least 24Mbps (H.264 blu ray)
    //TODO adjust depending on codec
    int quality = arg1*75/24;
    videoQualitySlider->setValue(quality);
}

void MainWindow::on_audioQualitySlider_sliderReleased()
{
    //Adjust bitrate
    //TODO adjust depending on codec
    int value = audioQualitySlider->value();
    int bitrate = value*320/100;
    audioBitRateEdit->setValue(bitrate);
}

void MainWindow::on_audioQualitySlider_valueChanged(int value)
{
    if (value >= 90)
    {
        audioQualityLabel->setText("(Nearly) Lossless | " + QString::number(value) + "%");
    }
    else if (value >= 60)
    {
        audioQualityLabel->setText("Very good | " + QString::number(value) + "%");
    }
    else if (value >= 30)
    {
        audioQualityLabel->setText("Good | " + QString::number(value) + "%");
    }
    else if (value >= 17)
    {
        audioQualityLabel->setText("Bad | " + QString::number(value) + "%");
    }
    else
    {
        audioQualityLabel->setText("Very bad | " + QString::number(value) + "%");
    }
}

void MainWindow::on_audioBitRateEdit_valueChanged(int arg1)
{
    //Adjust bitrate
    //TODO adjust depending on codec
    int quality = arg1*100/320;
    audioQualitySlider->setValue(quality);
}

void MainWindow::on_videoWidthButton_valueChanged()
{
    aspectRatio();
}

void MainWindow::on_videoHeightButton_valueChanged()
{
    aspectRatio();
}

void MainWindow::on_actionGo_triggered()
{
    //generate input and output
    FFMediaInfo *input = ffmpeg->getMediaInfo(inputEdit->text());
    FFMediaInfo *output = new FFMediaInfo();
    output->setFileName(outputEdit->text());
    if (!noVideoButton->isChecked())
    {
        output->setVideo(true);
        if (videoCopyButton->isChecked()) output->setVideoCodec(FFCodec("copy","Copy stream"));
        else
        {
            output->setVideoCodec(FFCodec(videoCodecsBox->currentData().toString(),videoCodecsBox->currentText()));
            if (resizeButton->isChecked())
            {
                output->setVideoWidth(videoWidthButton->value());
                output->setVideoHeight(videoHeightButton->value());
            }
            if (frameRateButton->isChecked())
            {
                output->setVideoFramerate(frameRateEdit->value());
            }
            output->setVideoBitrate(videoBitRateEdit->value(),FFMediaInfo::Mbps);
        }
    }
    if (!noAudioButton->isChecked())
    {
        output->setAudio(true);
        if (audioCopyButton->isChecked()) output->setAudioCodec(FFCodec("copy","Copy Stream",false));
        else
        {
            output->setAudioCodec(FFCodec(audioCodecsBox->currentData().toString(),audioCodecsBox->currentText()));
            if (samplingButton->isChecked())
            {
                output->setAudioSamplingRate(samplingBox->currentData().toInt());
            }
            output->setAudioBitrate(audioBitRateEdit->value(),FFMediaInfo::Kbps);
        }
    }
    //Launch!
    ffmpeg->encode(input,output);
}

void MainWindow::on_actionStop_triggered()
{
    mainStatusBar->showMessage("Stopping current transcoding...");
    //TODO ask for confirmation
    ffmpeg->stop(5000);
}

void MainWindow::on_actionSettings_triggered(bool checked)
{
    if (checked)
    {
        mainStackWidget->setCurrentIndex(1);
    }
    else
    {
        mainStackWidget->setCurrentIndex(0);
    }
}

void MainWindow::aspectRatio()
{
    double width = videoWidthButton->value();
    double height = videoHeightButton->value();
    double ratio =  width / height;
    //round it to 3 digits
    int roundedRatio = ratio*100+0.5;
    ratio = roundedRatio;
    ratio = ratio/100;
    qDebug() << ratio;
    aspectRatioLabel->setText(QString::number(ratio) + ":1");
}

void MainWindow::updateCSS(QString cssFileName)
{
    QString css = "";

    QFile cssFile(cssFileName);
    if (cssFile.exists())
    {
        cssFile.open(QFile::ReadOnly);
        css = QString(cssFile.readAll());
        cssFile.close();
    }

    qApp->setStyleSheet(css);
}

void MainWindow::reInitCurrentProgress()
{
    outputSizeLabel->setText("0 MB");
    outputBitrateLabel->setText("0 Mbps");
    timeLabel->setText("00:00:00");
    timeRemainingLabel->setText("00:00:00");
    speedLabel->setText("0x");
    currentEncodingNameLabel->setText("");
    progressBar->setMaximum(100);
    progressBar->setValue(0);
}

#ifndef Q_OS_MAC
void MainWindow::maximize()
{
    //TODO save setting
    if (this->isMaximized())
    {
        maximizeButton->setIcon(QIcon(":/icons/maximize"));
        this->showNormal();
    }
    else
    {
        maximizeButton->setIcon(QIcon(":/icons/unmaximize"));
        this->showMaximized();
    }

}

void MainWindow::changeFFmpegPath(QString path)
{
    ffmpeg_init();
}
#endif

void MainWindow::closeEvent(QCloseEvent *event)
{
    //save ui geometry
    settings->beginGroup("mainwindow");
    settings->setValue("size", size());
    settings->setValue("pos", pos());
    settings->endGroup();
    settings->beginGroup("consolesplitter");
    settings->setValue("consolesize",consoleSplitter->sizes()[0]);
    settings->setValue("queuesize",consoleSplitter->sizes()[1]);
    settings->endGroup();
    settings->sync();

    qDebug() << "About to close";
    event->accept();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::MouseButtonPress)
  {
      QMouseEvent *mouseEvent = (QMouseEvent*)event;
      if (mouseEvent->button() == Qt::LeftButton)
      {
        toolBarClicked = true;
        dragPosition = mouseEvent->globalPos() - this->frameGeometry().topLeft();
        event->accept();
      }
      return true;
  }
  else if (event->type() == QEvent::MouseMove)
  {
    QMouseEvent *mouseEvent = (QMouseEvent*)event;
    if (mouseEvent->buttons() & Qt::LeftButton && toolBarClicked)
    {
        if (this->isMaximized()) this->showNormal();
        this->move(mouseEvent->globalPos() - dragPosition);
        event->accept();
    }
    return true;
  }
  else if (event->type() == QEvent::MouseButtonRelease)
  {
      toolBarClicked = false;
      return true;
  }
#ifndef Q_OS_MAC
  else if (event->type() == QEvent::MouseButtonDblClick)
  {
      maximize();
      event->accept();
      return true;
  }
#endif
  else
  {
      // standard event processing
      return QObject::eventFilter(obj, event);
  }
}
