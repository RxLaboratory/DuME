#include "outputwidget.h"

OutputWidget::OutputWidget(FFmpeg *ff, PresetManager *pM, int id, MediaList *inputMedias, QWidget *parent) :
    QWidget(parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Output Widget";
#endif
    _freezeUI = true;
    _loadingPreset = false;

    setupUi(this);

    // FFmpeg
    _ffmpeg = ff;
    connect( _ffmpeg,SIGNAL(binaryChanged(QString)),this,SLOT(ffmpeg_init()) );
    // Keep the id
    _index = id;
    // Associated MediaInfo
    _mediaInfo = new MediaInfo( _ffmpeg, this);
    connect( _mediaInfo, SIGNAL(changed()), this, SLOT(mediaInfoChanged()));

    // preset manager
    _presetManager = pM;
    connect( _presetManager, SIGNAL(changed()), this, SLOT(loadPresets()));

    // Input medias
    _inputMedias = inputMedias;
    connect( _inputMedias, SIGNAL( newMedia(MediaInfo*)), this, SLOT( newInputMedia(MediaInfo*)) );

    // CREATE MENUS
    blocksMenu = new QMenu(this);
    blocksMenu->setTearOffEnabled(true);
    addBlockButton->setMenu( blocksMenu );

    presetsMenu = new QMenu(this);
    presetsButton->setMenu( presetsMenu ); 

    // SHADOWS
    videoWidget->setGraphicsEffect( new DropShadow() );
    audioWidget->setGraphicsEffect( new DropShadow() );

    // CREATE BLOCKS
    blocksMenu->addAction(actionVideo);
    blockResize = addBlock( new BlockResize( _mediaInfo ), actionResize );
    blockFrameRate = addBlock( new BlockFrameRate( _mediaInfo ), actionFrameRate );
    blockVideoCodec = addBlock( new BlockVideoCodec( _ffmpeg, _mediaInfo ), actionVideoCodec );
    blockVideoBitrate = addBlock( new BlockVideoBitrate( _mediaInfo ), actionVideoBitrate );
    blockVideoProfile = addBlock( new BlockVideoProfile( _mediaInfo ), actionProfile );
    blockLoops = addBlock( new BlockLoops( _mediaInfo ), actionLoops );
    blockStartNumber = addBlock( new BlockStartNumber( _mediaInfo ), actionStartNumber );
    blockAlpha = addBlock( new BlockAlpha( _mediaInfo ), actionAlpha );
    blockColor = addBlock( new BlockColor( _mediaInfo ), actionColor );
    blockPixFormat = addBlock( new BlockPixFormat( _mediaInfo ), actionPixelFormat );
    blocksMenu->addAction(actionAudio);
    blockSampling = addBlock( new BlockSampling( _mediaInfo ), actionSampling );
    blockAudioCodec = addBlock( new BlockAudioCodec( _ffmpeg, _mediaInfo ), actionAudioCodec );
    blockAudioBitrate = addBlock( new BlockAudioBitrate( _mediaInfo ), actionAudioBitrate );
    blocksMenu->addAction( actionOther );
    blockMap = addBlock( new BlockMapping( _mediaInfo, _inputMedias ), actionMap );
    blocksMenu->addAction( actionAddCustom );

    // PRESETS MENU
    presetsMenu->addAction( actionDefaultPreset );
    presetsMenu->addSeparator();
    presetsMenu->addAction( actionOpenPreset );
    presetsMenu->addAction( actionSavePreset );

    ffmpeg_init();

    _freezeUI = false;

    //Set defaults
    on_presetsFilterBox_activated("");
    //choose preset
    selectDefaultPreset();

    _outputPathIsCustom = false;

    //splitter sizes
    QList<int>sizes;
    sizes << 350;
    sizes << 600;
    splitter->setSizes(sizes);
}

