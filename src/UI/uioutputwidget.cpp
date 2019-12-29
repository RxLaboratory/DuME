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

    // CREATE MENUS

    videoMenu = new QMenu();
    addVideoBlockButton->setMenu( videoMenu );

    // CREATE BLOCKS
    blockResizeContent = new BlockResize( _mediaInfo );
    blockResize = addVideoBlock( blockResizeContent, actionResize );
    blockFrameRateContent = new BlockFrameRate( _mediaInfo );
    blockFrameRate = addVideoBlock( blockFrameRateContent, actionFrameRate );
    blockVideoCodecContent = new BlockVideoCodec( _ffmpeg, _mediaInfo );
    blockVideoCodec = addVideoBlock( blockVideoCodecContent, actionVideoCodec );
    blockVideoBitrateContent = new BlockVideoBitrate( _mediaInfo );
    blockVideoBitrate = addVideoBlock( blockVideoBitrateContent, actionVideoBitrate );
    blockVideoProfileContent = new BlockVideoProfile( _mediaInfo );
    blockVideoProfile = addVideoBlock( blockVideoProfileContent, actionProfile );
    blockLoopsContent = new BlockLoops( _mediaInfo );
    blockLoops = addVideoBlock( blockLoopsContent, actionLoops );
    blockStartNumberContent = new BlockStartNumber( _mediaInfo );
    blockStartNumber = addVideoBlock( blockStartNumberContent, actionStartNumber );
    blockAlphaContent = new BlockAlpha( _mediaInfo );
    blockAlpha = addVideoBlock( blockAlphaContent, actionAlpha );

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
    */

    //populate sampling box
    //TODO Get from ffmpeg
    samplingBox->addItem("8,000 Hz",QVariant(8000));
    samplingBox->addItem("11,025 Hz",QVariant(11025));
    samplingBox->addItem("16,000 Hz",QVariant(16000));
    samplingBox->addItem("22,050 Hz",QVariant(22050));
    samplingBox->addItem("32,000 Hz",QVariant(32000));
    samplingBox->addItem("44,100 Hz",QVariant(44100));
    samplingBox->addItem("48,000 Hz",QVariant(48000));
    samplingBox->addItem("88,200 Hz",QVariant(88200));
    samplingBox->addItem("96,000 Hz",QVariant(96000));
    samplingBox->setCurrentIndex(6);

    init();

    ffmpeg_init();

    _freezeUI = false;

    //Set defaults
    outputTabs->setCurrentIndex(0);
    on_presetsFilterBox_activated();
    //hide/show supported/unsupported options
    updateAudioVideoOptions();
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
    samplingButton->setChecked(false);
    audioCodecButton->setChecked(false);
    audioBitrateButton->setChecked(false);

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

    delete _mediaInfo;
    _mediaInfo = mediaInfo;
    mediaInfo->setParent(this);

    updateMediaInfo();
}

