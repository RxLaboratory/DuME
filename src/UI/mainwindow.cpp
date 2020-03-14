#include "mainwindow.h"

#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsBlurEffect>
#include "duexr.h"

MainWindow::MainWindow(int argc, char *argv[], QWidget *parent) :
    QMainWindow(parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Main Window";
#endif
    setupUi(this);
    log("Initialization");

    // === SETTINGS ===
    log("Init - Loading settings");
    settings = new QSettings(this);
    //create user presets folder if it does not exist yet
    QDir home = QDir::home();
    home.mkdir("DuME Presets");

    // Load Renderers info (to be passed to other widgets)
    log("Init - Connecting to FFmpeg");
    //FFmpeg
    connect( ffmpeg,SIGNAL( newLog(QString, LogUtils::LogType) ),this,SLOT( ffmpegLog(QString, LogUtils::LogType)) );
    connect( ffmpeg, SIGNAL( console(QString)), this, SLOT( ffmpegConsole(QString)) );
    connect( ffmpeg, SIGNAL( valid(bool) ), this, SLOT( ffmpegValid(bool)) );
    connect( ffmpeg,SIGNAL( statusChanged(MediaUtils::RenderStatus)), this, SLOT ( ffmpegStatus(MediaUtils::RenderStatus)) );
    //After Effects
    log("Init - Initializing the After Effects renderer");
    _ae = new AfterEffects( this );
    log("Init - Connecting to After Effects");
    connect(_ae, SIGNAL( newLog(QString, LogUtils::LogType) ), this, SLOT( aeLog(QString, LogUtils::LogType )) );
    connect( _ae, SIGNAL( console(QString)), this, SLOT( aeConsole(QString)) );

    // === UI SETUP ===

    log("Init - Completing User Interface");
    //remove right click on toolbar
    mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);

    //populate toolbar
    QMenu *goMenu = new QMenu();
    goMenu->addAction( actionGo );
    goMenu->addAction( actionGoQuit );

    QToolButton *goButton = new QToolButton(this);
    goButton->setIcon(actionGo->icon());
    goButton->setText(actionGo->text());
    goButton->setToolTip(actionGo->toolTip());
    goButton->setMenu(goMenu);
    goButton->setPopupMode(QToolButton::MenuButtonPopup);
    connect( goButton, SIGNAL( clicked()), this, SLOT ( on_actionGo_triggered()) );
    mainToolBar->addWidget(goButton);
    mainToolBar->addAction(actionStop);
    mainToolBar->addAction(actionStatus);
#ifdef QT_DEBUG
    mainToolBar->addAction(actionDevBuild);
#endif

    ToolBarSpacer *tbs = new ToolBarSpacer();
    mainToolBar->addWidget(tbs);

    //settings and other buttons to the right
    mainToolBar->addAction(actionSettings);
    mainToolBar->widgetForAction( actionSettings )->setObjectName("windowButton");

    QToolButton *helpButton = new QToolButton(this);
    helpButton->setIcon(QIcon(":/icons/help"));
    helpButton->setObjectName("windowButton");
    helpButton->setPopupMode( QToolButton::InstantPopup );
    helpMenu = new QMenu(this);
    helpMenu->addAction(actionBug_report);
    helpMenu->addAction(actionChat);
    helpMenu->addAction(actionForum);
    helpMenu->addAction(actionHelp);

    helpButton->setMenu(helpMenu);

    mainToolBar->addWidget( helpButton );

    //window buttons for frameless win

    minimizeButton = new QToolButton();
    minimizeButton->setIcon(QIcon(":/icons/minimize"));
    minimizeButton->setObjectName("windowButton");
#ifndef Q_OS_MAC
    mainToolBar->addWidget(minimizeButton);
#endif
    maximizeButton = new QToolButton(this);
    maximizeButton->setIcon(QIcon(":/icons/maximize"));
    maximizeButton->setObjectName("windowButton");
    mainToolBar->addWidget(maximizeButton);
    quitButton = new QToolButton(this);
    quitButton->setIcon(QIcon(":/icons/close"));
    quitButton->setObjectName("windowButton");
    mainToolBar->addWidget(quitButton);
#ifndef Q_OS_MAC
    this->setWindowFlags(Qt::FramelessWindowHint);
