#include "uimainwindow.h"

#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>
#include "duexr.h"

UIMainWindow::UIMainWindow(FFmpeg *ff, QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    log("Initialization");

    // === SETTINGS ===
    log("Init - Settings");
    settings = new QSettings(this);
    //create user presets folder if it does not exist yet
    QDir home = QDir::home();
    home.mkdir("DuME Presets");

    // Load Renderers info (to be passed to other widgets)
    _ffmpeg = ff;
    _ae = new AfterEffects( this );


    // === UI SETUP ===

    log("Init - UI");
    //remove right click on toolbar
    mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    //populate toolbar
    UIToolBarSpacer *tbs = new UIToolBarSpacer();
    mainToolBar->addWidget(tbs);

    //window buttons for frameless win
#ifdef Q_OS_WIN
    // Maximize and minimize only on windows
    this->setWindowFlags(Qt::FramelessWindowHint);
    maximizeButton = new QPushButton(QIcon(":/icons/maximize"),"");
    minimizeButton = new QPushButton(QIcon(":/icons/minimize"),"");
    mainToolBar->addWidget(minimizeButton);
    mainToolBar->addWidget(maximizeButton);
#endif
    //quit button for Mac & Win
#ifndef Q_OS_LINUX
    quitButton = new QPushButton(QIcon(":/icons/close"),"");
    mainToolBar->addWidget(quitButton);
#endif

    //drag window
    toolBarClicked = false;
    mainToolBar->installEventFilter(this);

    //settings widget
    settingsWidget = new UISettingsWidget(_ffmpeg, _ae, this);
    settingsPage->layout()->addWidget(settingsWidget);

    //queue widget
    queueWidget = new UIQueueWidget(_ffmpeg, this);
    queueLayout->addWidget(queueWidget);

    //add fonts
    QFontDatabase::addApplicationFont(":/fonts/calibri");
    QFontDatabase::addApplicationFont(":/fonts/calibri_bold");
    QFontDatabase::addApplicationFont(":/fonts/calibri_italic");
    QFontDatabase::addApplicationFont(":/fonts/calibri_light");
    QFontDatabase::addApplicationFont(":/fonts/calibri_light_italic");
    QFontDatabase::addApplicationFont(":/fonts/calibri_bold_italic");

    //set style
    updateCSS(":/styles/default");

    //add nice shadows
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
    effect->setBlurRadius(20);
    effect->setXOffset(0);
    effect->setYOffset(5);
    effect->setColor(QColor(0, 0, 0, 70));

    progressWidget->setGraphicsEffect(effect);

    //init UI
    consoleTabs->setCurrentIndex(3);
    mainStackWidget->setCurrentIndex(0);
    statusLabel = new QLabel("Ready");
    mainStatusBar->addWidget(statusLabel);
    versionLabel->setText(qApp->applicationName() + " | version: " + qApp->applicationVersion());

    //restore geometry
    settings->beginGroup("mainwindow");
    //size
    resize(settings->value("size", QSize(850, 850)).toSize());
    //position
    //move(settings->value("pos", QPoint(200, 200)).toPoint());
    settings->endGroup();
    //console splitter sizes
    settings->beginGroup("consolesplitter");
    QList<int>sizes;
    sizes << settings->value("consolesize",0).toInt();
    sizes << settings->value("queuesize",100).toInt();
    consoleSplitter->setSizes(sizes);
    settings->endGroup();

    // === FFMPEG ===
    log("Init - FFmpeg (run test)");
    if (_ffmpeg->status() == MediaUtils::Error)
    {
        log("FFmpeg error", LogUtils::Critical );
        log( _ffmpeg->lastErrorMessage() );
        queuePage->setEnabled(false);
    }
    else
    {
        ffmpeg_init();
    }

    // ==== Create RenderQueue ====

    _renderQueue = new RenderQueue( _ffmpeg, _ae, this );

    //accept drops
    setAcceptDrops(true);

    //map events
    mapEvents();
}

void UIMainWindow::ffmpegLog(QString l, LogUtils::LogType lt)
{
    log( "FFmpeg | " + l, lt);
}

