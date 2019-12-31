#include "uioutputwidget.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

UIOutputWidget::UIOutputWidget(FFmpeg *ff, int id, QWidget *parent) :
    QWidget(parent)
{
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
    connect( _mediaInfo, SIGNAL(changed()), this, SLOT(updateBlocksAvailability()));

    // CREATE MENUS

    blocksMenu = new QMenu(this);
    blocksMenu->setTearOffEnabled(true);
    addBlockButton->setMenu( blocksMenu );

    // SHADOWS
    videoWidget->setGraphicsEffect( new UIDropShadow() );
    audioWidget->setGraphicsEffect( new UIDropShadow() );

    // CREATE BLOCKS
    blocksMenu->addAction(actionVideo);
    blockResizeContent = new BlockResize( _mediaInfo );
    blockResize = addBlock( blockResizeContent, actionResize );
    blockFrameRateContent = new BlockFrameRate( _mediaInfo );
    blockFrameRate = addBlock( blockFrameRateContent, actionFrameRate );
    blockVideoCodecContent = new BlockVideoCodec( _ffmpeg, _mediaInfo );
    blockVideoCodec = addBlock( blockVideoCodecContent, actionVideoCodec );
    blockVideoBitrateContent = new BlockVideoBitrate( _mediaInfo );
    blockVideoBitrate = addBlock( blockVideoBitrateContent, actionVideoBitrate );
    blockVideoProfileContent = new BlockVideoProfile( _mediaInfo );
    blockVideoProfile = addBlock( blockVideoProfileContent, actionProfile );
    blockLoopsContent = new BlockLoops( _mediaInfo );
    blockLoops = addBlock( blockLoopsContent, actionLoops );
    blockStartNumberContent = new BlockStartNumber( _mediaInfo );
    blockStartNumber = addBlock( blockStartNumberContent, actionStartNumber );
    blockAlphaContent = new BlockAlpha( _mediaInfo );
    blockAlpha = addBlock( blockAlphaContent, actionAlpha );
    blockPixFormatContent = new BlockPixFormat( _mediaInfo );
    blockPixFormat = addBlock( blockPixFormatContent, actionPixelFormat );
    blocksMenu->addAction(actionAudio);
    blockSamplingContent = new BlockSampling( _mediaInfo );
    blockSampling = addBlock( blockSamplingContent, actionSampling );
    blockAudioCodecContent = new BlockAudioCodec( _ffmpeg, _mediaInfo );
    blockAudioCodec = addBlock( blockAudioCodecContent, actionAudioCodec );
    blockAudioBitrateContent = new BlockAudioBitrate( _mediaInfo );
    blockAudioBitrate = addBlock( blockAudioBitrateContent, actionAudioBitrate );

    init();

    ffmpeg_init();

    _freezeUI = false;

    //Set defaults
    on_presetsFilterBox_activated();
}

void UIOutputWidget::init()
{
    //main
    formatsFilterBox->setCurrentText("");
    formatsBox->setCurrentIndex(0);

    //video
    videoTranscodeButton->setChecked(true);
    blockResize->hide();
    blockFrameRate->hide();
    blockVideoCodec->hide();
    blockVideoBitrate->hide();
    blockVideoProfile->hide();
    blockLoops->hide();

    //audio
    audioTranscodeButton->setChecked(true);
    blockSampling->hide();
    blockAudioCodec->hide();
    blockAudioBitrate->hide();

    //params
    qDeleteAll(_customParamEdits);
    qDeleteAll(_customValueEdits);
    _customParamEdits.clear();
    _customValueEdits.clear();
}

void UIOutputWidget::ffmpeg_init()
{
    _freezeUI = true;
    _mediaInfo->reInit( false );
    ffmpeg_loadMuxers();
    _freezeUI = false;
}