void OutputWidget::ffmpeg_init()
{
    _freezeUI = true;
    _mediaInfo->reInit( false );
    ffmpeg_loadMuxers();
    _freezeUI = false;
}

void OutputWidget::ffmpeg_loadMuxers()
{
    bool uiFreezed = _freezeUI;
    _freezeUI = true;

    formatsBox->clear();

    QList<FFMuxer *> muxers = _ffmpeg->muxers();
    if (muxers.count() == 0)
    {
        _freezeUI = false;
        return;
    }

    int formatsFilter = formatsFilterBox->currentIndex();

    foreach(FFMuxer *muxer,muxers)
    {
        //skip muxers without extension
        if ((formatsFilter != 0 && formatsFilter != 5) && muxer->extensions().count() == 0) continue;
        else if (formatsFilter == 5 && muxer->extensions().count() == 0) formatsBox->addItem(muxer->prettyName(),QVariant(muxer->name()));

        if (formatsFilter == 0 ||
                (formatsFilter == 1 && muxer->isAudio() && muxer->isVideo() ) ||
                (formatsFilter == 2 && muxer->isSequence() ) ||
                (formatsFilter == 3 && muxer->isAudio() && !muxer->isVideo() ) ||
                (formatsFilter == 4 && muxer->isVideo() && !muxer->isAudio() ) )
        {
            formatsBox->addItem("." + muxer->extensions().join(", .") + " | " + muxer->prettyName(),QVariant(muxer->name()));
        }
    }

    formatsBox->setCurrentIndex( -1 );
    _freezeUI = uiFreezed;
}

MediaInfo *OutputWidget::getMediaInfo()
{
    //ADD CUSTOM PARAMS
    _mediaInfo->clearFFmpegOptions( true );
    foreach ( BlockBaseWidget *b, _customParams )
    {
        BlockCustom *bc = static_cast<BlockCustom *>( b->content() );
        QString param = bc->param();

        if (param != "")
        {
            QStringList option(param);
            option << bc->value();
            _mediaInfo->addFFmpegOption(option, true);
        }
    }
    MediaInfo *mi = new MediaInfo( _ffmpeg );

    mi->copyFrom( _mediaInfo, true, true);
    return mi;
}

void OutputWidget::setMediaInfo(MediaInfo *mediaInfo)
{
    if (mediaInfo == nullptr) return;

    _mediaInfo->copyFrom( mediaInfo );
}

void OutputWidget::mediaInfoChanged()
{
    qDebug() << "changed";
    _freezeUI = true;

    //Audio / Video Buttons and Muxer selection
    FFMuxer *m = _mediaInfo->muxer();

    bool audio = m->isAudio() && !m->isSequence();
    audioButton->setEnabled ( audio );

    bool video =  m->isVideo() || m->isSequence();
    videoButton->setEnabled(  video  );

    // select muxer in formats box
    for (int i = 0; i < formatsBox->count(); i++)
    {
        formatsBox->setCurrentData( m->name() );
    }
    if (formatsBox->currentIndex() == -1)
        {
            //try without filter
            formatsFilterBox->setCurrentIndex(0);
            ffmpeg_loadMuxers();
            formatsBox->setCurrentData( m->name() );
        }

    if (m->name() == "")
    {
        audioButton->setEnabled( false );
        videoButton->setEnabled( false );
    }

    audioButton->setChecked( _mediaInfo->hasAudio() );
    audioTranscodeButton->setEnabled( _mediaInfo->hasAudio() );
    audioCopyButton->setEnabled( _mediaInfo->hasAudio() );
    if ( _mediaInfo->hasAudio() )
    {
        bool copy = _mediaInfo->audioStreams()[0]->isCopy();
        audioCopyButton->setChecked( copy );
        audioTranscodeButton->setChecked( !copy );
    }

    videoButton->setChecked( _mediaInfo->hasVideo() );
    videoTranscodeButton->setEnabled( _mediaInfo->hasVideo() );
    videoCopyButton->setEnabled( _mediaInfo->hasVideo() );
    if ( _mediaInfo->hasVideo() )
    {
        bool copy = _mediaInfo->videoStreams()[0]->isCopy();
        videoTranscodeButton->setChecked( !copy );
        videoCopyButton->setChecked( copy );
    }

    //Customs
    //remove previous
    qDeleteAll( _customParams );
    _customParams.clear();
    //add others
    foreach( QStringList option, _mediaInfo->ffmpegOptions() )
    {
        addNewParam( option[0], option[1] );
    }

    //set the preset to custom if we're not loading a preset
    if (!_loadingPreset)
    {
        presetsBox->setCurrentIndex(0);

        actionDefaultPreset->setChecked( false );
        actionDefaultPreset->setText("Set as default preset");
    }
    mediaInfoEdit->setPlainText( _mediaInfo->getDescription() );

    _freezeUI = false;
    qDebug() << "change ok";
}