void UIMainWindow::mapEvents()
{
    // === MAP EVENTS ===
    log("Init - Map events");

    //connect maximize and minimize buttons
#ifdef Q_OS_WIN
    connect(maximizeButton,SIGNAL(clicked()),this,SLOT(maximize()));
    connect(minimizeButton,SIGNAL(clicked()),this,SLOT(showMinimized()));
#endif
    //connect close button
#ifndef Q_OS_LINUX
    connect(quitButton,SIGNAL(clicked()),this,SLOT(close()));
#endif
    //FFmpeg
    connect(_ffmpeg,SIGNAL( newLog(QString, LogUtils::LogType) ),this,SLOT( ffmpegLog(QString, LogUtils::LogType)) );
    connect(_ffmpeg,SIGNAL( binaryChanged(QString)),this,SLOT(ffmpeg_init()) );

    //After Effects
    connect(_ae, SIGNAL( newLog(QString, LogUtils::LogType) ), this, SLOT( aeLog(QString, LogUtils::LogType )) );

    //settings
    connect(settingsWidget,SIGNAL(presetsPathChanged()),queueWidget,SLOT(presetsPathChanged()));

    //QueueWidget
    connect(queueWidget,SIGNAL(newLog(QString,LogUtils::LogType)),this,SLOT(log(QString,LogUtils::LogType)));

    //RenderQueue
    connect(_renderQueue, SIGNAL( statusChanged(MediaUtils::Status)), this, SLOT(renderQueueStatusChanged(MediaUtils::Status)) );
    connect(_renderQueue, SIGNAL( newLog( QString, LogUtils::LogType )), this, SLOT( log( QString, LogUtils::LogType )) );
    connect(_renderQueue, SIGNAL( progress( )), this, SLOT( progress( )) );
}

void UIMainWindow::ffmpeg_init()
{
    //get help
    helpEdit->setText(_ffmpeg->getLongHelp());
    queuePage->setEnabled(true);
}

void UIMainWindow::aeLog(QString l, LogUtils::LogType lt)
{
    log("After Effects Info | " + l, lt);
}

void UIMainWindow::ffmpeg_debugLog(QString log)
{
    log("FFmpeg: " + log);
}

void UIMainWindow::ffmpeg_errorOccurred(QString e)
{
    log("FFmpeg error: " + e,Warning);
    mainStatusBar->showMessage("An FFmpeg error has occured, see the console and the debug log.");
}

void UIMainWindow::ffmpeg_started(FFQueueItem *item)
{
    //TODO disable corresponding queue widget

    reInitCurrentProgress();
    //Get input infos from first video input
    foreach(MediaInfo *input, item->getInputMedias())
    {
        if (input->hasVideo())
        {
            QFileInfo inputFile(input->fileName());
            mainStatusBar->clearMessage();
            statusLabel->setText("Transcoding: " + inputFile.fileName());

            //adjust progress
            currentEncodingNameLabel->setText(inputFile.fileName());
            if (input->duration() > 0) progressBar->setMaximum(input->duration() * input->videoFramerate());
            else if (input->isImageSequence()) progressBar->setMaximum(input->frames().count());
            else progressBar->setMaximum(0);

            break;
        }
    }
}

void UIMainWindow::ffmpeg_finished(FFQueueItem *item)
{
    progressBar->setValue(0);
    progressBar->setMaximum(1);
    //TODO add item to history
}

void UIMainWindow::ffmpeg_statusChanged(FFmpeg::Status status)
{
    if (status == FFmpeg::Waiting)
    {
        actionGo->setEnabled(true);
        actionStop->setEnabled(false);
        mainStatusBar->clearMessage();
        statusLabel->setText("Ready.");
        actionStatus->setText("Ready");
        log("== WAITING ==");
        queueWidget->setEnabled(true);
        setCursor(Qt::ArrowCursor);
    }
    else if (status == FFmpeg::Encoding)
    {
        actionGo->setEnabled(false);
        actionStop->setEnabled(true);
        mainStatusBar->clearMessage();
        statusLabel->setText("Transcoding...");
        actionStatus->setText("Transcoding...");
        log("== TRANSCODING ==");
        queueWidget->setEnabled(false);
        setCursor(Qt::BusyCursor);
    }
    else if (status == FFmpeg::Error)
    {
        actionGo->setEnabled(true);
        actionStop->setEnabled(false);
        statusLabel->setText("Ready.");
        actionStatus->setText("An error occured");
        log("== AN ERROR OCCURED ==");
        queueWidget->setEnabled(true);
        setCursor(Qt::ArrowCursor);
    }
    else if (status == FFmpeg::AERendering)
    {
        actionGo->setEnabled(false);
        actionStop->setEnabled(true);
        statusLabel->setText("Rendering.");
        actionStatus->setText("Rendering (Ae)...");
        log("== RENDERING (Ae) ==");
        queueWidget->setEnabled(false);
        setCursor(Qt::BusyCursor);
    }
    else if (status == FFmpeg::Cleaning)
    {
        actionGo->setEnabled(false);
        actionStop->setEnabled(true);
        statusLabel->setText("Cleaning.");
        actionStatus->setText("Cleaning...");
        log("== CLEANING ==");
        queueWidget->setEnabled(true);
        setCursor(Qt::ArrowCursor);
    }
}

