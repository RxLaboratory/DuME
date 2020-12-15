#include "mainwindow.h"

#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsBlurEffect>
#include "duexr.h"

MainWindow::MainWindow(int argc, char *argv[], QWidget *parent) :
    QMainWindow(parent)
{
    qDebug() << "Create Main Window";
    // Build the form
    setupUi(this);

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

    // Add default stuff
    duqf_initUi();

    log("Initialization");

    // === SETTINGS ===
    log("Init - Loading settings");

    //create user presets folder if it does not exist yet
    QDir home = QDir::home();
    home.mkdir("DuME Presets");

    // Load Renderers info (to be passed to other widgets)
    log("Init - Connecting to FFmpeg");
    //FFmpeg
    connect( ffmpeg, SIGNAL( newLog(QString, LogUtils::LogType) ),this,SLOT( ffmpegLog(QString, LogUtils::LogType)) );
    connect( ffmpeg, SIGNAL( console(QString)), this, SLOT( ffmpegConsole(QString)) );
    connect( ffmpeg, SIGNAL( valid(bool) ), this, SLOT( ffmpegValid(bool)) );
    connect( ffmpeg, SIGNAL( statusChanged(MediaUtils::RenderStatus)), this, SLOT ( ffmpegStatus(MediaUtils::RenderStatus)) );
    //After Effects
    log("Init - Initializing the After Effects renderer");
    _ae = new AfterEffects( this );
    log("Init - Connecting to After Effects");
    connect( _ae, SIGNAL( newLog(QString, LogUtils::LogType) ), this, SLOT( aeLog(QString, LogUtils::LogType )) );
    connect( _ae, SIGNAL( console(QString)), this, SLOT( aeConsole(QString)) );

    // === UI SETUP ===

    log("Init - Adding settings widget");

    //settings widget
    ffmpegSettingsWidget = new FFmpegSettingsWidget();
    settingsWidget->addPage(ffmpegSettingsWidget, "FFmpeg", QIcon(":/icons/ffmpeg"));
    aeSettingsWidget = new AESettingsWidget(_ae);
    settingsWidget->addPage(aeSettingsWidget, "After Effects", QIcon(":/icons/ae"));


    log("Init - Adding queue widget");

    //queue widget
    queueWidget = new QueueWidget(this);
    queueLayout->addWidget(queueWidget);

    log("Init - Setting default UI items");

    //init UI
    consoleTabs->setCurrentIndex(0);
    mainStack->setCurrentIndex(0);
    statusLabel = new QLabel("Ready");
    mainStatusBar->addWidget(statusLabel);
    versionLabel->setText(qApp->applicationName() + " | version: " + qApp->applicationVersion());

    log("Init - Setting window geometry");

    //restore geometry
    settings.beginGroup("mainwindow");
    //size
    resize(settings.value("size", QSize(850, 850)).toSize());
    //position
    //move(settings.value("pos", QPoint(200, 200)).toPoint());
    //maximized
#ifndef Q_OS_MAC
    duqf_maximize( settings.value("maximized",false).toBool() );
#endif
    settings.endGroup();
    //console splitter sizes
    settings.beginGroup("consolesplitter");
    QList<int>sizes;
    sizes << settings.value("consolesize",0).toInt();
    sizes << settings.value("queuesize",100).toInt();
    consoleSplitter->setSizes(sizes);
    settings.endGroup();

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

    //settings
    connect(ffmpegSettingsWidget,SIGNAL(presetsPathChanged()),queueWidget,SLOT(presetsPathChanged()));

    //QueueWidget
    connect(queueWidget,SIGNAL(newLog(QString,LogUtils::LogType)),this,SLOT(log(QString,LogUtils::LogType)));

    log("Init - Setting stylesheet");
    // Set style
    duqf_setStyle();

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
                    if ( arg == "--use-queue" ) useQueue = true;
                    else if ( (arg == "--comp" || arg == "-c") && i < argc-1 )
                    {
                        i++;
                        compName = argv[i];
                    }
                    else if (arg == "--rq-item" && i < argc-1 )
                    {
                        i++;
                        rqItem = QString(argv[i]).toInt();
                    }
                    else if ( (arg == "--framerate" || arg == "-f" ) && i < argc-1 )
                    {
                        i++;
                        framerate = QString(argv[i]).toDouble();
                    }
                    else if (arg == "--color-profile" && i < argc-1 )
                    {
                        i++;
                        colorProfile = argv[i];
                    }
                    else if ( (arg == "--output" || arg == "-o") && i < argc-1 )
                    {
                        i++;
                        queueWidget->setOutputPath( argv[i] );
                    }
                    else if ( arg == "--autostart" )
                    {
                        autoStart = true;
                    }
                    else if ( arg == "--autoquit" )
                    {
                        autoQuit = true;
                    }
                    else if ( (arg == "--preset" || arg == "-p") && i < argc-1 )
                    {
                        i++;
                        queueWidget->setOutputPreset( argv[i] );
                    }
#ifndef Q_OS_LINUX
                    else if ( arg == "--aerender" && i < argc-1 )
                    {
                        i++;
                        settings.setValue("aerender/path", argv[i]);
                        settings.sync();
                        _ae->setBinary("Custom");
                    }
#endif
                    else if (arg != "--no-banner" && arg != "--hide-console")
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

void MainWindow::duqf_initUi()
{
    // ===== TOOLBAR ======

    // remove right click on toolbar
    mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    // set frameless
#ifndef Q_OS_MAC
    this->setWindowFlags(Qt::FramelessWindowHint);
#endif
    //drag window
    duqf_toolBarClicked = false;
    mainToolBar->installEventFilter(this);

    // ==== TOOLBAR BUTTONS
    mainToolBar->addWidget(new ToolBarSpacer());
    title = new QLabel(STR_FILEDESCRIPTION);
    mainToolBar->addWidget(title);
    //minimize
#ifndef Q_OS_MAC
    QToolButton *minimizeButton = new QToolButton();
    minimizeButton->setIcon(QIcon(":/icons/minimize"));
    minimizeButton->setObjectName("windowButton");
    mainToolBar->addWidget(minimizeButton);
#endif
    //maximize
    duqf_maximizeButton = new QToolButton(this);
    duqf_maximizeButton->setIcon(QIcon(":/icons/maximize"));
    duqf_maximizeButton->setObjectName("windowButton");
    mainToolBar->addWidget(duqf_maximizeButton);
    //quit
    QToolButton *quitButton = new QToolButton(this);
    quitButton->setIcon(QIcon(":/icons/quit"));
    quitButton->setObjectName("windowButton");
    mainToolBar->addWidget(quitButton);

    // ===== STATUSBAR ======

    // version in statusbar
    mainStatusBar->addPermanentWidget(new QLabel("v" + QString(STR_VERSION)));
    duqf_settingsButton = new QToolButton();
    duqf_settingsButton->setIcon(QIcon(":/icons/settings"));
    duqf_settingsButton->setToolTip("Go to Settings");
    duqf_settingsButton->setCheckable(true);
    mainStatusBar->addPermanentWidget(duqf_settingsButton);
    QToolButton *helpButton = new QToolButton();
    helpButton->setIcon(QIcon(":/icons/help"));
    helpButton->setToolTip("Get Help");
    helpButton->setPopupMode( QToolButton::InstantPopup );
    QMenu *helpMenu = new QMenu(this);
    if (QString(URL_DOC) != "")
    {
        QAction *docAction = new QAction(QIcon(":/icons/documentation"), "Help");
        docAction->setToolTip("Read the documentation");
        docAction->setShortcut(QKeySequence("F1"));
        helpMenu->addAction(docAction);
        helpMenu->addSeparator();
        connect(docAction, SIGNAL(triggered()), this, SLOT(duqf_doc()));
    }
    bool chat = QString(URL_CHAT) != "";
    bool bugReport = QString(URL_BUGREPORT) != "";
    bool forum = QString(URL_FORUM) != "";
    if (bugReport)
    {
        QAction *bugReportAction = new QAction(QIcon(":/icons/bug-report"), "Bug Report");
        bugReportAction->setToolTip("Report a bug");
        helpMenu->addAction(bugReportAction);
        if (!chat && !forum) helpMenu->addSeparator();
        connect(bugReportAction, SIGNAL(triggered()), this, SLOT(duqf_bugReport()));
    }
    if (chat)
    {
        QAction *chatAction = new QAction(QIcon(":/icons/chat"), "Chat");
        chatAction->setToolTip("Come and have a chat");
        helpMenu->addAction(chatAction);
        if (!forum) helpMenu->addSeparator();
        connect(chatAction, SIGNAL(triggered()), this, SLOT(duqf_chat()));
    }
    if (forum)
    {
        QAction *forumAction = new QAction(QIcon(":/icons/forum"), "Forum");
        forumAction->setToolTip("Join us on our forum");
        helpMenu->addAction(forumAction);
        helpMenu->addSeparator();
        connect(forumAction, SIGNAL(triggered()), this, SLOT(duqf_forum()));
    }
    QAction *aboutQtAction = new QAction(QIcon(":/icons/qt"), "About Qt");
    helpMenu->addAction(aboutQtAction);

    helpButton->setMenu(helpMenu);
    mainStatusBar->addPermanentWidget(helpButton);

    // ========= SETTINGS ========

    settingsWidget = new SettingsWidget();
    duqf_settingsLayout->addWidget(settingsWidget);
    duqf_closeSettingsButton->setObjectName("windowButton");

    AppearanceSettingsWidget *asw = new AppearanceSettingsWidget();
    settingsWidget->addPage(asw, "Appearance", QIcon(":/icons/color"));

    // ====== CONNECTIONS ======
    connect(duqf_maximizeButton,SIGNAL(clicked()),this,SLOT(duqf_maximize()));
    connect(minimizeButton,SIGNAL(clicked()),this,SLOT(showMinimized()));
    connect(quitButton,SIGNAL(clicked()),this,SLOT(close()));

    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(duqf_settingsButton, SIGNAL(clicked(bool)), this, SLOT(duqf_settings(bool)));
    connect(duqf_closeSettingsButton, SIGNAL(clicked()), this, SLOT(duqf_closeSettings()));
}

void MainWindow::duqf_setStyle()
{
    // ======== STYLE ========

    //Re-set StyleSheet
    QString cssFile = settings.value("appearance/cssFile", ":/styles/default").toString();
    QString style = settings.value("appearance/style","Default").toString();
    if (cssFile != "")
    {
        DuUI::updateCSS(cssFile);
    }
    else
    {
        DuUI::updateCSS("");
        qApp->setStyle(QStyleFactory::create(style));
    }
    //and font
    DuUI::setFont(settings.value("appearance/font", "Ubuntu").toString());
    //and tool buttons
    int styleIndex = settings.value("appearance/toolButtonStyle", 2).toInt();
    DuUI::setToolButtonStyle(styleIndex);
}

void MainWindow::duqf_maximize(bool max)
{
    if (!max)
    {
        duqf_maximizeButton->setIcon(QIcon(":/icons/maximize"));
        this->showNormal();
    }
    else
    {
        duqf_maximizeButton->setIcon(QIcon(":/icons/unmaximize"));
        this->showMaximized();
    }
}

void MainWindow::duqf_maximize()
{
    duqf_maximize(!this->isMaximized());
}

void MainWindow::duqf_bugReport()
{
    QDesktopServices::openUrl ( QUrl( URL_BUGREPORT ) );
}

void MainWindow::duqf_forum()
{
    QDesktopServices::openUrl ( QUrl( URL_FORUM ) );
}

void MainWindow::duqf_chat()
{
    QDesktopServices::openUrl ( QUrl( URL_CHAT ) );
}

void MainWindow::duqf_doc()
{
    QDesktopServices::openUrl ( QUrl( URL_DOC ) );
}

void MainWindow::duqf_settings(bool checked)
{
    duqf_settingsButton->setChecked(checked);
    if (checked)
    {
        mainStack->setCurrentIndex(1);
    }
    else
    {
        mainStack->setCurrentIndex(0);
    }
}

void MainWindow::duqf_closeSettings()
{
    duqf_settings(false);
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

    speedLabel->setText( QString::number(int(_renderQueue->encodingSpeed()*100)/100.0) + "x");

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
#ifdef QT_DEBUG
    qDebug() << log;
#endif
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

void MainWindow::on_actionGoQuit_triggered()
{
    go();
    autoQuit = true;
}

void MainWindow::on_actionStop_triggered()
{
    mainStatusBar->showMessage("Stopping current transcoding...");
    //TODO ask for confirmation
    _renderQueue->stop(6000);
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    _renderQueue->stop(6000);
    //save ui geometry
    settings.beginGroup("mainwindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("maximized",this->isMaximized());
    settings.endGroup();
    settings.beginGroup("consolesplitter");
    settings.setValue("consolesize",consoleSplitter->sizes()[0]);
    settings.setValue("queuesize",consoleSplitter->sizes()[1]);
    settings.endGroup();
    settings.sync();

    //remove temp folders
    cacheManager->purgeCache();

    event->accept();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = (QMouseEvent*)event;
        if (mouseEvent->button() == Qt::LeftButton)
        {
          duqf_toolBarClicked = true;
          duqf_dragPosition = mouseEvent->globalPos() - this->frameGeometry().topLeft();
          event->accept();
        }
        return true;
    }
    else if (event->type() == QEvent::MouseMove)
    {
        if (this->isMaximized()) return false;
      QMouseEvent *mouseEvent = (QMouseEvent*)event;
      if (mouseEvent->buttons() & Qt::LeftButton && duqf_toolBarClicked)
      {
          this->move(mouseEvent->globalPos() - duqf_dragPosition);
          event->accept();
      }
      return true;
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        duqf_toolBarClicked = false;
        return true;
    }
  #ifndef Q_OS_MAC
    else if (event->type() == QEvent::MouseButtonDblClick)
    {
        duqf_maximize();
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

void MainWindow::quit(bool force)
{
    if (force || !MediaUtils::isBusy( _renderQueue->status() )) close();
}