void UIOutputWidget::updateMediaInfo()
{
    init();

    //MUXER
    FFMuxer *muxer = _mediaInfo->muxer();
    if (muxer != nullptr)
    {
        for (int i = 0;i < formatsBox->count() ; i++)
        {
            if (formatsBox->itemData(i).toString() == muxer->name())
            {
                formatsBox->setCurrentIndex(i);
                break;
            }
        }
    }

    //VIDEO
    if (_mediaInfo->hasVideo())
    {
        FFCodec *_currentVideoCodec = _mediaInfo->videoCodec();
        if (_currentVideoCodec != nullptr)
        {
            if (_currentVideoCodec->name() == "copy") videoCopyButton->setChecked(true);
            else
            {
                videoTranscodeButton->setChecked(true);
                //set pixFmt
                FFPixFormat *pf = _mediaInfo->pixFormat();
                if (pf != nullptr)
                {
                    pixFmtButton->setChecked(true);
                    for(int i = 0 ; i  < pixFmtBox->count() ; i++)
                    {
                        if (pixFmtBox->itemData(i).toString() == pf->name())
                        {
                            pixFmtBox->setCurrentIndex(i);
                            break;
                        }
                    }
                }
            }
        }

        blockVideoCodec->setVisible( _mediaInfo->videoCodec() != nullptr );
        blockVideoBitrate->setVisible( _mediaInfo->videoBitrate() != 0.0 );
        blockVideoProfile->setVisible( _mediaInfo->videoProfile() != -1 );
        int width = _mediaInfo->videoWidth();
        int height = _mediaInfo->videoHeight();
        blockResize->setVisible( width != 0 && height != 0 );
        blockFrameRate->setVisible( _mediaInfo->videoFramerate() != 0.0 );
        blockLoops->setVisible( _mediaInfo->loop() != -1 );

    }
    else
    {
        noVideoButton->setChecked(true);
    }


    //AUDIO
    if (_mediaInfo->hasAudio())
    {
        FFCodec *_currentAudioCodec = _mediaInfo->audioCodec();
        if (_currentAudioCodec != nullptr)
        {
            if (_currentAudioCodec->name() == "copy") audioCopyButton->setChecked(true);
            else
            {
                audioTranscodeButton->setChecked(true);
                audioCodecButton->setChecked(true);
                for(int i = 0 ; i  < audioCodecsBox->count() ; i++)
                {
                    if (audioCodecsBox->itemData(i).toString() == _currentAudioCodec->name())
                    {
                        audioCodecsBox->setCurrentIndex(i);
                        break;
                    }
                }
            }
        }
        int sampling = _mediaInfo->audioSamplingRate();
        if (sampling != 0)
        {
            samplingButton->setChecked(true);
            for(int i = 0 ; i < samplingBox->count() ; i++)
            {
                if (samplingBox->itemData(i).toInt() == sampling)
                {
                    samplingBox->setCurrentIndex(i);
                    break;
                }
            }
        }
        qint64 bitrate = _mediaInfo->audioBitrate( );
        if (bitrate != 0)
        {
            audioBitrateButton->setChecked(true);
            audioBitRateEdit->setValue(bitrate/1024);
        }
    }
    else
    {
        noAudioButton->setChecked(true);
    }


    //CUSTOM
    foreach (QStringList option , _mediaInfo->ffmpegOptions())
    {
        QString name = option[0];
        QString value = "";
        if (option.count() > 0) value = option[1];
        addNewParam(name,value);
    }
}

QString UIOutputWidget::getOutputPath()
{
    return outputEdit->text();
}

void UIOutputWidget::on_videoTranscodeButton_toggled( bool checked )
{
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
    if ( checked )
    {
        addVideoBlockButton->setEnabled( true );
        _mediaInfo->setVideo(true);
    }
    else if ( videoCopyButton->isChecked() )
    {
        _mediaInfo->setVideo(true);
        addVideoBlockButton->setEnabled( false );
        _mediaInfo->setVideoCodec( _ffmpeg->videoEncoder("copy") );
    }
    else
    {
        _mediaInfo->setVideo(false);
        addVideoBlockButton->setEnabled( false );
        _mediaInfo->setVideoCodec( nullptr );
    }
    updateAudioVideoOptions();
}

void UIOutputWidget::on_audioTranscodeButton_toggled( bool checked )
{
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
    if ( checked )
    {
        _mediaInfo->setAudio(true);
        if ( audioCodecButton->isChecked() )
        {
            _mediaInfo->setAudioCodec( _ffmpeg->audioEncoder( audioCodecsBox->currentData(Qt::UserRole).toString()));
        }
        else
        {
            _mediaInfo->setAudioCodec( nullptr );
        }
    }
    else if ( videoCopyButton->isChecked() )
    {
        _mediaInfo->setAudio(true);
        _mediaInfo->setAudioCodec( _ffmpeg->audioEncoder("copy") );
    }
    else
    {
        _mediaInfo->setAudio(false);
        _mediaInfo->setAudioCodec( nullptr );
    }
    updateAudioVideoOptions();
}

