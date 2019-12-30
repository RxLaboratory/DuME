#include "uioutputwidget.h"
#include "uidropshadow.h"

#include <QGraphicsDropShadowEffect>

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

    // SHADOWS
    videoWidget->setGraphicsEffect( new UIDropShadow() );
    audioWidget->setGraphicsEffect( new UIDropShadow() );

    // CREATE MENUS

    blocksMenu = new QMenu();
    addBlockButton->setMenu( blocksMenu );

    // CREATE BLOCKS
    blocksMenu->addSection("Video");
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
    blocksMenu->addSection("Audio");
    blockSamplingContent = new BlockSampling( _mediaInfo );
    blockSampling = addBlock( blockSamplingContent, actionSampling );
    blockAudioCodecContent = new BlockAudioCodec( _ffmpeg, _mediaInfo );
    blockAudioCodec = addBlock( blockAudioCodecContent, actionAudioCodec );
    blockAudioBitrateContent = new BlockAudioBitrate( _mediaInfo );
    blockAudioBitrate = addBlock( blockAudioBitrateContent, actionAudioBitrate );

    //TODO connect mediainfo changed to h264 width/height check
    //TODO connect mediainfo changed to blocks availability

    /*
    if (!muxer->isSequence() && muxer->name() != "gif")
    {
        mainVideoCodecWidget->show();
        videoCodecButton->show();
        videoCodecWidget->show();
        videoBitrateButton->show();
        videoBitRateEdit->show();
    }

    if (muxer->isSequence())
    {
        sequenceWidget->show();
        startNumberButton->show();
        startNumberEdit->show();
    }

            //prores
            if (codec->name() == "prores")
            {
                mainVideoCodecWidget->show();
                videoProfileBox->clear();
                videoProfileBox->addItem("Proxy",0);
                videoProfileBox->addItem("LT",1);
                videoProfileBox->addItem("Normal",2);
                videoProfileBox->addItem("HQ",3);
                videoProfileButton->show();
                videoProfileBox->show();
            }
            //h264
            else if (codec->name() == "h264")
            {
                mainVideoCodecWidget->show();
                videoQualityButton->show();
                videoQualityWidget->show();
            }
            //gif
            else if (codec->name() == "gif")
            {
                sequenceWidget->show();
                videoLoopsButton->show();
                videoLoopsEdit->show();
            }

    if (audioTranscodeButton->isChecked())
    {
        //show/hide bitrate depending on muxer
        FFMuxer *muxer = _ffmpeg->muxer(formatsBox->currentData().toString());
        if (muxer != nullptr)
        {
            if (!muxer->isSequence() && muxer->name() != "gif")
            {
                mainAudioCodecWidget->show();
                audioCodecButton->show();
                audioCodecWidget->show();
                audioBitrateButton->show();
                audioBitRateEdit->show();
            }
        }
    }
    */

    init();

    ffmpeg_init();

    _freezeUI = false;

    //Set defaults
    on_presetsFilterBox_activated();

    qDebug() << "created";
    if ( _mediaInfo->videoCodec() != nullptr )
        qDebug() << _mediaInfo->videoCodec()->name();
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

    //TODO copy mediaInfo to the current one
}

QString UIOutputWidget::getOutputPath()
{
    return outputEdit->text();
}

void UIOutputWidget::on_videoTranscodeButton_toggled( bool checked )
{
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
    // TODO enable/disable video blocks
    // and update mediainfo
}

void UIOutputWidget::on_audioTranscodeButton_toggled( bool checked )
{
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
    // TODO enable/disable audio blocks
    // and update mediainfo
}

void UIOutputWidget::on_outputBrowseButton_clicked()
{
    QString outputPath = QFileDialog::getSaveFileName(this,"Output file",outputEdit->text());
    if (outputPath == "") return;
    updateOutputExtension(outputPath);
}

/*void UIOutputWidget::on_videoCodecsBox_currentIndexChanged( )
{
    updateAudioVideoOptions();
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);

    FFCodec *_currentVideoCodec = _ffmpeg->videoEncoder(videoCodecsBox->currentData().toString());
    pixFmtFilterBox->setCurrentIndex(0);

    //TODO Move all of this to the corresponding block update

    //check if there is alpha in one of the pixel formats to display button
    int numAlpha = 0;
    int numNoAlpha = 0;

    if (_currentVideoCodec == nullptr) return;

    QList<FFPixFormat *> pixFmts = _currentVideoCodec->pixFormats();
    if (pixFmts.count() == 0) return;

    foreach(FFPixFormat *pf, _currentVideoCodec->pixFormats())
    {
        if (pf->hasAlpha()) numAlpha++;
        else numNoAlpha++;
        if (numAlpha > 0 && numNoAlpha > 0) break;
    }
    if (numAlpha > 0 && numNoAlpha > 0) alphaButton->show();
    else alphaButton->hide();

    ffmpeg_loadPixFmts(true);
}*/

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
        if (noVideoButton->isChecked()) videoTranscodeButton->setChecked(true);
        videoTranscodeButton->setEnabled(true);
        videoCopyButton->setEnabled(true);
    }
    else
    {
        noVideoButton->setChecked(true);
        videoTranscodeButton->setEnabled(false);
        videoCopyButton->setEnabled(false);
    }

    //audio
    if (_currentMuxer->isAudio())
    {
        if (noAudioButton->isChecked()) audioTranscodeButton->setChecked(true);
        audioTranscodeButton->setEnabled(true);
        audioCopyButton->setEnabled(true);
    }
    else
    {
        noAudioButton->setChecked(true);
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

void UIOutputWidget::newInputMedia(MediaInfo *input)
{
    //set output fileName
    QFileInfo inputFile(input->fileName());
    QString outputPath = inputFile.path() + "/" + inputFile.completeBaseName();
    updateOutputExtension(outputPath);

    // update (hidden) fields
    if (blockResize->isHidden())
    {
        blockResizeContent->setWidth( input->videoWidth() );
        blockResizeContent->setHeight( input->videoHeight() );
    }
    if (blockFrameRate->isHidden()) blockFrameRateContent->setFrameRate( input->videoFramerate() );

    if (input->pixFormat() != nullptr)
    {
        if (input->pixFormat()->hasAlpha())
        {
            actionAlpha->setEnabled( true );
        }
        else
        {
            actionAlpha->setEnabled( false );
            blockAlpha->hide();
        }
    }
    else
    {
        actionAlpha->setEnabled( true );
    }

    if (input->isAep())
    {
        actionAlpha->setEnabled( true );
    }

    if (blockSampling->isHidden()) blockSamplingContent->setSampling( input->audioSamplingRate() );

    //If ae render queue
    if (input->isAep() && input->aeUseRQueue())
    {
        this->hide();
    }
    else
    {
        this->show();
    }
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