void UIOutputWidget::ffmpeg_loadMuxers()
{
    _freezeUI = true;

    formatsBox->clear();

    QList<FFMuxer *> muxers = _ffmpeg->muxers();
    if (muxers.count() == 0) return;

    int formatsFilter = formatsFilterBox->currentIndex();

    foreach(FFMuxer *muxer,muxers)
    {
        //skip muxers without extension
        if ((formatsFilter != 0 && formatsFilter != 5) && muxer->extensions().count() == 0) continue;
        else if (formatsFilter == 5 && muxer->extensions().count() == 0) formatsBox->addItem(muxer->prettyName(),QVariant(muxer->name()));

        if (formatsFilter == 0 ||
                (formatsFilter == 1 && muxer->isAudio() && muxer->isVideo() )  ||
                (formatsFilter == 2 && muxer->isSequence() ) ||
                (formatsFilter == 3 && muxer->isAudio() && !muxer->isVideo() )  ||
                (formatsFilter == 4 && muxer->isVideo() && !muxer->isAudio() ))
        {
            formatsBox->addItem("." + muxer->extensions().join(", .") + " | " + muxer->prettyName(),QVariant(muxer->name()));
        }
    }
    _freezeUI = false;
    on_formatsBox_currentIndexChanged(formatsBox->currentIndex());
}

MediaInfo *UIOutputWidget::getMediaInfo()
{
    //ADD CUSTOM PARAMS
    for (int i = 0 ; i < _customParamEdits.count() ; i++)
    {
        QString param = _customParamEdits[i]->text();
        if (param != "")
        {
            QStringList option(param);
            option << _customValueEdits[i]->text();
            _mediaInfo->addFFmpegOption(option);
        }
    }

    return _mediaInfo;
}

void UIOutputWidget::setMediaInfo(MediaInfo *mediaInfo)
{
    if (mediaInfo == nullptr) return;

    _mediaInfo->updateInfo( mediaInfo );
}

QString UIOutputWidget::getOutputPath()
{
    return outputEdit->text();
}

void UIOutputWidget::updateBlocksAvailability()
{
    bool ok = false;

    bool okVideo = _mediaInfo->hasVideo() && !_mediaInfo->copyVideo();
    bool okAudio = _mediaInfo->hasAudio() && !_mediaInfo->copyAudio();

    FFCodec *vc = _mediaInfo->videoCodec();
    if ( vc == nullptr ) vc = _mediaInfo->defaultVideoCodec();

    //Resize
    if(!okVideo) blockResize->hide();
    actionResize->setVisible(okVideo);

    //Framerate
    ok = !_mediaInfo->isImageSequence();
    if(!okVideo || !ok) blockFrameRate->hide();
    actionFrameRate->setVisible(okVideo && ok);

    //Video codec
    ok = !_mediaInfo->isImageSequence();
    if (ok && vc != nullptr) ok = vc->name() != "gif";
    if(!okVideo || !ok) blockVideoCodec->hide();
    actionVideoCodec->setVisible(okVideo && ok);

    //Video Bitrate
    ok = !_mediaInfo->isImageSequence();
    if (ok && vc != nullptr) ok = vc->name() != "gif";
    if (!okVideo || !ok) blockVideoBitrate->hide();
    actionVideoBitrate->setVisible( okVideo && ok );

    //Video Profile
    ok = false;
    if ( vc != nullptr) ok = vc->name() == "prores";
    if (!okVideo || !ok) blockVideoProfile->hide();
    actionProfile->setVisible( okVideo && ok );

    //Loops
    ok = false;
    if ( vc != nullptr) ok = vc->name() == "gif";
    if (!okVideo || !ok) blockLoops->hide();
    actionLoops->setVisible( okVideo && ok );

    //Start Number
    ok = _mediaInfo->isImageSequence();
    if (!okVideo || !ok) blockStartNumber->hide();
    actionStartNumber->setVisible( okVideo && ok );

    //Alpha
    ok = _mediaInfo->canHaveAlpha();
    if (!okVideo || !ok) blockAlpha->hide();
    actionAlpha->setVisible( okVideo && ok );

    //Pix format
    if(!okVideo) blockPixFormat->hide();
    actionPixelFormat->setVisible(okVideo);

    //Sampling
    if(!okAudio) blockSampling->hide();
    actionSampling->setVisible(okAudio);

    //Audio Codec
    if(!okAudio) blockAudioCodec->hide();
    actionAudioCodec->setVisible(okAudio);

    //Audio Bitrate
    if(!okAudio) blockAudioBitrate->hide();
    actionAudioBitrate->setVisible(okAudio);
}

