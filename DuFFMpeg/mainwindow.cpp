#include "mainwindow.h"

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);

    // === SETTINGS ===
    QCoreApplication::setOrganizationName("Duduf");
    QCoreApplication::setOrganizationDomain("duduf.com");
    QCoreApplication::setApplicationName("DuFFmpeg");
    settings = new QSettings(this);

    // === UI SETUP ===


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



    // === FFMPEG INIT ===

    ffmpegTest = new FFmpeg(settings->value("ffmpeg/path","E:/DEV SRC/DuFFMpeg/ffmpeg/ffmpeg.exe").toString());

    ffmpegRunningType = -1;
    ffmpegOutput = "";
    //Load FFMpeg path
    ffmpeg = new QProcess(this);
    //TODO auto find ffmpeg if no settings
    ffmpeg->setProgram(settings->value("ffmpeg/path","E:/DEV SRC/DuFFMpeg/ffmpeg/ffmpeg.exe").toString());

    //connect ffmpeg
    connect(ffmpeg,SIGNAL(readyReadStandardError()),this,SLOT(ffmpeg_stdError()));
    connect(ffmpeg,SIGNAL(readyReadStandardOutput()),this,SLOT(ffmpeg_stdOutput()));
    connect(ffmpeg,SIGNAL(started()),this,SLOT(ffmpeg_started()));
    connect(ffmpeg,SIGNAL(finished(int)),this,SLOT(ffmpeg_finished()));
    connect(ffmpeg,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(ffmpeg_errorOccurred(QProcess::ProcessError)));

    //get codec list
    ffmpeg->setArguments(QStringList("-codecs"));
    ffmpegRunningType = 1;
    ffmpeg->start(QIODevice::ReadOnly);

    // === MAP EVENTS ===

    // Window management
#ifndef Q_OS_MAC
    // Windows and linux
    connect(maximizeButton,SIGNAL(clicked()),this,SLOT(maximize()));
    connect(minimizeButton,SIGNAL(clicked()),this,SLOT(showMinimized()));
#endif
    connect(quitButton,SIGNAL(clicked()),this,SLOT(close()));
}

void MainWindow::ffmpeg_stdError()
{
    QString output = ffmpeg->readAllStandardError();
    ffmpeg_readyRead(output);
}

void MainWindow::ffmpeg_stdOutput()
{
    QString output = ffmpeg->readAllStandardOutput();
    ffmpeg_readyRead(output);
}

void MainWindow::ffmpeg_readyRead(QString output)
{
    ffmpegOutput = ffmpegOutput + output;
    //got codecs list
    if (ffmpegRunningType == 1)
    {
        videoCodecsBox->clear();
        audioCodecsBox->clear();
    }
    //got help
    else if (ffmpegRunningType == 2)
    {
        helpEdit->setText(helpEdit->toPlainText() + output);
    }
    //encoding
    else if (ffmpegRunningType == 0)
    {
        console(output);
    }
}

void MainWindow::ffmpeg_errorOccurred(QProcess::ProcessError e)
{



    mainStatusBar->showMessage("An FFmpeg error has occured, see the console.");
}

void MainWindow::ffmpeg_started()
{
    ffmpegOutput = "";
    mainStatusBar->clearMessage();
    if (ffmpegRunningType == 1)
    {
        statusLabel->setText("Listing codecs...");
    }
    else if (ffmpegRunningType == 2)
    {
        statusLabel->setText("Getting FFmpeg help...");
    }
    else if (ffmpegRunningType == 4)
    {
        statusLabel->setText("Getting file infos...");
    }
    else if (ffmpegRunningType == 0)
    {
        statusLabel->setText("Transcoding...");
    }
    actionGo->setEnabled(false);
    actionStop->setEnabled(true);
}

void MainWindow::ffmpeg_finished()
{
    isReady();
    actionStop->setEnabled(false);

    if (ffmpegRunningType == 1)
    {
        //get help too
        ffmpeg_gotCodecs();
        QStringList args("-h");
        args << "long";
        ffmpeg->setArguments(args);
        ffmpegRunningType = 2;
        ffmpeg->start(QIODevice::ReadOnly);
        return;
    }
    else if (ffmpegRunningType == 4)
    {
        FFMediaInfo *info = ffmpeg_gotMediaInfo();
        displayMediaInfo(info);
        delete info;
    }

    mainStatusBar->clearMessage();
    statusLabel->setText("Ready");

    ffmpegRunningType = -1;

    progressBar->setValue(0);
}

void MainWindow::ffmpeg_gotCodecs()
{
    //get codecs
    QStringList codecs = ffmpegOutput.split("\n");
    for (int i = 10 ; i < codecs.count() ; i++)
    {
        QString codec = codecs[i];
        //if video encoding
        QRegExp reVideo("[\\.D]EV[\\.I][\\.L][\\.S] ([\\S]+)\\s+([^\\(\\n]+)");
        if (reVideo.indexIn(codec) != -1)
        {
            QString codecName = reVideo.cap(1);
            QString codecPrettyName = reVideo.cap(2);
            videoCodecsBox->addItem(codecPrettyName.trimmed(),QVariant(codecName));
        }
        //if audio encoding
        QRegExp reAudio("[\\.D]EA[\\.I][\\.L][\\.S] ([\\S]+)\\s+([^\\(\\n]+)");
        if (reAudio.indexIn(codec) != -1)
        {
            QString codecName = reAudio.cap(1);
            QString codecPrettyName = reAudio.cap(2);
            audioCodecsBox->addItem(codecPrettyName.trimmed(),QVariant(codecName));
        }
    }
}

