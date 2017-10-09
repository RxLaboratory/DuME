#include "mainwindow.h"

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);

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

    //set style
    updateCSS(":/styles/default");

    consoleTabs->setCurrentIndex(0);
    mainStackWidget->setCurrentIndex(0);

    statusLabel = new QLabel("Ready");
    mainStatusBar->addWidget(statusLabel);


    // === FFMPEG INIT ===

    ffmpegRunningType = -1;
    //Load FFMpeg path
    //TODO in settings
    ffmpegPath = "E:/DEV SRC/DuFFMpeg/ffmpeg/ffmpeg.exe";
    ffmpeg = new QProcess(this);
    ffmpeg->setProgram(ffmpegPath);

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
    connect(quitButton,SIGNAL(clicked()),qApp,SLOT(quit()));
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

void MainWindow::ffmpeg_readyRead(QString ffmpegOutput)
{
    //got codecs list
    if (ffmpegRunningType == 1)
    {
        videoCodecsBox->clear();
        audioCodecsBox->clear();
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
    else if (ffmpegRunningType == 2)
    {
        helpEdit->setText(helpEdit->toPlainText() + ffmpegOutput);
    }
    //encoding
    else if (ffmpegRunningType == 0)
    {
        console(ffmpegOutput);
    }
}

void MainWindow::ffmpeg_errorOccurred(QProcess::ProcessError e)
{
    QString error;
    if (e == QProcess::FailedToStart)
    {
        error = "Failed to start FFMpeg.";
    }
    else if (e == QProcess::Crashed)
    {
        if (ffmpegRunningType == 3)
        {
            error = "WARNING: FFmpeg could not be stopped properly, the output file may be unreadable.";
        }
        else
        {
            error = "FFmpeg just crashed.";
        }
    }
    else if (e == QProcess::Timedout)
    {
        error = "Operation timed out.";
    }
    else if (e == QProcess::WriteError)
    {
        error = "Write Error.";
    }
    else if (e == QProcess::ReadError)
    {
        error = "Cannot read FFMpeg output.";
    }
    else if (e == QProcess::UnknownError)
    {
        error = "An unknown error occured.";
    }
    console(error);
    qDebug() << error;



    mainStatusBar->showMessage("An FFmpeg error has occured, see the console.");
}

void MainWindow::ffmpeg_started()
{
    mainStatusBar->clearMessage();
    if (ffmpegRunningType == 1)
    {
        statusLabel->setText("Listing codecs...");
    }
    else if (ffmpegRunningType == 2)
    {
        statusLabel->setText("Getting FFmpeg help...");
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
        QStringList args("-h");
        args << "long";
        ffmpeg->setArguments(args);
        ffmpegRunningType = 2;
        ffmpeg->start(QIODevice::ReadOnly);
        return;
    }

    mainStatusBar->clearMessage();
    statusLabel->setText("Ready");

    ffmpegRunningType = -1;
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
    aspectLabel->setEnabled(checked);
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
    QString inputPath = QFileDialog::getOpenFileName(this,"Select the media file to transcode");
    if (inputPath == "") return;
    inputEdit->setText(inputPath);
    toolBox->setItemText(0,QFileInfo(inputPath).fileName());
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



