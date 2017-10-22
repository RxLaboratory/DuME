#include "mainwindow.h"

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    debugLog("Initialization");

    // === SETTINGS ===
    debugLog("Init - Settings");
    QCoreApplication::setOrganizationName("Duduf");
    QCoreApplication::setOrganizationDomain("duduf.com");
    QCoreApplication::setApplicationName("DuFFmpeg");
    QCoreApplication::setApplicationVersion(APPVERSION);
    settings = new QSettings(this);
    versionLabel->setText(qApp->applicationName() + " | version: " + qApp->applicationVersion());
    //create user presets folder if it does not exist yet
    QDir home = QDir::home();
    home.mkdir("DuFFmpeg Presets");
    settings->setValue("presets/path",home.path() + "/DuFFmpeg Presets/");


    // === FFMPEG INIT ===
    debugLog("Init - FFmpeg");
    //TODO auto find ffmpeg if no settings or path invalid
    //then save to settings
    ffmpeg = new FFmpeg(settings->value("ffmpeg/path","ffmpeg.exe").toString());


    // === UI SETUP ===

    debugLog("Init - UI");
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

    //queue widget
    queueWidget = new QueueWidget(ffmpeg,this);
    queueLayout->addWidget(queueWidget);

    //set style
    updateCSS(":/styles/default");

    //init UI
    consoleTabs->setCurrentIndex(3);
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
    debugLog("Init - FFmpeg (run test)");
    if (ffmpeg->getStatus() == FFmpeg::Error)
    {
        debugLog("FFmpeg error",Warning);
        debugLog(ffmpeg->getLastErrorMessage());
    }
    else
    {
        ffmpeg_init();
    }

    // === MAP EVENTS ===
#ifdef QT_DEBUG
    qDebug() << "Init - Map Events";
#endif
    debugLog("Init - Map events");
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
    connect(ffmpeg,SIGNAL(binaryChanged()),this,SLOT(ffmpeg_init()));
    connect(ffmpeg,SIGNAL(debugInfo(QString)),this,SLOT(ffmpeg_debugLog(QString)));
    //settings
    connect(settingsWidget,SIGNAL(ffmpegPathChanged(QString)),ffmpeg,SLOT(setBinaryFileName(QString)));
    connect(settingsWidget,SIGNAL(presetsPathChanged(QString)),queueWidget,SLOT(presetsPathChanged(QString)));
}

void MainWindow::ffmpeg_init()
{
    //get help
    helpEdit->setText(ffmpeg->getLongHelp());
}

void MainWindow::ffmpeg_debugLog(QString log)
{
    debugLog("FFmpeg: " + log);
}

void MainWindow::ffmpeg_errorOccurred(QString e)
{
    debugLog("FFmpeg error: " + e,Warning);
    mainStatusBar->showMessage("An FFmpeg error has occured, see the console and the debug log.");
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
            QFileInfo inputFile(input->fileName());
            mainStatusBar->clearMessage();
            statusLabel->setText("Transcoding: " + inputFile.fileName());

            //adjust progress
            currentEncodingNameLabel->setText(inputFile.fileName());
            progressBar->setMaximum(input->duration() * input->videoFramerate());
            break;
        }
    }
}

void MainWindow::ffmpeg_finished(FFQueueItem *item)
{
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
    debugLog("=== Encoding progress: output file size: " + QString::number(outputSize) + "MB");
    //speed
    speedLabel->setText(QString::number(ffmpeg->getEncodingSpeed()) + "x");
    //time remaining
    QTime remaining = ffmpeg->getTimeRemaining();
    timeRemainingLabel->setText(remaining.toString("hh:mm:ss"));
    //progress bar
    progressBar->setValue(ffmpeg->getCurrentFrame());
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

void MainWindow::debugLog(QString log, ErrorType type)
{
    //add date
    QTime currentTime = QTime::currentTime();
    //type
    QString typeString = "";
    if (type == Warning) typeString = "/!\\ Warning: ";
    else if (type == Critical) typeString = " --- !!! Critical: ";
    else if (type == Fatal) typeString = " === Fatal === ";
    //log
    debugEdit->setText(debugEdit->toPlainText() + "\n" + currentTime.toString("[hh:mm:ss.zzz]: ") + typeString + log);
    // put the slider at the bottom
    debugEdit->verticalScrollBar()->setSliderPosition(debugEdit->verticalScrollBar()->maximum());
    qDebug() << typeString << log;
}

void MainWindow::on_ffmpegCommandsEdit_returnPressed()
{
    on_ffmpegCommandsButton_clicked();
}

void MainWindow::on_ffmpegCommandsButton_clicked()
{
    QString commands = ffmpegCommandsEdit->text();
    if (commands == "") commands = "-h";
    ffmpeg->runCommand(commands);
}

void MainWindow::on_actionGo_triggered()
{
    //generate input and output
    FFMediaInfo *input = queueWidget->getInputMedia();
    FFMediaInfo *output = queueWidget->getOutputMedia();

    //Launch!
    debugLog("=== Beginning encoding ===");
    ffmpeg->encode(input,output);
}

void MainWindow::on_actionStop_triggered()
{
    mainStatusBar->showMessage("Stopping current transcoding...");
    //TODO ask for confirmation
    ffmpeg->stop(6000);
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

void MainWindow::updateCSS(QString cssFileName)
{
    QStringList cssFiles(cssFileName);
    //check if there's a duffmpeg file to include
    QFileInfo cssFileInfo(cssFileName);
    QString includeName = cssFileInfo.completeBaseName() + "-duffmpeg";
    QString includePath = cssFileInfo.path() + "/" + includeName + ".css";
    QFile includeFile(includePath);
    includePath = cssFileInfo.path() + "/" + includeName;
    if (!includeFile.exists()) includeFile.setFileName(includePath);
    if (includeFile.exists())
    {
        cssFiles << includePath;
    }
    QString css = RainboxUI::loadCSS(cssFiles);
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

    queueWidget->saveSettings();

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