#endif

    //drag window
    toolBarClicked = false;
    mainToolBar->installEventFilter(this);

    log("Init - Adding settings widget");

    //settings widget
    settingsWidget = new SettingsWidget(_ae, this);
    settingsPage->layout()->addWidget(settingsWidget);

    log("Init - Adding queue widget");

    //queue widget
    queueWidget = new QueueWidget(this);
    queueLayout->addWidget(queueWidget);

    log("Init - Adding shadows");

    //add nice shadows
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
    effect->setBlurRadius(20);
    effect->setXOffset(0);
    effect->setYOffset(5);
    effect->setColor(QColor(0, 0, 0, 70));

    progressWidget->setGraphicsEffect(effect);

    log("Init - Setting default UI items");

    //init UI
    consoleTabs->setCurrentIndex(0);
    mainStackWidget->setCurrentIndex(0);
    statusLabel = new QLabel("Ready");
    mainStatusBar->addWidget(statusLabel);
    versionLabel->setText(qApp->applicationName() + " | version: " + qApp->applicationVersion());

    log("Init - Setting window geometry");

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
    ffmpegValid( ffmpeg->isValid() );

    // ==== Create RenderQueue ====

    log("Init - Creating render queue");

    _renderQueue = new RenderQueue( _ae, this );
    connect(_renderQueue, SIGNAL( statusChanged(MediaUtils::RenderStatus)), this, SLOT(renderQueueStatusChanged(MediaUtils::RenderStatus)) );
    connect(_renderQueue, SIGNAL( newLog( QString, LogUtils::LogType )), this, SLOT( log( QString, LogUtils::LogType )) );
    connect(_renderQueue, SIGNAL( ffmpegConsole( QString )), this, SLOT( ffmpegConsole( QString )) );
    connect(_renderQueue, SIGNAL( aeConsole( QString )), this, SLOT( aeConsole( QString )) );
    connect(_renderQueue, SIGNAL( progress( )), this, SLOT( progress( )) );

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

    log("Init - Setting stylesheet");
    //Re-set StyleSheet
    DuUI::updateCSS(":/styles/default", "dume");
    //and font
    DuUI::setFont();

    //parse arguments if ffmpeg is valid
    autoQuit = false;

    if (ffmpeg->isValid())
    {
        if (argc == 1) queueWidget->addInputFile("");
        else
        {
            log("Reading arguments.");
            int i = 1;

            //Arguments
            QString compName = "";
            int rqItem = 0;
            bool useQueue = false;
            double framerate = 0;
            QString colorProfile = "";
            bool autoStart = false;

            while (i < argc)
            {
                QString arg = argv[i];
                if (arg.startsWith("-"))
                {
                    arg = arg.toLower();
                    if ( arg == "-usequeue" ) useQueue = true;
                    else if ( arg == "-comp" && i < argc-1 )
                    {
                        i++;
                        compName = argv[i];
                    }
                    else if (arg == "-rqitem" && i < argc-1 )
                    {
                        i++;
                        rqItem = QString(argv[i]).toInt();
                    }
                    else if ( (arg == "-framerate" || arg == "-fps" ) && i < argc-1 )
                    {
                        i++;
                        framerate = QString(argv[i]).toDouble();
                    }
                    else if (arg == "-colorprofile" && i < argc-1 )
                    {
                        i++;
                        colorProfile = argv[i];
                    }
                    else if ( (arg == "-output" || arg == "-o") && i < argc-1 )
                    {
                        i++;
                        queueWidget->setOutputPath( argv[i] );
                    }
                    else if ( arg == "-autostart" )
                    {
                        autoStart = true;
                    }
                    else if ( arg == "-autoquit" )
                    {
                        autoQuit = true;
                    }
                    else if ( arg == "-preset" && i < argc-1 )
                    {
                        i++;
                        queueWidget->setOutputPreset( argv[i] );
                    }
                    else if (arg != "-nobanner" && arg != "-hideconsole")
                    {
                        log("Unknown argument: " + arg + ". All other arguments will be ignored", LogUtils::Warning);
                        break;
                    }
                    i++;
                }
                else
                {
                    //open
                    log( "Opening " + arg );
                    MediaInfo *input = queueWidget->addInputFile( arg );
                    //set params
                    if (input->isAep())
                    {
                        if (compName != "") input->setAepCompName(compName);
                        else if (rqItem > 0) input->setAepRqindex(rqItem);
                        input->setAeUseRQueue(useQueue);
                    }
                    input->setFramerate(framerate);
                    input->setColorProfile( colorProfile );
                    //reinit args
                    compName = "";
                    rqItem = 0;
                    useQueue = false;
                    framerate = 0;
                    colorProfile = "";
                    i++;
                }
            }

            //autostart
            if (autoStart) go();
        }
    }
    log("Ready!");
}

void MainWindow::ffmpegLog(QString l, LogUtils::LogType lt)
{
    log( "FFmpeg | " + l, lt);
}

void MainWindow::ffmpegConsole(QString c)
{
    //add date
    QTime currentTime = QTime::currentTime();

    //log
    consoleEdit->setText(consoleEdit->toPlainText() + "\n" + currentTime.toString("[hh:mm:ss.zzz]: ") + c );
    consoleEdit->verticalScrollBar()->setSliderPosition(consoleEdit->verticalScrollBar()->maximum());
}