FFMediaInfo *MainWindow::ffmpeg_gotMediaInfo()
{
    FFMediaInfo *info = new FFMediaInfo(ffmpegOutput,this);
    return info;
}

void MainWindow::displayMediaInfo(FFMediaInfo *info)
{

    videoWidthButton->setValue(info->getVideoWidth());
    videoHeightButton->setValue(info->getVideoHeight());
    frameRateEdit->setValue(info->getVideoFramerate());
    int samplingRate = info->getAudioSamplingRate();
    if (samplingRate == 8000) samplingBox->setCurrentIndex(0);
    else if (samplingRate == 11025) samplingBox->setCurrentIndex(1);
    else if (samplingRate == 16000) samplingBox->setCurrentIndex(2);
    else if (samplingRate == 22050) samplingBox->setCurrentIndex(3);
    else if (samplingRate == 32000) samplingBox->setCurrentIndex(4);
    else if (samplingRate == 44100) samplingBox->setCurrentIndex(5);
    else if (samplingRate == 48000) samplingBox->setCurrentIndex(6);
    else if (samplingRate == 88200) samplingBox->setCurrentIndex(7);
    else if (samplingRate == 96000) samplingBox->setCurrentIndex(8);
    else samplingBox->setCurrentIndex(6);

    mediaInfosText->setText("Media Information:\n"+info->getFfmpegOutput());
}

void MainWindow::console(QString log)
{
    consoleEdit->setText(consoleEdit->toPlainText() + "\n" + log);
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
    inputEdit->setText(inputPath);
    toolBox->setItemText(0,QFileInfo(inputPath).fileName());
    //get media infos
    QStringList arguments("-i");
    arguments << inputEdit->text().replace("/","\\");
    ffmpeg->setArguments(arguments);
    ffmpegRunningType = 4;
    ffmpeg->start(QIODevice::ReadOnly);
    isReady();
}

void MainWindow::on_outputBrowseButton_clicked()
{
    QString outputPath = QFileDialog::getSaveFileName(this,"Output file");
    if (outputPath == "") return;
    outputEdit->setText(outputPath);
    isReady();
}

void MainWindow::on_inputEdit_textChanged(const QString &arg1)
{
    isReady();
}

void MainWindow::on_outputEdit_textChanged(const QString &arg1)
{
    isReady();
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
    QStringList arguments = generateArguments(1);

    qDebug() << "LAUNCHING WITH ARGUMENTS:" << arguments.join(" | ");

    //Launch!
    ffmpeg->setArguments(arguments);
    ffmpegRunningType = 0;
    ffmpeg->start(QIODevice::ReadWrite);
    ffmpeg->waitForStarted();
}

void MainWindow::on_actionStop_triggered()
{
    ffmpegRunningType = 3;
    mainStatusBar->showMessage("Stopping current transcoding...");
    //TODO ask for confirmation
    ffmpeg->write("q");
    // Wait until it finishes, or ask to kill
    if (!ffmpeg->waitForFinished(2000))
    {
        //TODO ask before killing
        ffmpeg->kill();
    }
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

bool MainWindow::isReady()
{
    if (!QFile(inputEdit->text()).exists())
    {
        actionGo->setEnabled(false);
        //TODO add warning icon
        return false;
    }
    if (outputEdit->text() == "")
    {
        actionGo->setEnabled(false);
        //TODO add warning icon
        return false;
    }
    actionGo->setEnabled(true);
    return true;
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

QStringList MainWindow::generateArguments(int pass)
{
    QStringList arguments("-stats");

    arguments << "-y";

    //inFile
    QStringList inArgs("-i");
    inArgs << inputEdit->text().replace("/","\\");
    arguments.append(inArgs);

    //out options
    if (noVideoButton->isChecked())
    {
        arguments << "-vn";
    }
    else if (videoCopyButton->isChecked())
    {
        arguments << "-vcodec" << "copy";
    }
    else
    {
        //resize
        if (resizeButton->isChecked())
        {
            arguments << "-s" << QString::number(videoWidthButton->value()) + "x" + QString::number(videoHeightButton->value()) ;
        }
        if (frameRateButton->isChecked())
        {
            arguments << "-r" << QString::number(frameRateEdit->value());
        }
        arguments << "-vcodec" << videoCodecsBox->currentData().toString();
        arguments << "-b:v" << QString::number(videoBitRateEdit->value()*1024*1024);
    }

    if (noAudioButton->isChecked())
    {
        arguments << "-an";
    }
    else if (audioCopyButton->isChecked())
    {
        arguments << "-acodec" << "copy";
    }
    else
    {
        if (samplingButton->isChecked())
        {
            arguments << "-ar" << samplingBox->currentText().replace(" Hz","").replace(",","");
        }
        arguments << "-acodec" << audioCodecsBox->currentData().toString();
        arguments << "-b:a" << QString::number(audioBitRateEdit->value()*1024);
    }

    //output file
    arguments << outputEdit->text().replace("/","\\");

    return arguments;
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

#ifndef Q_OS_MAC
void MainWindow::maximize()
{

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