void OutputWidget::newInputMedia(MediaInfo *m)
{
    connect( m, SIGNAL(changed()), this, SLOT(inputChanged()));
}

void OutputWidget::customParamActivated(bool activated)
{
    if (!activated)
    {
        for ( int i = 0; i < _customParams.count(); i++)
        {
            if (sender() == _customParams[i])
            {
                QWidget *w = _customParams.takeAt(i);
                w->deleteLater();
                return;
            }
        }
    }
}

void OutputWidget::on_videoButton_clicked(bool checked)
{
    if (checked)
    {
        if (!_mediaInfo->hasVideo())
        {
            //only if muxer is capable of video
            FFMuxer *m = _mediaInfo->muxer();
            if (m->name() == "")
            {
                videoButton->setChecked(false);
                videoButton->setEnabled(false);
                videoTranscodeButton->setEnabled( false );
                videoCopyButton->setEnabled( false );
                return;
            }
            if (!m->isVideo() && !m->isSequence())
            {
                videoButton->setChecked(false);
                videoButton->setEnabled(false);
                videoTranscodeButton->setEnabled( false );
                videoCopyButton->setEnabled( false );
                return;
            }
            _mediaInfo->addVideoStream( new VideoInfo(_ffmpeg) );
        }
    }
    else
    {
        _mediaInfo->clearVideoStreams( );
    }
}

void OutputWidget::on_audioButton_clicked(bool checked)
{
    if (checked)
    {
        if (!_mediaInfo->hasAudio())
        {
            //only if muxer is capable of video
            FFMuxer *m = _mediaInfo->muxer();
            if (m->name() == "")
            {
                videoButton->setChecked(false);
                videoButton->setEnabled(false);
                videoTranscodeButton->setEnabled( false );
                videoCopyButton->setEnabled( false );
                return;
            }
            if (!m->isAudio())
            {
                videoButton->setChecked(false);
                videoButton->setEnabled(false);
                videoTranscodeButton->setEnabled( false );
                videoCopyButton->setEnabled( false );
                return;
            }
            _mediaInfo->addAudioStream( new AudioInfo(_ffmpeg) );
        }
    }
    else
    {
        _mediaInfo->clearAudioStreams();
    }
}

void OutputWidget::on_videoTranscodeButton_toggled( bool checked )
{
    if (!_mediaInfo->hasVideo()) return;
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
    if ( checked ) _mediaInfo->videoStreams()[0]->setCodec( "" );
    else _mediaInfo->videoStreams()[0]->setCodec( "copy" );
}

void OutputWidget::on_audioTranscodeButton_toggled( bool checked )
{
    if (!_mediaInfo->hasAudio()) return;
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
    if ( checked ) _mediaInfo->setAudioCodec( "" );
    else _mediaInfo->setAudioCodec( "copy" );
}

void OutputWidget::on_outputBrowseButton_clicked()
{
    QString outputPath = QFileDialog::getSaveFileName(this,"Output file",outputEdit->text());
    if (outputPath == "") return;
    setOutputPath(outputPath);
    _outputPathIsCustom = true;
}

