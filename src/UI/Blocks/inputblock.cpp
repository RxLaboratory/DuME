#include "inputblock.h"

InputBlock::InputBlock(MediaInfo *mediaInfo, QWidget *parent)
    : QWidget{parent}
{
    m_mediaInfo = mediaInfo;
    setupUi();
    connectEvents();

    this->setAcceptDrops(true);
}

void InputBlock::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(0,0,0,0);

    ui_fileSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::File, this);
    ui_fileSelector->setDialogTitle("Select input media...");
    ui_fileSelector->setPlaceHolderText("Input file...");
    mainLayout->addWidget(ui_fileSelector);

    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);

    mainLayout->addWidget(splitter);

    ui_mediaInfosText = new QTextBrowser(splitter);

    splitter->addWidget(ui_mediaInfosText);

    QWidget *optionsWidget = new QWidget(splitter);
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsWidget);
    optionsLayout->setSpacing(3);
    optionsLayout->setContentsMargins(0,0,0,0);

    splitter->addWidget(optionsWidget);

    ui_addBlockButton = new QToolButton(optionsWidget);
    ui_addBlockButton->setIcon(QIcon(":/icons/list"));
    ui_addBlockButton->setPopupMode(QToolButton::InstantPopup);

    ui_blockMenu = new QMenu(ui_addBlockButton);
    ui_addBlockButton->setMenu(ui_blockMenu);

    optionsLayout->addWidget(ui_addBlockButton);

    QScrollArea *optionsArea = new QScrollArea(optionsWidget);
    optionsArea->setFrameShape(QFrame::NoFrame);
    optionsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    optionsArea->setWidgetResizable(true);

    ui_optionsContentWidget = new QWidget();

    ui_optionsContentLayout = new QVBoxLayout(ui_optionsContentWidget);
    ui_optionsContentLayout->setSpacing(3);
    ui_optionsContentLayout->setContentsMargins(0,3,3,0);

    optionsArea->setWidget(ui_optionsContentWidget);

    optionsLayout->addWidget(optionsArea);

    // ADD BLOCKS AND ACTIONS

    actionTimeRange = new QAction(this);
    actionTimeRange->setCheckable(true);
    actionTimeRange->setIcon(QIcon(":/icons/set-time-range"));
    actionTimeRange->setText("Time range");
    actionTimeRange->setToolTip("Select a specific time range for encoding");
    actionTimeRange->setShortcut(QKeySequence("T"));
    blockTimeRange = addBlock( new BlockTimeRange( m_mediaInfo ), actionTimeRange, ":/icons/time-range" );

    actionFramerate = new QAction(this);
    actionFramerate->setCheckable(true);
    actionFramerate->setIcon(QIcon(":/icons/set-framerate"));
    actionFramerate->setText("Set Frame rate");
    actionFramerate->setToolTip("Override the input frame rate");
    actionFramerate->setShortcut(QKeySequence("F"));
    blockFrameRate = addBlock( new BlockFrameRate( m_mediaInfo ), actionFramerate, ":/icons/framerate" );

    actionColor = new QAction(this);
    actionColor->setCheckable(true);
    actionColor->setIcon(QIcon(":/icons/set-color"));
    actionColor->setText("Interpret Colors");
    actionColor->setToolTip("Options to interpret the colors of the footage");
    actionColor->setShortcut(QKeySequence("C"));
    blockColor = addBlock( new BlockColorInput( m_mediaInfo ), actionColor, ":/icons/color" );

    actionEXR = new QAction(this);
    actionEXR->setCheckable(true);
    actionEXR->setIcon(QIcon(":/icons/raw-options"));
    actionEXR->setText("EXR options");
    actionEXR->setToolTip("Set specific options to EXR format");
    actionEXR->setShortcut(QKeySequence("E"));
    blockEXR = addBlock( new BlockEXR( m_mediaInfo ), actionEXR, ":/icons/raw-file" );

    actionAfter_Effects_composition = new QAction(this);
    actionAfter_Effects_composition->setCheckable(true);
    actionAfter_Effects_composition->setIcon(QIcon(":/icons/set-ae-comp"));
    actionAfter_Effects_composition->setText("After Effects composition");
    actionAfter_Effects_composition->setToolTip("Select the composition to render");
    actionAfter_Effects_composition->setShortcut(QKeySequence("A"));
    blockAEComp = addBlock( new BlockAEComp( m_mediaInfo ), actionAfter_Effects_composition, ":/icons/ae-comp" );

    actionAddCustom = new QAction(this);
    actionAddCustom->setCheckable(true);
    actionAddCustom->setIcon(QIcon(":/icons/add-custom"));
    actionAddCustom->setText("Add FFmpeg parameter");
    actionAddCustom->setToolTip("Adds a custom FFmpeg parameter for this input file");
    actionAddCustom->setShortcut(QKeySequence("Z"));
    blockCustom = addBlock( new BlockCustom( m_mediaInfo ), actionAddCustom, ":/icons/add-customdd-custom" );

    ui_optionsContentLayout->addStretch();

    //splitter sizes
    QList<int>sizes;
    sizes << 500;
    sizes << 500;
    splitter->setSizes(sizes);
}