void MainWindow::ffmpegValid(bool valid)
{
    if ( valid )
    {
        queuePage->setGraphicsEffect( nullptr );
        helpEdit->setText(ffmpeg->longHelp());
        queuePage->setEnabled(true);
        setAcceptDrops( true );
        actionStatus->setText( "Ready");
        actionGo->setEnabled( true );
    }
    else
    {
        queuePage->setGraphicsEffect( new QGraphicsBlurEffect() );
        log("FFmpeg error", LogUtils::Critical );
        log( ffmpeg->lastErrorMessage() );
        queuePage->setEnabled(false);
        setAcceptDrops( false );
        actionStatus->setText( "FFmpeg not found or not working properly. Set its path in the settings.");
        actionGo->setEnabled( false );
    }
}

void MainWindow::ffmpegStatus(MediaUtils::RenderStatus status)
{
    if ( status == MediaUtils::Initializing )
    {
        ffmpegValid( false );
        actionStatus->setText( "Initializing FFmpeg..." );
    }
    else
    {
       ffmpegValid( ffmpeg->isValid() );
    }
}

void MainWindow::aeLog(QString l, LogUtils::LogType lt)
{
    log("After Effects Info | " + l, lt);
}

void MainWindow::aeConsole(QString c)
{
    //add date
    QTime currentTime = QTime::currentTime();

    //log
    aeConsoleEdit->setText(aeConsoleEdit->toPlainText() + "\n" + currentTime.toString("[hh:mm:ss.zzz]: ") + c );
    aeConsoleEdit->verticalScrollBar()->setSliderPosition(aeConsoleEdit->verticalScrollBar()->maximum());
}

void MainWindow::progress()
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
                    if (input->hasVideo())
                    {
                        if ( input->duration() > 0 ) numFrames = input->duration() * input->videoStreams()[0]->framerate();
                        else if ( input->isSequence() ) numFrames = input->frames().count();
                    }
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

void MainWindow::renderQueueStatusChanged(MediaUtils::RenderStatus status)
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

        if ( autoQuit )
        {
            // Quit after a few seconds to be sure we're finished
            QTimer::singleShot(2000, this, SLOT(quit()));
            // Display a warning
            actionStatus->setText("About to quit. See you soon!");
        }
    }
}

void MainWindow::log(QString log, LogUtils::LogType type)
{
    //type
    QString typeString = "";
    if ( type == LogUtils::Debug )
    {
        qDebug().noquote() << log;
    }
    else if ( type == LogUtils::Information )
    {
        qInfo().noquote() << log;
    }
    if (type == LogUtils::Warning)
    {
        qWarning().noquote() << log;
        typeString = "/!\\ Warning: ";
    }
    else if (type == LogUtils::Critical)
    {
        qCritical().noquote() << log;
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

void MainWindow::go()
{
    //generate input and output
    QList<MediaInfo *> input = queueWidget->getInputMedia();
    QList<MediaInfo *> output = queueWidget->getOutputMedia();

    //Launch!
    log("=== Beginning encoding ===");
    _renderQueue->encode(input,output);
}

void MainWindow::on_actionGo_triggered()
{
    go();
}

void MainWindow::on_actionStop_triggered()
{
    mainStatusBar->showMessage("Stopping current transcoding...");
    //TODO ask for confirmation
    _renderQueue->stop(6000);
}

void MainWindow::on_actionSettings_triggered(bool checked)
{
    if (checked)
    {
        actionSettings->setIcon(QIcon(":/icons/close-thin"));
        mainStackWidget->setCurrentIndex(1);
    }
    else
    {
        actionSettings->setIcon(QIcon(":/icons/settings"));
        mainStackWidget->setCurrentIndex(0);
    }
}

void MainWindow::reInitCurrentProgress()
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

void MainWindow::maximize(bool max)
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

void MainWindow::maximize()
{
    maximize(!this->isMaximized());
}

void MainWindow::closeEvent(QCloseEvent *event)
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

void MainWindow::dropEvent(QDropEvent *event)
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
                queueWidget->openInputFile(f);
            }
        }

    }
    else if (mimeData->hasText())
    {
        QString f = mimeData->text();
        if (QFile(f).exists())
        {
            queueWidget->openInputFile(f);
        }
    }

    setCursor(Qt::ArrowCursor);
    event->acceptProposedAction();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
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

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
     event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void MainWindow::on_actionBug_report_triggered()
{
    QDesktopServices::openUrl ( QUrl( URL_BUGREPORT ) );
}

void MainWindow::on_actionChat_triggered()
{
    QDesktopServices::openUrl ( QUrl( URL_CHAT ) );
}

void MainWindow::on_actionForum_triggered()
{
    QDesktopServices::openUrl ( QUrl( URL_FORUM ) );
}

void MainWindow::on_actionHelp_triggered()
{
    QDesktopServices::openUrl ( QUrl( URL_DOC ) );
}

void MainWindow::on_actionGoQuit_triggered()
{
    go();
    autoQuit = true;
}

void MainWindow::quit(bool force)
{
    if (force) close();
    else if (!MediaUtils::isBusy( _renderQueue->status() )) close();
}