void OutputWidget::on_formatsBox_currentIndexChanged(int index)
{
    if (index == -1) return;
    if (_freezeUI) return;

    _mediaInfo->setMuxer( formatsBox->currentData().toString() );
    setOutputPath(outputEdit->text());
}

void OutputWidget::on_formatsFilterBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    qDebug() << "Format filter selected: " + QString::number(index);
    ffmpeg_loadMuxers();
}

void OutputWidget::on_presetsBox_currentIndexChanged(int index)
{
    if (index == 0) return;
    if (_freezeUI) return;

    _freezeUI = true;
    bool def = presetsBox->itemData(index).toString() == _presetManager->defaultPreset().file().absoluteFilePath();
    if (def)
    {
        actionDefaultPreset->setChecked( true );
        actionDefaultPreset->setText("Default preset");
    }
    else
    {
        actionDefaultPreset->setChecked( false );
        actionDefaultPreset->setText("Set as default preset");
    }
    _freezeUI = false;

    //load
    _loadingPreset = true;
    //load

    _mediaInfo->loadPreset( QFileInfo(presetsBox->itemData(index).toString()) );

    _loadingPreset = false;
}

void OutputWidget::on_presetsFilterBox_activated(QString arg1)
{
    qDebug() << "Preset filter selected: " + arg1;
    loadPresets( );
}

void OutputWidget::setOutputPath(QString outputPath)
{
    if (outputPath == "") return;
    QFileInfo output(outputPath);

    //remove existing {#####}
    QString outputName = output.completeBaseName();
    outputName = outputName.replace(QRegularExpression("_?{#+}"),"");

    QString newExt = "";

    FFMuxer *_currentMuxer = _mediaInfo->muxer();
    //add {#####}

    if (_currentMuxer->isSequence())
    {
        newExt = "_{#####}";
    }

    if (_currentMuxer->extensions().count() > 0)
    {
        newExt += "." + _currentMuxer->extensions()[0];
    }

    QString num = "";
    if (_index > 1) num = "_" + QString::number(_index);
    outputName.replace(QRegularExpression(num + "$"),"");
    outputPath = output.path() + "/" + outputName + num + newExt;
    //check if file exists
    if (QFile(outputPath).exists())
    {
        outputPath = output.path() + "/" + outputName + "_transcoded" + num + newExt;
    }

    outputPath = QDir::toNativeSeparators(outputPath);
    outputEdit->setText( outputPath );
    _mediaInfo->setFileName( outputPath, true );
    emit newLog( "Output path set to: \"" + outputPath + "\"", LogUtils::Debug );
}

void OutputWidget::setOutputPreset(QString preset)
{
    presetsBox->setCurrentData(preset);
    if (presetsBox->currentIndex() < 0) presetsBox->setCurrentText(preset);
}

void OutputWidget::addNewParam(QString name, QString value)
{
    //add a param and a value
    qDebug() << "New Custom param: " + name + " " + value;
    BlockCustom *block = new BlockCustom( _mediaInfo, name, value );
    BlockBaseWidget *bw = new BlockBaseWidget( "FFmpeg parameter", block, blocksWidget );
    blocksLayout->addWidget( bw );
    bw->show();

    connect( bw, SIGNAL(activated(bool)), this, SLOT(customParamActivated(bool)));
    //add to list
    _customParams << bw;
}

void OutputWidget::inputChanged()
{
    MediaInfo *input = static_cast<MediaInfo *>(sender());

    //set output fileName
    if ( outputEdit->text() == "" || !_outputPathIsCustom )
    {
        QFileInfo inputFile(input->fileName());
        QString outputPath = inputFile.path() + "/" + inputFile.completeBaseName();
        setOutputPath(outputPath);
    }

    //If ae render queue
    this->setVisible( !(input->isAep() && input->aeUseRQueue()));
}