void InputBlock::connectEvents()
{
    connect ( m_mediaInfo, SIGNAL(changed()), this, SLOT (updateOptions()));

    connect(ui_fileSelector, SIGNAL(pathChanged(QString)), this, SLOT(openFile(QString)));
}

BlockBaseWidget *InputBlock::addBlock(BlockContentWidget *content, QAction *action, QString icon)
{
    // create block
    BlockBaseWidget *b = new BlockBaseWidget( action->text(), content, icon, ui_optionsContentWidget);
    ui_optionsContentLayout->addWidget( b );
    //add and connect action
    ui_blockMenu->addAction( action );
    connect( action, SIGNAL( triggered(bool) ), b, SLOT( setVisible(bool) ) );
    connect( b, SIGNAL( activated(bool) ), action, SLOT( setChecked(bool) ) );
    connect( b, SIGNAL( blockEnabled(bool) ), action, SLOT(setVisible(bool)));
    connect(b,  &BlockBaseWidget::hidden, action, [action]{ action->setChecked(false); });

    return b;
}

MediaInfo *InputBlock::mediaInfo() const
{
    return m_mediaInfo;
}

void InputBlock::openFile(QString file)
{
    QSignalBlocker b(ui_fileSelector);

    QSettings settings;
    if (file == "") return;
    file = QDir::toNativeSeparators( file );

    QFileInfo fileInfo(file);
    m_mediaInfo->update( fileInfo );

    //keep in settings
    settings.setValue("input/path", fileInfo.path() );

    ui_fileSelector->setPath( m_mediaInfo->fileName() );

    updateOptions();
}

void InputBlock::openFile(QUrl file)
{
    openFile(file.toLocalFile());
}

void InputBlock::dropEvent(QDropEvent *event)
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
                openFile(f);
            }
        }

    }
    else if (mimeData->hasText())
    {
        QString f = mimeData->text();
        if (QFile(f).exists())
        {
            openFile(f);
        }
    }

    setCursor(Qt::ArrowCursor);
    event->acceptProposedAction();
}

void InputBlock::dragEnterEvent(QDragEnterEvent *event)
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

void InputBlock::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void InputBlock::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void InputBlock::updateInfo()
{
    QString mediaInfoString = "Media information\n\n";
    mediaInfoString += m_mediaInfo->getDescription();

    ui_mediaInfosText->setText(mediaInfoString);
}

void InputBlock::updateOptions()
{
    // framerate buttons
    if (m_mediaInfo->isSequence() || m_mediaInfo->isAep())
    {
        actionFramerate->setVisible( true );
        blockFrameRate->show();
    }
    else
    {
        actionFramerate->setVisible( false );
        blockFrameRate->hide();
    }

    // After Effects options
    if (m_mediaInfo->isAep())
    {
        actionAfter_Effects_composition->setVisible(true);
        blockAEComp->show();
    }
    else
    {
        actionAfter_Effects_composition->setVisible(false);
        blockAEComp->hide();
    }

    //Color options
    if ( m_mediaInfo->hasVideo() )
    {
        VideoInfo *stream = m_mediaInfo->videoStreams()[0];
        if ( stream->colorTRC()->name() != "" || stream->colorPrimaries()->name() != "" || stream->colorRange()->name() != "" || stream->colorSpace()->name() != "")
        {
            blockColor->show();
        }
        else
        {
            blockColor->hide();
        }
    }

    updateInfo();
}