void UIMainWindow::ffmpeg_progress()
{
#ifdef QT_DEBUG
    qDebug() << "== Encoding Progress ==";
#endif
    //size
    int outputSize = _ffmpeg->getOutputSize(MediaInfo::MB);
    outputSizeLabel->setText(QString::number(outputSize) + " MB");
    //bitrate
    int bitrate = _ffmpeg->getOutputBitrate(MediaInfo::Mbps);
    outputBitrateLabel->setText(QString::number(bitrate) + " Mbps");
    //time elapsed
    QTime elapsed = _ffmpeg->getElapsedTime();
    timeLabel->setText(elapsed.toString("hh:mm:ss"));

#ifdef QT_DEBUG
    qDebug() << elapsed.toString("hh:mm:ss") << QString::number(outputSize) + " MB";
#endif
    log("=== Encoding progress: output file size: " + QString::number(outputSize) + "MB");
    //speed
    speedLabel->setText(QString::number(_ffmpeg->getEncodingSpeed()) + "x");
    //time remaining
    QTime remaining = _ffmpeg->getTimeRemaining();
    timeRemainingLabel->setText(remaining.toString("hh:mm:ss"));
    //progress bar
    progressBar->setValue(_ffmpeg->getCurrentFrame());
}

void UIMainWindow::log(QString log, LogUtils::LogType type)
{
    //type
    QString typeString = "";
    if ( type == LogUtils::Debug )
    {
        qDebug() << log;
    }
    else if ( type == LogUtils::Information )
    {
        qInfo() << log;
    }
    if (type == LogUtils::Warning)
    {
        qWarning() << log;
        typeString = "/!\\ Warning: ";
    }
    else if (type == LogUtils::Critical)
    {
        qCritical() << log;
        typeString = " --- !!! Critical: ";
    }
    else if (type == LogUtils::Fatal)
    {
        qFatal("%s", qUtf8Printable(log));
        typeString = " === Fatal === ";
    }

    //status bar
    mainStatusBar->showMessage(log);

    //add date
    QTime currentTime = QTime::currentTime();

    //console
    consoleEdit->setText(consoleEdit->toPlainText() + "\n" + currentTime.toString("[hh:mm:ss.zzz]: ") + log);
    consoleEdit->verticalScrollBar()->setSliderPosition(consoleEdit->verticalScrollBar()->maximum());

    //log
    if ( type != LogUtils::Debug )
    {
        debugEdit->setText(debugEdit->toPlainText() + "\n" + currentTime.toString("[hh:mm:ss.zzz]: ") + typeString + "\n" + log);
        debugEdit->verticalScrollBar()->setSliderPosition(debugEdit->verticalScrollBar()->maximum());
    }
}

void UIMainWindow::on_ffmpegCommandsEdit_returnPressed()
{
    on_ffmpegCommandsButton_clicked();
}

void UIMainWindow::on_ffmpegCommandsButton_clicked()
{
    QString commands = ffmpegCommandsEdit->text();
    if (commands == "") commands = "-h";
    _ffmpeg->runCommand(commands);
}

void UIMainWindow::on_actionGo_triggered()
{
    //generate input and output
    MediaInfo *input = queueWidget->getInputMedia();
    QList<MediaInfo *> output = queueWidget->getOutputMedia();

    //Launch!
    log("=== Beginning encoding ===");
    _ffmpeg->encode(input,output);
}

void UIMainWindow::on_actionStop_triggered()
{
    mainStatusBar->showMessage("Stopping current transcoding...");
    //TODO ask for confirmation
    _ffmpeg->stop(6000);
}

void UIMainWindow::on_actionSettings_triggered(bool checked)
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

void UIMainWindow::updateCSS(QString cssFileName)
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
    QString css = UIRainbox::loadCSS(cssFiles);
    qApp->setStyleSheet(css);
}

void UIMainWindow::reInitCurrentProgress()
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
void UIMainWindow::maximize()
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

void UIMainWindow::closeEvent(QCloseEvent *event)
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

    event->accept();
}

bool UIMainWindow::eventFilter(QObject *obj, QEvent *event)
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

void UIMainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        if (urlList.count() == 1)
        {
            QString f = urlList[0].toLocalFile();
            if (QFile(f).exists())
            {
                queueWidget->addInputFile(f);
            }
        }

    }
    else if (mimeData->hasText())
    {
        QString f = mimeData->text();
        if (QFile(f).exists())
        {
            queueWidget->addInputFile(f);
        }
    }

    setCursor(Qt::ArrowCursor);
    event->acceptProposedAction();
}

void UIMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    bool ok = false;

    if (mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        if (urlList.count() > 0)
        {
            ok = QFile(urlList[0].toLocalFile()).exists();
        }

    }
    else if (mimeData->hasText())
    {
        ok = QFile(mimeData->text()).exists();
    }

    if (ok) setCursor(Qt::DragMoveCursor);
    else setCursor(Qt::ForbiddenCursor);

    event->acceptProposedAction();
}

void UIMainWindow::dragMoveEvent(QDragMoveEvent *event)
{
     event->acceptProposedAction();
}

void UIMainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}