void OutputWidget::loadPresets()
{
    if (_freezeUI) return;
    _freezeUI = true;

    presetsBox->clear();
    int index = presetsFilterBox->currentIndex();

    QList<Preset> presets;
    if (index == 0) presets = _presetManager->presets();
    else if (index == 1) presets = _presetManager->internalPresets();
    else if (index == 2) presets = _presetManager->userPresets();

    //add custom
    presetsBox->addItem("Custom");
    foreach(Preset preset, presets)
    {
        presetsBox->addItem(preset.name(), preset.file().absoluteFilePath() );
    }

    _freezeUI = false;
}

void OutputWidget::selectDefaultPreset()
{
    QString defaultPreset = _presetManager->defaultPreset().file().absoluteFilePath();
    presetsBox->setCurrentData( defaultPreset );
    if ( presetsBox->currentIndex() != -1 )
    {
        _freezeUI = true;
        actionDefaultPreset->setChecked(true);
        _freezeUI = false;
    }
}

BlockBaseWidget *OutputWidget::addBlock(BlockContentWidget *content, QAction *action )
{
    // create block
    BlockBaseWidget *b = new BlockBaseWidget( action->text(), content, blocksWidget);
    blocksLayout->addWidget( b );
    //add and connect action
    blocksMenu->addAction( action );
    connect( action, SIGNAL( triggered(bool) ), b, SLOT( activate(bool) ) );
    connect( b, SIGNAL( activated(bool) ), action, SLOT( setChecked( bool ) ) );
    connect( b, SIGNAL( blockEnabled(bool) ), action, SLOT(setVisible(bool)));
    connect(b,  &BlockBaseWidget::hidden, action, [action]{ action->setChecked(false); });
    return b;
}

void OutputWidget::on_actionAddCustom_triggered()
{
    addNewParam();
}

void OutputWidget::on_actionSavePreset_triggered()
{
    _freezeUI = true;

    presetsBox->setCurrentIndex(0);

    //ask for file
    QString saveFileName = QFileDialog::getSaveFileName(this,"Save output preset",settings.value("presets/path",QDir::homePath() + "/DuME Presets/").toString(),"DuME preset (*.meprst);;JSON (*.json);;All Files (*.*)");
    if (saveFileName == "")
    {
        _freezeUI = false;
        return;
    }

    //add extension if none
    if ( QFileInfo(saveFileName).suffix() == "" ) saveFileName += ".meprst";

    //update infos
    getMediaInfo();
    //export
    _mediaInfo->exportPreset(saveFileName);
    //add to box
    _freezeUI = false;
    _presetManager->load();
}

void OutputWidget::on_actionOpenPreset_triggered()
{
    _freezeUI = true;

    presetsBox->setCurrentIndex(0);

    QString openFileName = QFileDialog::getOpenFileName(this,"Load output preset",settings.value("presets/path",QDir::homePath() + "/DuME Presets/").toString(),"DuME preset (*.meprst);;JSON (*.json);;All Files (*.*)");
    if (openFileName == "")
    {
        _freezeUI = false;
        return;
    }
    _loadingPreset = true;
    _freezeUI = false;
    //load
    _mediaInfo->loadPreset(openFileName);
    _loadingPreset = false;
}

void OutputWidget::on_actionDefaultPreset_triggered(bool checked)
{
    if (_freezeUI) return;
    if (checked)
    {
        Preset p = Preset( QFileInfo( presetsBox->currentData().toString() ) );
        _presetManager->setDefaultPreset(p);
        actionDefaultPreset->setText("Default preset");
    }
    else
    {
        _presetManager->resetDefaultPreset();
        actionDefaultPreset->setText("Set as default preset");
    }
}

void OutputWidget::on_outputEdit_textEdited(QString text)
{
    _mediaInfo->setFileName( text, true );
    _outputPathIsCustom = true;
}