void UIOutputWidget::on_samplingButton_toggled(bool checked)
{
    samplingBox->setEnabled(checked);
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);

    if (checked)
    {
        _mediaInfo->setAudioSamplingRate( samplingBox->currentData(Qt::UserRole).toInt() );
    }
    else
    {
        _mediaInfo->setAudioSamplingRate( 0 );
    }
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

void UIOutputWidget::on_audioCodecsBox_currentIndexChanged()
{
    FFCodec *_currentAudioCodec = _ffmpeg->audioEncoder(audioCodecsBox->currentData().toString());
    _mediaInfo->setAudioCodec( _currentAudioCodec );
}

void UIOutputWidget::on_pixFmtFilterBox_currentIndexChanged()
{
    ffmpeg_loadPixFmts();
}

void UIOutputWidget::on_audioCodecsFilterBox_currentIndexChanged()
{
    ffmpeg_loadCodecs();
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

    selectDefaultAudioCodec();

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
    updateAudioVideoOptions();
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_formatsFilterBox_currentIndexChanged()
{
    ffmpeg_loadMuxers();
}

void UIOutputWidget::on_pixFmtButton_toggled(bool checked)
{
    if (checked)
    {
        pixFmtFilterBox->setItemText(0,"All bit depths");
        pixFmtWidget->setEnabled(true);
        _mediaInfo->setPixFormat( _ffmpeg->pixFormat( pixFmtBox->currentData(Qt::UserRole).toString() ));
    }
    else
    {
        pixFmtFilterBox->setCurrentIndex(0);
        ffmpeg_loadPixFmts();
        pixFmtFilterBox->setItemText(0,"Default");
        pixFmtWidget->setEnabled(false);
        _mediaInfo->setPixFormat( nullptr );
    }
}

void UIOutputWidget::on_audioCodecButton_toggled(bool checked)
{
    if (checked)
    {
        audioCodecsFilterBox->setItemText(0,"All codecs");
        audioCodecWidget->setEnabled(true);
        _mediaInfo->setAudioCodec( _ffmpeg->audioEncoder( audioCodecsBox->currentData(Qt::UserRole).toString()));
    }
    else
    {
        audioCodecsFilterBox->setCurrentIndex(0);
        ffmpeg_loadCodecs();
        audioCodecsFilterBox->setItemText(0,"Default");
        audioCodecWidget->setEnabled(false);
        _mediaInfo->setAudioCodec( nullptr );
    }
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_audioBitrateButton_toggled(bool checked)
{
    if (checked)
    {
        audioBitRateEdit->setValue(320);
        audioBitRateEdit->setSuffix(" Kbps");
        audioBitRateEdit->setEnabled(true);
        _mediaInfo->setAudioBitrate( MediaUtils::convertToBps( 320, MediaUtils::kbps ));
    }
    else
    {
        audioBitRateEdit->setValue(0);
        audioBitRateEdit->setSuffix(" Auto");
        audioBitRateEdit->setEnabled(false);
        _mediaInfo->setAudioBitrate( 0 );
    }
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
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
        updateMediaInfo();
        _loadingPreset = false;
    }
    //load preset
    else
    {
        _loadingPreset = true;
        //load
        _freezeUI = false;
        //set filters to all
        formatsFilterBox->setCurrentIndex(0);
        audioCodecsFilterBox->setCurrentIndex(0);

        _mediaInfo->loadPreset(presetsBox->currentData().toString());
        updateMediaInfo();

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

void UIOutputWidget::selectDefaultAudioCodec()
{
    FFMuxer *_currentMuxer = _mediaInfo->muxer();
    if (_currentMuxer == nullptr) return;

    FFCodec *audioCodec = _ffmpeg->muxerDefaultCodec(_currentMuxer, FFCodec::Audio);

    if (audioCodec != nullptr)
    {
        for (int a = 0; a < audioCodecsBox->count() ; a++)
        {
            if (audioCodecsBox->itemData(a).toString() == audioCodec->name())
            {
                audioCodecsBox->setCurrentIndex(a);
                break;
            }
        }
    }
}

void UIOutputWidget::selectDefaultPixFmt()
{
    FFCodec *_currentVideoCodec = _mediaInfo->videoCodec();
    if (_currentVideoCodec == nullptr) return;

    FFPixFormat *pixFmt = _currentVideoCodec->defaultPixFormat();

    //Select Default Pix Format

    bool ok = false;
    if (pixFmt != nullptr)
    {
        for (int i = 0; i < pixFmtBox->count() ; i++)
        {
            if (pixFmtBox->itemData(i).toString() == pixFmt->name())
            {
                pixFmtBox->setCurrentIndex(i);
                ok = true;
                break;
            }
        }
        if (!ok)
        {
            int bpc = pixFmt->bitsPerComponent();
            for (int i = 0; i < pixFmtBox->count() ; i++)
            {
                if (pixFmtBox->itemText(i).indexOf("@" + QString::number(bpc) + "bpc") > 0)
                {
                    pixFmtBox->setCurrentIndex(i);
                    break;
                }
            }
        }
    }
}

void UIOutputWidget::updateAudioVideoOptions()
{
    //VIDEO
    blockVideoCodec->hide();
    blockVideoBitrate->hide();
    blockVideoProfile->hide();
    blockResize->hide();
    blockFrameRate->hide();
    blockStartNumber->hide();
    componentsWidget->hide();
    //AUDIO
    samplingWidget->hide();
    mainAudioCodecWidget->hide();

    //hide all details

    //VIDEO

    //AUDIO
    //codec
    audioCodecButton->hide();
    audioCodecWidget->hide();
    //bitrate
    audioBitrateButton->hide();
    audioBitRateEdit->hide();

    if (videoTranscodeButton->isChecked())
    {
        componentsWidget->show();
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
                samplingWidget->show();
                audioCodecButton->show();
                audioCodecWidget->show();
                audioBitrateButton->show();
                audioBitRateEdit->show();
            }
        }
    }

    //uncheck what is hidden

    if (audioCodecButton->isHidden()) audioCodecButton->setChecked(false);
    if (audioBitrateButton->isHidden()) audioBitrateButton->setChecked(false);
    if (pixFmtButton->isHidden()) pixFmtButton->setChecked(false);


    //TEMP - Hide all to test blocks
    componentsWidget->hide();
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
    //ffmpeg_loadCodecs();
    ffmpeg_loadMuxers();
    _freezeUI = false;
}

void UIOutputWidget::ffmpeg_loadCodecs()
{
    _freezeUI = true;
    //get codecs and muxers
    audioCodecsBox->clear();
    QList<FFCodec *> encoders = _ffmpeg->encoders();
    if (encoders.count() == 0)
    {
        _freezeUI = false;
        return;
    }

    int audioFilter = audioCodecsFilterBox->currentIndex();

    foreach(FFCodec *encoder,encoders)
    {
        if (encoder->name() == "copy") continue;

        if (encoder->isAudio())
        {
            if (audioFilter <= 0 || (audioFilter == 1 && encoder->isLossy()) || (audioFilter == 2 && encoder->isLossless()))
            {
                audioCodecsBox->addItem(encoder->prettyName(),QVariant(encoder->name()));
            }
        }
    }

    selectDefaultAudioCodec();
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

void UIOutputWidget::ffmpeg_loadPixFmts(bool init)
{
    _freezeUI = true;

    //get pixFmts
    pixFmtBox->clear();
    FFCodec *_currentVideoCodec = _mediaInfo->videoCodec();
    if (_currentVideoCodec == nullptr)
    {
        _freezeUI = false;
        return;
    }
    QList<FFPixFormat *> pixFmts = _currentVideoCodec->pixFormats();
    if (pixFmts.count() == 0)
    {
        _freezeUI = false;
        return;
    }

    QStringList filters = QStringList();

    foreach (FFPixFormat *pixFmt, pixFmts)
    {

        QString filter = QString::number(pixFmt->bitsPerPixel()) + "bits (" + QString::number(pixFmt->numComponents()) + " channels @" + QString::number(pixFmt->bitsPerComponent()) + "bpc)" ;
        bool addToList = pixFmt->prettyName().indexOf(pixFmtFilterBox->currentText()) > 0 || pixFmtFilterBox->currentIndex() == 0;

        //check if there is alpha
        /*if (alphaButton->isVisible())
        {
            if (pixFmt->hasAlpha() && alphaButton->isChecked())
            {
                //add pix fmt in list
                if (addToList) pixFmtBox->addItem(pixFmt->prettyName(),QVariant(pixFmt->name()));
                if (!filters.contains(filter)) filters << filter;
            }
            else if (!pixFmt->hasAlpha() && !alphaButton->isChecked())
            {
                //add pix fmt in list
                if (addToList) pixFmtBox->addItem(pixFmt->prettyName(),QVariant(pixFmt->name()));
                if (!filters.contains(filter)) filters << filter;
            }
        }
        else
        {
            if (addToList) pixFmtBox->addItem(pixFmt->prettyName(),QVariant(pixFmt->name()));
            if (!filters.contains(filter)) filters << filter;
        }*/
    }

    if (init)
    {
        filters.sort();
        //remove old
        pixFmtFilterBox->clear();
        pixFmtFilterBox->addItem("All bit depths",QVariant(0));
        foreach(QString filter,filters)
        {
            pixFmtFilterBox->addItem(filter);
        }
    }

    selectDefaultPixFmt();
    _freezeUI = false;
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

    if (!samplingButton->isChecked())
    {
        int sampling = input->audioSamplingRate();
        if (sampling == 8000) samplingBox->setCurrentIndex(0);
        else if (sampling == 11025) samplingBox->setCurrentIndex(1);
        else if (sampling == 16000) samplingBox->setCurrentIndex(2);
        else if (sampling == 22050) samplingBox->setCurrentIndex(3);
        else if (sampling == 32000) samplingBox->setCurrentIndex(4);
        else if (sampling == 44100) samplingBox->setCurrentIndex(5);
        else if (sampling == 48000) samplingBox->setCurrentIndex(6);
        else if (sampling == 88200) samplingBox->setCurrentIndex(7);
        else if (sampling == 96000) samplingBox->setCurrentIndex(8);
    }

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

void UIOutputWidget::on_samplingBox_currentIndexChanged(int index)
{
    _mediaInfo->setAudioSamplingRate( samplingBox->itemData(index, Qt::UserRole).toInt() );
}

void UIOutputWidget::on_pixFmtBox_currentIndexChanged(int index)
{
    _mediaInfo->setPixFormat( _ffmpeg->pixFormat( pixFmtBox->itemData(index, Qt::UserRole).toString()) );
}

void UIOutputWidget::on_audioBitRateEdit_valueChanged(int arg1)
{
    _mediaInfo->setAudioBitrate( MediaUtils::convertToBps( arg1, MediaUtils::kbps ) );
}

UIBlockWidget *UIOutputWidget::addVideoBlock(UIBlockContent *content, QAction *action )
{
    // create block
    UIBlockWidget *b = new UIBlockWidget( action->text(), content, videoTab);
    videoLayout->addWidget( b );
    //add and connect action
    videoMenu->addAction( action );
    connect( action, SIGNAL( triggered(bool) ), b, SLOT( setVisible(bool) ) );
    connect( b, SIGNAL( activated(bool) ), action, SLOT( setChecked( bool ) ) );

    return b;
}
