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
    //FFmpeg
    connect(_ffmpeg,SIGNAL( newLog(QString, LogUtils::LogType) ),this,SLOT( ffmpegLog(QString, LogUtils::LogType)) );
    connect( _ffmpeg, SIGNAL( console(QString)), this, SLOT( ffmpegConsole(QString)) );
    connect(_ffmpeg,SIGNAL( binaryChanged(QString)),this,SLOT(ffmpeg_init()) );
    //After Effects
    connect(_ae, SIGNAL( newLog(QString, LogUtils::LogType) ), this, SLOT( aeLog(QString, LogUtils::LogType )) );
    connect( _ae, SIGNAL( console(QString)), this, SLOT( aeConsole(QString)) );

    // === UI SETUP ===

    log("Init - UI");
    //remove right click on toolbar
    mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    //populate toolbar
    UIToolBarSpacer *tbs = new UIToolBarSpacer();
    mainToolBar->addWidget(tbs);

    //window buttons for frameless win

    minimizeButton = new QToolButton();
    minimizeButton->setIcon(QIcon(":/icons/minimize"));
    minimizeButton->setObjectName("minimizeButton");
#ifndef Q_OS_MAC
    mainToolBar->addWidget(minimizeButton);
#endif
    maximizeButton = new QToolButton(this);
    maximizeButton->setIcon(QIcon(":/icons/maximize"));
    maximizeButton->setObjectName("maximizeButton");
    mainToolBar->addWidget(maximizeButton);
    quitButton = new QToolButton(this);
    quitButton->setIcon(QIcon(":/icons/close"));
    quitButton->setObjectName("quitButton");
    mainToolBar->addWidget(quitButton);
#ifndef Q_OS_MAC
    this->setWindowFlags(Qt::FramelessWindowHint);
#endif

    //drag window
    toolBarClicked = false;
    mainToolBar->installEventFilter(this);

    log("Init - Add settings widget", LogUtils::Debug);

    //settings widget
    settingsWidget = new UISettingsWidget(_ffmpeg, _ae, this);
    settingsPage->layout()->addWidget(settingsWidget);

    log("Init - Add queue widget", LogUtils::Debug);

    //queue widget
    queueWidget = new UIQueueWidget(_ffmpeg, this);
    qDebug() << "test";
    queueLayout->addWidget(queueWidget);

    log("Init - Appearance", LogUtils::Debug);

    //add nice shadows
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
    effect->setBlurRadius(20);
    effect->setXOffset(0);
    effect->setYOffset(5);
    effect->setColor(QColor(0, 0, 0, 70));

    progressWidget->setGraphicsEffect(effect);

    log("Init - Load Defaults", LogUtils::Debug);

    //init UI
    consoleTabs->setCurrentIndex(0);
    mainStackWidget->setCurrentIndex(0);
    statusLabel = new QLabel("Ready");
    mainStatusBar->addWidget(statusLabel);
    versionLabel->setText(qApp->applicationName() + " | version: " + qApp->applicationVersion());

    log("Init - Set Geometry", LogUtils::Debug);

    //restore geometry
    settings->beginGroup("mainwindow");
    //size
    resize(settings->value("size", QSize(850, 850)).toSize());
    //position
    //move(settings->value("pos", QPoint(200, 200)).toPoint());
    //maximized
#ifndef Q_OS_MAC
    maximize( settings->value("maximized",false).toBool() );
#endif
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

    log("Init - Create render queue");

    _renderQueue = new RenderQueue( _ffmpeg, _ae, this );
    connect(_renderQueue, SIGNAL( statusChanged(MediaUtils::RenderStatus)), this, SLOT(renderQueueStatusChanged(MediaUtils::RenderStatus)) );
    connect(_renderQueue, SIGNAL( newLog( QString, LogUtils::LogType )), this, SLOT( log( QString, LogUtils::LogType )) );
    connect(_renderQueue, SIGNAL( ffmpegConsole( QString )), this, SLOT( ffmpegConsole( QString )) );
    connect(_renderQueue, SIGNAL( aeConsole( QString )), this, SLOT( aeConsole( QString )) );
    connect(_renderQueue, SIGNAL( progress( )), this, SLOT( progress( )) );

    //accept drops
    setAcceptDrops(true);

    // final connections

    //connect maximize and minimize buttons
    connect(maximizeButton,SIGNAL(clicked()),this,SLOT(maximize()));
    connect(minimizeButton,SIGNAL(clicked()),this,SLOT(showMinimized()));
    //connect close button
    connect(quitButton,SIGNAL(clicked()),this,SLOT(close()));

    //settings
    connect(settingsWidget,SIGNAL(presetsPathChanged()),queueWidget,SLOT(presetsPathChanged()));

    //QueueWidget
    connect(queueWidget,SIGNAL(newLog(QString,LogUtils::LogType)),this,SLOT(log(QString,LogUtils::LogType)));

    //Re-set StyleSheet
    UIRainbox::updateCSS(":/styles/default", "dume");
    //and font
    UIRainbox::setFont();

    log("Ready!");

}

void UIMainWindow::ffmpegLog(QString l, LogUtils::LogType lt)
{
    log( "FFmpeg | " + l, lt);
}

void UIMainWindow::ffmpegConsole(QString c)
{
    //add date
    QTime currentTime = QTime::currentTime();

    //log
    consoleEdit->setText(consoleEdit->toPlainText() + "\n" + currentTime.toString("[hh:mm:ss.zzz]: ") + c );
    consoleEdit->verticalScrollBar()->setSliderPosition(consoleEdit->verticalScrollBar()->maximum());
}

void UIMainWindow::ffmpeg_init()
{
    //get help
    helpEdit->setText(_ffmpeg->longHelp());
    queuePage->setEnabled(true);
}

