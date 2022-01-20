#include "mainwindow.h"

#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsBlurEffect>
#include "duexr.h"

MainWindow::MainWindow(QStringList args, QWidget *parent) :
    QMainWindow(parent)
{
    qDebug() << "Create Main Window";
    // Build the form
    setupUi(this);

    //populate toolbar
    mainToolBar->addAction(actionShowQueue);
    mainToolBar->addAction(actionTools);
    QMenu *goMenu = new QMenu();
    goMenu->addAction( actionGo );
    goMenu->addAction( actionGoQuit );
    goMenu->addAction( actionLaunchJob );
    goButton = new QToolButton(this);
    goButton->setIcon(actionGo->icon());
    goButton->setText(actionGo->text());
    goButton->setToolTip(actionGo->toolTip());
    goButton->setMenu(goMenu);
    goButton->setPopupMode(QToolButton::MenuButtonPopup);
    mainToolBar->addWidget(goButton);
    mainToolBar->addAction(actionStop);
    mainToolBar->addAction(actionStatus);
#ifdef QT_DEBUG
    mainToolBar->addAction(actionDevBuild);
#endif

    //custom status bar buttons
    ui_consoleButton = new QToolButton(this);
    ui_consoleButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui_consoleButton->setText("");
    ui_consoleButton->setIcon(QIcon(":/icons/console"));
    ui_consoleButton->setCheckable(true);
    mainStatusBar->addPermanentWidget(ui_consoleButton);

    _cacheButton = new QToolButton(this);
    _cacheButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    mainStatusBar->addPermanentWidget(_cacheButton,1);

    // Add default stuff
    duqf_initUi();

    // Complete the help menu
    helpMenu->addAction(actionAbout_FFmpeg);

    log("Initialization");

    // === SETTINGS ===
    log("Init - Loading settings", DuQFLog::Debug);

    //create user presets folder if it does not exist yet
    QDir home = QDir::home();
    home.mkdir("DuME Presets");

    // Load Renderers info (to be passed to other widgets)
    log("Init - Connecting to FFmpeg", DuQFLog::Debug);
    //FFmpeg
    connect( FFmpeg::instance(), SIGNAL( valid(bool) ), this, SLOT( ffmpegValid(bool)) );
    connect( FFmpeg::instance(), SIGNAL( statusChanged(MediaUtils::RenderStatus)), this, SLOT ( ffmpegStatus(MediaUtils::RenderStatus)) );

    // === UI SETUP ===

    log("Init - Adding settings widget", DuQFLog::Debug);

    //settings widget
    ffmpegSettingsWidget = new FFmpegSettingsWidget();
    settingsWidget->addPage(ffmpegSettingsWidget, "FFmpeg", QIcon(":/icons/ffmpeg"));
#ifndef Q_OS_LINUX
    aeSettingsWidget = new AESettingsWidget( this );
    settingsWidget->addPage(aeSettingsWidget, "After Effects", QIcon(":/icons/ae"));
#endif
    ocioSettingsWidget = new OcioSettingsWidget();
    settingsWidget->addPage(ocioSettingsWidget, "OpenColorIO", QIcon(":/icons/ocio"));
    cacheSettingsWidget = new CacheSettingsWidget();
    settingsWidget->addPage(cacheSettingsWidget, "Cache and Memory", QIcon(":/icons/cache"));

    //tools widget
    SettingsWidget *toolsWidget = new SettingsWidget("Tools", this);
    toolsWidget->showReinitButton(false);
    mainStack->addWidget(toolsWidget);
    connect(toolsWidget,&SettingsWidget::closeRequested, this, &MainWindow::closeTools);
    toolsWidget->addPage(new LutBakerWidget(this), "OCIO LUT Baker", QIcon(":/icons/lut-baker"));
    toolsWidget->addPage(new LutConverterWidget(this), "LUT Converter", QIcon(":/icons/lut-settings"));

    log("Init - Adding queue widget", DuQFLog::Debug);

    //queue widget
    queueWidget = new QueueWidget(this);
    queueLayout->addWidget(queueWidget);

    // Cache monitoring
    _cacheButton->setMinimumWidth(100);
    connect(_cacheButton, &QToolButton::clicked, this, &MainWindow::openCacheDir);
    connect(CacheManager::instance(), &CacheManager::cacheSizeChanged, this, &MainWindow::cacheSizeChanged);
    CacheManager::instance()->scan();

    log("Init - Setting default UI items", DuQFLog::Debug);

    // Docks
    ConsoleWidget *consoleWidget = new ConsoleWidget(this);
    ui_consoleDock = new QDockWidget("Console", this);
    ui_consoleDock->setObjectName("consoleDock");
    DuQFDockTitle *ui_consoleTitle = new DuQFDockTitle("Console", this);
    ui_consoleTitle->setObjectName("dockTitle");
    ui_consoleTitle->setIcon(":/icons/console");
    ui_consoleDock->setTitleBarWidget(ui_consoleTitle);
    ui_consoleDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    ui_consoleDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_consoleDock->setWidget( consoleWidget );
    this->addDockWidget(Qt::RightDockWidgetArea, ui_consoleDock);

    RQueueWidget *rQueueWidget = new RQueueWidget(this);
    ui_rQueueDock = new QDockWidget("Queue", this);
    ui_rQueueDock->setObjectName("queueDock");
    DuQFDockTitle *ui_rQueueTitle = new DuQFDockTitle("Queue", this);
    ui_rQueueTitle->setObjectName("dockTitle");
    ui_rQueueTitle->setIcon(":/icons/show-rqueue");
    ui_rQueueDock->setTitleBarWidget(ui_rQueueTitle);
    ui_rQueueDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    ui_rQueueDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_rQueueDock->setWidget( rQueueWidget );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_rQueueDock);

    //init UI
    mainStack->setCurrentIndex(0);
    statusLabel = new QLabel("Ready");
    mainStatusBar->addWidget(statusLabel);

    log("Init - Setting window geometry", DuQFLog::Debug);

    //restore geometry
    // Restore UI state
    settings.beginGroup("ui");
    if (settings.value("maximized", false).toBool() )
        this->showMaximized();
    this->restoreState( settings.value("windowState").toByteArray() );
    settings.endGroup();

    ui_consoleButton->setChecked(ui_consoleDock->isVisible());
    actionShowQueue->setChecked(ui_rQueueDock->isVisible());

    // === FFMPEG ===
    log("Init - FFmpeg (run test)");
    ffmpegValid( FFmpeg::instance()->isValid() );

    // ==== Create RenderQueue ====

    log("Init - Creating render queue");

    renderQueue = RenderQueue::instance();
    connect(renderQueue, SIGNAL( statusChanged(MediaUtils::RenderStatus)), this, SLOT(renderQueueStatusChanged(MediaUtils::RenderStatus)) );
    connect(renderQueue, SIGNAL( newLog( QString, DuQFLog::LogType )), this, SLOT( log( QString, DuQFLog::LogType )) );
    connect(renderQueue, SIGNAL( progress( )), this, SLOT( progress( )) );

    // final connections

    //settings
    connect(ffmpegSettingsWidget,SIGNAL(presetsPathChanged()),queueWidget,SLOT(presetsPathChanged()));

    log("Init - Setting stylesheet", DuQFLog::Debug);

    //parse arguments if ffmpeg is valid
    autoQuit = false;

    if (FFmpeg::instance()->isValid())
    {
        int argc = args.count();
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
                QString arg = args[i];

                if (arg.startsWith("-"))
                {
                    arg = arg.toLower();
                    if ( arg == "--use-queue" ) useQueue = true;
                    else if ( (arg == "--comp" || arg == "-c") && i < argc-1 )
                    {
                        i++;
                        compName = args[i];
                    }
                    else if (arg == "--rq-item" && i < argc-1 )
                    {
                        i++;
                        rqItem = QString(args[i]).toInt();
                    }
                    else if ( (arg == "--framerate" || arg == "-f" ) && i < argc-1 )
                    {
                        i++;
                        framerate = QString(args[i]).toDouble();
                    }
                    else if (arg == "--color-profile" && i < argc-1 )
                    {
                        i++;
                        colorProfile = args[i];
                    }
                    else if ( (arg == "--output" || arg == "-o") && i < argc-1 )
                    {
                        i++;
                        queueWidget->setOutputPath( args[i] );
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
                        queueWidget->setOutputPreset( args[i] );
                    }
                    else if ( arg == "--minimize" || arg == "-m" )
                    {
                        showMinimized();
                    }
#ifndef Q_OS_LINUX
                    else if ( arg == "--aerender" && i < argc-1 )
                    {
                        i++;
                        settings.setValue("aerender/path", args[i]);
                        settings.sync();
                        AfterEffects::instance()->setBinary("Custom");
                        AERenderer::instance()->setUseTemplates(false);
                    }
#endif
                    else if (arg != "--no-banner" && arg != "--hide-console")
                    {
                        log("Unknown argument: " + arg + ". All other arguments will be ignored", DuQFLog::Warning);
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

    // Connect buttons
    connect(actionGo, &QAction::triggered, this, &MainWindow::go);
    connect(rQueueWidget, SIGNAL(launchQueue()), this, SLOT(go()));
    connect( goButton, &QToolButton::clicked, this, &MainWindow::go );

    connect(ui_consoleDock,SIGNAL(visibilityChanged(bool)), ui_consoleButton, SLOT(setChecked(bool)));
    connect(ui_consoleButton,SIGNAL(clicked(bool)), ui_consoleDock, SLOT(setVisible(bool)));
    connect(ui_rQueueDock,SIGNAL(visibilityChanged(bool)), actionShowQueue, SLOT(setChecked(bool)));
    connect(actionShowQueue,SIGNAL(triggered(bool)), ui_rQueueDock, SLOT(setVisible(bool)));

    // Set style
    duqf_setStyle();

    log("Ready!");
}

void MainWindow::duqf_initUi()
{
    // ===== ABOUT ========
    duqf_aboutDialog = new AboutDialog();

    // ===== TOOLBAR ======

    // remove right click on toolbar
    mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    //drag window
    duqf_toolBarClicked = false;
    mainToolBar->installEventFilter(this);

    // ==== TOOLBAR BUTTONS
    mainToolBar->addWidget(new ToolBarSpacer());
    title = new QLabel(STR_FILEDESCRIPTION);
    mainToolBar->addWidget(title);

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
    helpMenu = new QMenu(this);
    if (QString(URL_DOC) != "")
    {
        QAction *docAction = new QAction(QIcon(":/icons/documentation"), "Help");
        docAction->setToolTip("Read the documentation");
        docAction->setShortcut(QKeySequence("F1"));
        helpMenu->addAction(docAction);
        connect(docAction, SIGNAL(triggered()), this, SLOT(duqf_doc()));
    }
    QAction *aboutAction = new QAction(QIcon(":/icons/about"), "About");
    helpMenu->addAction(aboutAction);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(duqf_about()));
    helpMenu->addSeparator();
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

    AppearanceSettingsWidget *asw = new AppearanceSettingsWidget();
    settingsWidget->addPage(asw, "Appearance", QIcon(":/icons/color"));

    // ====== CONNECTIONS ======
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(duqf_settingsButton, SIGNAL(clicked(bool)), this, SLOT(duqf_settings(bool)));
    connect(settingsWidget, SIGNAL(closeRequested()), this, SLOT(duqf_closeSettings()));
    connect(settingsWidget, SIGNAL(reinitRequested()), this, SLOT(duqf_reinitSettings()));
}

void MainWindow::duqf_setStyle()
{
    // ======== STYLE ========
    // Font first so it's available in the css
    DuUI::setFont(settings.value("appearance/font", "Ubuntu").toString());

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

    //and tool buttons
    int styleIndex = settings.value("appearance/toolButtonStyle", 2).toInt();
    DuUI::setToolButtonStyle(styleIndex);
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

void MainWindow::duqf_reinitSettings()
{
    QMessageBox::StandardButton choice = QMessageBox::question(this, "Reset settings", "This will reset all settings to their default values and restart the application.\nAre you sure you want to continue?" );
    if (choice == QMessageBox::Yes)
    {
        settings.clear();
        settings.sync();
        this->close();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
}

void MainWindow::duqf_about()
{
    duqf_aboutDialog->show();
}

void MainWindow::ffmpegValid(bool valid)
{
    if ( valid )
    {
        queuePage->setGraphicsEffect( nullptr );
        queuePage->setEnabled(true);
        setAcceptDrops( true );
        actionStatus->setText( "Ready");
        actionGo->setEnabled( true );
        actionGoQuit->setEnabled(true);
        goButton->setEnabled(true);
    }
    else
    {
        queuePage->setGraphicsEffect( new QGraphicsBlurEffect() );
        log("FFmpeg error", DuQFLog::Critical );
        log( FFmpeg::instance()->lastErrorMessage() );
        queuePage->setEnabled(false);
        setAcceptDrops( false );
        actionStatus->setText( "FFmpeg not found or not working properly. Set its path in the settings.");
        actionGo->setEnabled( false );
        actionGoQuit->setEnabled(false);
        goButton->setEnabled(false);
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
       ffmpegValid( FFmpeg::instance()->isValid() );
    }
}

void MainWindow::progress()
{
    //get input info
    QueueItem *item = renderQueue->currentItem();
    QString filename = "";
    int numFrames = renderQueue->numFrames();
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

    progressBar->setValue( renderQueue->currentFrame() );

    double outputSize = renderQueue->outputSize();
    QString outputSizeText = "";
    if (outputSize < 10)
    {
        outputSizeText = "N/A";
    }
    else if ( outputSize < 10*1024*1024*1024.0 )
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

    double outputBitrate = renderQueue->outputBitrate();
    QString outputBitrateText = "";
    if (outputBitrate < 10)
    {
        outputBitrateText = "N/A";
    }
    else if ( outputBitrate < 10000000)
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

    double expectedSize = renderQueue->expectedSize();
    QString expectedSizeText = "";
    if (expectedSize < 10)
    {
        expectedSizeText = "N/A";
    }
    else if ( expectedSize < 10*1024*1024*1024.0)
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

    float speed = int(renderQueue->encodingSpeed()*100)/100.0;
    if (speed > 0.1)
    {
        speedTitleLabel->setText("Speed: ");
        speedLabel->setText( QString::number(speed) + "x");
    }
    else
    {
        speedTitleLabel->setText("Frame: ");
        speedLabel->setText(QString::number(renderQueue->currentFrame()) + " / " + QString::number(numFrames));
    }

    timeRemainingLabel->setText( renderQueue->remainingTime().toString("hh:mm:ss"));

    timeLabel->setText( renderQueue->elapsedTime().toString("hh:mm:ss") );

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
        actionGoQuit->setEnabled(false);
        goButton->setEnabled(false);
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
        actionGoQuit->setEnabled(true);
        goButton->setEnabled(true);
        actionStop->setEnabled(false);
        mainStatusBar->clearMessage();
        setCursor(Qt::ArrowCursor);

        if ( autoQuit )
        {
            // Quit after a few seconds to be sure we're finished
            QTimer::singleShot(2000, this, SLOT(close()));
            // Display a warning
            actionStatus->setText("About to quit. See you soon!");
        }
    }
}
void MainWindow::cacheSizeChanged(qint64 size)
{
    _cacheButton->setText( "Cache: " + MediaUtils::sizeString(size));
}

void MainWindow::openCacheDir()
{
    FileUtils::openInExplorer(CacheManager::instance()->getRootCacheDir().absolutePath());
}

void MainWindow::log(QString log, DuQFLog::LogType type)
{
    Q_UNUSED(type)
    //status bar
    mainStatusBar->showMessage(log);
}

void MainWindow::closeTools()
{
    mainStack->setCurrentIndex(0);
    actionTools->setChecked(false);
    on_actionTools_triggered(false);
}

void MainWindow::go()
{
    //Launch!
    log("=== Beginning encoding ===");
    renderQueue->encode( queueWidget->job() );
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
    renderQueue->stop(6000);
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
    renderQueue->stop(6000);

    qDebug() << "Saving geometry and settings...";

    //save ui geometry
    // Let's save the ui state
    settings.beginGroup("ui");
    settings.setValue("maximized", this->isMaximized());
    settings.setValue("windowState", this->saveState());
    settings.endGroup();

    qDebug() << "Purging disk cache...";

    //remove temp folders
    CacheManager::instance()->purgeCache();

    //remove app fonts
    QFontDatabase::removeAllApplicationFonts();

    QMainWindow::closeEvent(event);
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
        if (this->isMaximized()) this->showNormal();
        else this->showMaximized();
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
    if (force || !MediaUtils::isBusy( renderQueue->status() )) close();
}

void MainWindow::on_actionAbout_FFmpeg_triggered()
{
    QDesktopServices::openUrl ( QUrl( "https://ffmpeg.org/" ) );
}

void MainWindow::on_actionTools_triggered(bool checked)
{
    if (checked)
    {
        mainStack->setCurrentIndex(2);
        actionShowQueue->setChecked(false);
    }
    else
    {
        mainStack->setCurrentIndex(0);
        actionShowQueue->setChecked( settings.value("rQueueVisible", false).toBool() );
    }
}