void UIOutputWidget::on_videoButton_clicked(bool checked)
{
    videoTranscodeButton->setEnabled( checked );
    videoCopyButton->setEnabled( checked );

    _mediaInfo->setVideo( checked );
}

void UIOutputWidget::on_audioButton_clicked(bool checked)
{
    audioTranscodeButton->setEnabled( checked );
    audioCopyButton->setEnabled( checked );

    _mediaInfo->setAudio( checked );
}

void UIOutputWidget::on_videoTranscodeButton_toggled( bool checked )
{
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);

    if ( checked ) _mediaInfo->setVideoCodec( nullptr );
    else _mediaInfo->setVideoCodec( "copy" );
}

void UIOutputWidget::on_audioTranscodeButton_toggled( bool checked )
{
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);

    if ( checked ) _mediaInfo->setAudioCodec( nullptr );
    else _mediaInfo->setAudioCodec( "copy" );
}

void UIOutputWidget::on_outputBrowseButton_clicked()
{
    QString outputPath = QFileDialog::getSaveFileName(this,"Output file",outputEdit->text());
    if (outputPath == "") return;
    updateOutputExtension(outputPath);
}

void UIOutputWidget::on_addParam_clicked()
{
    addNewParam();
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_formatsBox_currentIndexChanged(int index)
{
    if (index == -1) return;
    FFMuxer *_currentMuxer = _ffmpeg->muxer(formatsBox->currentData().toString());

    _mediaInfo->setMuxer( _currentMuxer );

    if (_freezeUI) return;
    _freezeUI = true;

    //UI

    //video
    if (_currentMuxer->isVideo())
    {
        videoButton->setChecked(true);
        videoTranscodeButton->setEnabled(true);
        videoCopyButton->setEnabled(true);
    }
    else
    {
        videoButton->setChecked(false);
        videoTranscodeButton->setEnabled(false);
        videoCopyButton->setEnabled(false);
    }

    //audio
    if (_currentMuxer->isAudio())
    {
        audioButton->setChecked( true );
        audioTranscodeButton->setEnabled(true);
        audioCopyButton->setEnabled(true);
    }
    else
    {
        audioButton->setChecked( false );
        audioTranscodeButton->setEnabled(false);
        audioCopyButton->setEnabled(false);
    }



    _freezeUI = false;

    updateOutputExtension(outputEdit->text());
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_formatsFilterBox_currentIndexChanged()
{
    ffmpeg_loadMuxers();
}

void UIOutputWidget::on_presetsBox_currentIndexChanged(int index)
{
    if (index == 0) return;
    if (_freezeUI) return;
    _freezeUI = true;

    //save
    if (index == presetsBox->count()-1)
    {
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
        loadPresets();
    }
    //load
    else if (index == presetsBox->count()-2)
    {
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
    //load preset
    else
    {
        _loadingPreset = true;
        //load
        _freezeUI = false;

        _mediaInfo->loadPreset(presetsBox->currentData().toString());
        _loadingPreset = false;
    }

    _freezeUI = false;
}

void UIOutputWidget::on_presetsFilterBox_activated()
{
    loadPresets( );
}

void UIOutputWidget::updateOutputExtension(QString outputPath)
{
    if (outputPath == "") return;
    QFileInfo output(outputPath);

    //remove existing {#####}
    QString outputName = output.completeBaseName();
    outputName = outputName.replace(QRegularExpression("_?{#+}"),"");

    QString newExt = "";

    FFMuxer *_currentMuxer = _mediaInfo->muxer();

    if (_currentMuxer != nullptr)
    {
        //add {#####}
        if (_currentMuxer->isSequence())
        {
            newExt = "_{#####}";
        }

        if (_currentMuxer->extensions().count() > 0)
        {
            newExt += "." + _currentMuxer->extensions()[0];
        }

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

    outputEdit->setText(QDir::toNativeSeparators(outputPath));

    _mediaInfo->setFileName( outputEdit->text() );
}

void UIOutputWidget::addNewParam(QString name, QString value)
{
    //add a param and a value
    QLineEdit *customParam = new QLineEdit(this);
    customParam->setPlaceholderText("-param");
    customParam->setText(name);
    customParam->setMinimumWidth(100);
    customParam->setMaximumWidth(100);
    //the value edit
    QLineEdit *customValue = new QLineEdit(this);
    customValue->setPlaceholderText("Value");
    customValue->setText(value);
    //add to layout and lists
    customOptionsLayout->insertRow(1,customParam,customValue);
    _customParamEdits << customParam;
    _customValueEdits << customValue;
}

void UIOutputWidget::newInputMedia(MediaInfo *input)
{
    //set output fileName
    QFileInfo inputFile(input->fileName());
    QString outputPath = inputFile.path() + "/" + inputFile.completeBaseName();
    updateOutputExtension(outputPath);

    //update blocks availability
    updateBlocksAvailability();

    // update (hidden) fields
    if (blockResize->isHidden())
    {
        blockResizeContent->setWidth( input->videoWidth() );
        blockResizeContent->setHeight( input->videoHeight() );
    }
    if (blockFrameRate->isHidden()) blockFrameRateContent->setFrameRate( input->videoFramerate() );


    if ( !input->hasAlpha() )
    {
        actionAlpha->setVisible(false);
        blockAlpha->hide();
    }


    if (blockSampling->isHidden()) blockSamplingContent->setSampling( input->audioSamplingRate() );

    //If ae render queue
    this->setVisible( !(input->isAep() && input->aeUseRQueue()));

}

void UIOutputWidget::loadPresets()
{
    if (_freezeUI) return;
    _freezeUI = true;

    presetsBox->clear();
    int index = presetsFilterBox->currentIndex();

    QStringList presets;
    //list internal
    if (index == 0 || index == 1)
    {
        foreach(QString preset, QDir(":/presets/").entryList(QDir::Files))
        {
            presets << ":/presets/" + preset;
        }
    }
    //list users
    if (index == 0 || index == 2)
    {
        QString userPresetsPath = settings.value("presets/path",QDir::homePath() + "/DuME Presets/").toString();
        QStringList filters("*.meprst");
        filters << "*.json" << "*.dffp";
        foreach (QString preset, QDir(userPresetsPath).entryList(filters,QDir::Files))
        {
            presets << userPresetsPath + "/" + preset;
        }
    }

    presets.sort();
    //add custom
    presetsBox->addItem("Custom");
    foreach(QString preset,presets)
    {
        presetsBox->addItem(QFileInfo(preset).completeBaseName(),preset);
    }
    //add load ans save
    presetsBox->addItem("Load...");
    presetsBox->addItem("Save as...");

    _freezeUI = false;
}

UIBlockWidget *UIOutputWidget::addBlock(UIBlockContent *content, QAction *action )
{
    // create block
    UIBlockWidget *b = new UIBlockWidget( action->text(), content, blocksWidget);
    blocksLayout->addWidget( b );
    //add and connect action
    blocksMenu->addAction( action );
    connect( action, SIGNAL( triggered(bool) ), b, SLOT( setVisible(bool) ) );
    connect( b, SIGNAL( activated(bool) ), action, SLOT( setChecked( bool ) ) );

    return b;
}