void UIMainWindow::aeLog(QString l, LogUtils::LogType lt)
{
    log("After Effects Info | " + l, lt);
}

void UIMainWindow::aeConsole(QString c)
{
    //add date
    QTime currentTime = QTime::currentTime();

    //log
    aeConsoleEdit->setText(aeConsoleEdit->toPlainText() + "\n" + currentTime.toString("[hh:mm:ss.zzz]: ") + c );
    aeConsoleEdit->verticalScrollBar()->setSliderPosition(aeConsoleEdit->verticalScrollBar()->maximum());
}

void UIMainWindow::progress()
{
    //get input info
    QueueItem *item = _renderQueue->currentItem();
    QString filename = "";
    int numFrames = _renderQueue->numFrames();
    if ( item != nullptr )
    {
        foreach(MediaInfo *input, item->getInputMedias())
        {
            if ( input->hasVideo() || input->isAep() )
            {
                QFileInfo inputFile(input->fileName());
                mainStatusBar->clearMessage();
                filename = inputFile.fileName();

                //adjust progress

                if (numFrames == 0)
                {
                    if ( input->duration() > 0 ) numFrames = input->duration() * input->videoFramerate();
                    else if ( input->isImageSequence() ) numFrames = input->frames().count();
                }
                break;
            }
        }
    }

    currentEncodingNameLabel->setText( filename );

    progressBar->setMaximum( numFrames );

    progressBar->setValue( _renderQueue->currentFrame() );

    double outputSize = _renderQueue->outputSize();
    QString outputSizeText = "";
    if ( outputSize < 10*1024*1024*1024.0 )
    {
        outputSize = outputSize / 1024/1024;
        outputSizeText = QString::number(int(outputSize*100)/100) + " MB";
    }
    else
    {
        outputSize = outputSize /1024/1024/1024;
        outputSizeText = QString::number(int(outputSize*100)/100) + " GB";
    }
    outputSizeLabel->setText( outputSizeText );

    double outputBitrate = _renderQueue->outputBitrate();
    QString outputBitrateText = "";
    if ( outputBitrate < 10000000 )
    {
        outputBitrate = outputBitrate / 1000;
        outputBitrateText = QString::number(int(outputBitrate*100)/100) + " kbps";
    }
    else
    {
        outputBitrate = outputBitrate /1000/1000;
        outputBitrateText = QString::number(int(outputBitrate*100)/100) + " Mbps";
    }
    outputBitrateLabel->setText( outputBitrateText );

    double expectedSize = _renderQueue->expectedSize();
    QString expectedSizeText = "";
    if ( expectedSize < 10*1024*1024*1024.0 )
    {
        expectedSize = expectedSize /1024/1024;
        expectedSizeText = QString::number(int(expectedSize*100)/100) + " MB";
    }
    else
    {
        expectedSize = expectedSize /1024/1024/1024;
        expectedSizeText = QString::number(int(expectedSize*100)/100) + " GB";
    }
    expectedSizeLabel->setText ( expectedSizeText );

    speedLabel->setText( QString::number(int(_renderQueue->encodingSpeed()*100)/100) + "x");

    timeRemainingLabel->setText( _renderQueue->remainingTime().toString("hh:mm:ss"));

    timeLabel->setText( _renderQueue->elapsedTime().toString("hh:mm:ss") );

}

void UIMainWindow::renderQueueStatusChanged(MediaUtils::RenderStatus status)
{
    QString stText = MediaUtils::statusString( status );
    actionStatus->setText( stText );
    statusLabel->setText( stText );
    log( stText );


    if( MediaUtils::isBusy( status ) )
    {
        queuePage->setEnabled( false );
        actionGo->setEnabled(false);
        actionStop->setEnabled(true);
        mainStatusBar->clearMessage();
        setCursor(Qt::BusyCursor);
    }
    else
    {
        queuePage->setEnabled( true );

        reInitCurrentProgress();
        currentEncodingNameLabel->setText("");
        actionGo->setEnabled(true);
        actionStop->setEnabled(false);
        mainStatusBar->clearMessage();
        setCursor(Qt::ArrowCursor);
    }
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

    //log
    if ( type != LogUtils::Debug )
    {
        //status bar
        mainStatusBar->showMessage(log);

        //add date
        QTime currentTime = QTime::currentTime();
        debugEdit->setText(debugEdit->toPlainText() + "\n" + currentTime.toString("[hh:mm:ss.zzz]: ") + typeString + log);
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
    _renderQueue->encode(input,output);
}

void UIMainWindow::on_actionStop_triggered()
{
    mainStatusBar->showMessage("Stopping current transcoding...");
    //TODO ask for confirmation
    _renderQueue->stop(6000);
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

void UIMainWindow::reInitCurrentProgress()
{
    progressBar->setMaximum( 1 );
    progressBar->setValue( 0 );
    outputSizeLabel->setText( "0 MB" );
    outputBitrateLabel->setText( "0 bps" );
    expectedSizeLabel->setText( "0 MB" );
    speedLabel->setText("x");
    timeRemainingLabel->setText("00:00:00");
    timeLabel->setText("00:00:00");
}

void UIMainWindow::maximize(bool max)
{
    if (!max)
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

void UIMainWindow::maximize()
{
    maximize(!this->isMaximized());
}

void UIMainWindow::closeEvent(QCloseEvent *event)
{
    _renderQueue->stop(6000);
    //save ui geometry
    settings->beginGroup("mainwindow");
    settings->setValue("size", size());
    settings->setValue("pos", pos());
    settings->setValue("maximized",this->isMaximized());
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
      if (this->isMaximized()) return false;
    QMouseEvent *mouseEvent = (QMouseEvent*)event;
    if (mouseEvent->buttons() & Qt::LeftButton && toolBarClicked)
    {
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
