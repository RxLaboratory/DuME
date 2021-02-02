#include "inputwidget.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

InputWidget::InputWidget(int id, QWidget *parent) :
    QWidget(parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Input Widget";
#endif
    setupUi(this);

    _mediaInfo = new MediaInfo(this);
    _mediaInfo->setId(id);
    connect ( _mediaInfo, SIGNAL(changed()), this, SLOT (updateOptions()));

    // CREATE MENUS
    blocksMenu = new QMenu(this);
    blocksMenu->setTearOffEnabled(true);
    addBlockButton->setMenu( blocksMenu );

    // CREATE BLOCKS
    blockTimeRangeContent = new BlockTimeRange( _mediaInfo );
    blockTimeRange = addBlock( blockTimeRangeContent, actionTimeRange, ":/icons/time-range" );
    blockFrameRateContent = new BlockFrameRate( _mediaInfo );
    blockFrameRate = addBlock( blockFrameRateContent, actionFramerate, ":/icons/framerate" );
    blockColorContent = new BlockColor( _mediaInfo );
    blockColor = addBlock( blockColorContent, actionColor, ":/icons/color" );
    blockEXRContent = new BlockEXR( _mediaInfo );
    blockEXR = addBlock( blockEXRContent, actionEXR, ":/icons/raw-file" );
    blockAECompContent = new BlockAEComp( _mediaInfo );
    blockAEComp = addBlock( blockAECompContent, actionAfter_Effects_composition, ":/icons/ae-comp");
    blockAEThreadsContent = new BlockAEThreads( _mediaInfo );
    blockAEThreads = addBlock( blockAEThreadsContent, actionAfter_Effects_threads, ":/icons/ae-threads");
    blockCustomContent = new BlockCustom( _mediaInfo );
    blockCustom = addBlock( blockCustomContent, actionAddCustom, ":/icons/add-custom");

    //splitter sizes
    QList<int>sizes;
    sizes << 500;
    sizes << 500;
    splitter->setSizes(sizes);

    updateOptions();
}

MediaInfo *InputWidget::mediaInfo()
{
    return _mediaInfo;
}

void InputWidget::openFile(QString file)
{
    QSettings settings;
    if (file == "") return;
    file = QDir::toNativeSeparators( file );

    QFileInfo fileInfo(file);
    _mediaInfo->update( fileInfo );

    //keep in settings
    settings.setValue("input/path", fileInfo.path() );

    inputEdit->setText( _mediaInfo->fileName() );

    updateOptions();
}

void InputWidget::openFile(QUrl file)
{
    openFile(file.toLocalFile());
}

bool InputWidget::hasMedia()
{
    return inputEdit->text() == "";
}

void InputWidget::on_inputBrowseButton_clicked()
{
    QSettings settings;
    QString inputPath = QFileDialog::getOpenFileName(this,"Select the media file to transcode",settings.value("input/path","").toString());
    if (inputPath == "") return;
    openFile(inputPath);
}

void InputWidget::on_inputEdit_editingFinished()
{
    //check if file exists, try to read url
    QUrl test(inputEdit->text());
    if (!test.isEmpty())
    {
        if (test.isValid())
        {
            inputEdit->setText(test.toLocalFile());
        }
    }

    openFile(inputEdit->text());
}

BlockBaseWidget *InputWidget::addBlock(BlockContentWidget *content, QAction *action, QString icon)
{
    // create block
    BlockBaseWidget *b = new BlockBaseWidget( action->text(), content, icon, blocksWidget);
    blocksLayout->addWidget( b );
    //add and connect action
    blocksMenu->addAction( action );
    connect( action, SIGNAL( triggered(bool) ), b, SLOT( setVisible(bool) ) );
    connect( b, SIGNAL( activated(bool) ), action, SLOT( setChecked( bool ) ) );
    connect( b, SIGNAL( blockEnabled(bool) ), action, SLOT(setVisible(bool)));
    connect(b,  &BlockBaseWidget::hidden, action, [action]{ action->setChecked(false); });

    return b;
}

void InputWidget::updateOptions()
{
    // framerate buttons
    if (_mediaInfo->isSequence() || _mediaInfo->isAep())
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
    if (_mediaInfo->isAep())
    {
        actionAfter_Effects_composition->setVisible(true);
        blockAEComp->show();
        actionAfter_Effects_threads->setVisible(true);
    }
    else
    {
        actionAfter_Effects_composition->setVisible(false);
        blockAEComp->hide();
        actionAfter_Effects_threads->setVisible(false);
        blockAEThreads->hide();
    }

    //Color options
    if ( _mediaInfo->hasVideo() )
    {
        VideoInfo *stream = _mediaInfo->videoStreams()[0];
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

void InputWidget::updateInfo()
{
    QString mediaInfoString = "Media information\n\n";
    mediaInfoString += _mediaInfo->getDescription();

    mediaInfosText->setText(mediaInfoString);
}

