#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include "ui_outputwidget.h"
#include "Renderer/mediainfo.h"
#include "Renderer/presetmanager.h"

// BLOCKS
#include "UI/Blocks/blockbasewidget.h"
#include "UI/Blocks/blockvideospeed.h"
#include "UI/Blocks/blockmotioninterpolation.h"
#include "UI/Blocks/blockcrop.h"
#include "UI/Blocks/blocklut.h"
#include "UI/Blocks/blockdeinterlace.h"
#include "UI/Blocks/blockresize.h"
#include "UI/Blocks/blockframerate.h"
#include "UI/Blocks/blockvideocodec.h"
#include "UI/Blocks/blockvideobitrate.h"
#include "UI/Blocks/blockvideoprofile.h"
#include "UI/Blocks/blockloops.h"
#include "UI/Blocks/blockstartnumber.h"
#include "UI/Blocks/blockalpha.h"
#include "UI/Blocks/blockpixformat.h"
#include "UI/Blocks/blocksampling.h"
#include "UI/Blocks/blockaudiocodec.h"
#include "UI/Blocks/blockaudiobitrate.h"
#include "UI/Blocks/blockcustom.h"
#include "UI/Blocks/blockcolor.h"
#include "UI/Blocks/blockmapping.h"
#include "UI/Blocks/blockaudiosampleformat.h"

// OTHER UI
#include "UI/dropshadow.h"

#include <QFileDialog>
#include <QSettings>
#include <QStringList>
#include <QMenu>
#include <QSignalMapper>

class OutputWidget : public QWidget, private Ui::OutputWidget
{
    Q_OBJECT

public:
    explicit OutputWidget(int id, MediaList *inputMedias, QWidget *parent = nullptr);
    MediaInfo *mediaInfo();
    void setMediaInfo(MediaInfo *mediaInfo);

signals:
    void checkSaveOutputPath(QString, OutputWidget*);
    /**
     * @brief console general messages to be displayed in the UI by MainWindow
     */
    void newLog( QString, LogUtils::LogType lt = LogUtils::Information );

public slots:
    void ffmpeg_init();
    void ffmpeg_loadMuxers();
    void loadPresets();
    void setOutputPath(QString outputPath);
    void setOutputPreset(QString preset);
    void openPresetFile(QString filename);

private slots:
    void mediaInfoChanged();
    void newInputMedia(MediaInfo* m);
    void inputChanged();

    void on_videoTranscodeButton_toggled(bool checked);
    void on_audioTranscodeButton_toggled(bool checked);
    void on_outputBrowseButton_clicked();
    void on_formatsBox_currentIndexChanged(int index);
    void on_formatsFilterBox_currentIndexChanged(int index);
    void on_presetsBox_currentIndexChanged(int index);
    void on_presetsFilterBox_activated(QString arg1);

    void on_videoButton_clicked(bool checked);
    void on_audioButton_clicked(bool checked);

    void on_actionSavePreset_triggered();
    void on_actionOpenPreset_triggered();
    void on_actionDefaultPreset_triggered(bool checked);

    void on_outputEdit_textEdited(QString text);

private:
    // BLOCKS
    BlockBaseWidget *addBlock(BlockContentWidget *content, QAction *action, QString icon="");
    BlockBaseWidget *blockVideoSpeed;
    BlockBaseWidget *blockMotionInterpolation;
    BlockBaseWidget *blockCrop;
    BlockBaseWidget *blockLut;
    BlockBaseWidget *blockDeinterlace;
    BlockBaseWidget *blockResize;
    BlockBaseWidget *blockFrameRate;
    BlockBaseWidget *blockVideoCodec;
    BlockBaseWidget *blockVideoBitrate;
    BlockBaseWidget *blockVideoProfile;
    BlockBaseWidget *blockLoops;
    BlockBaseWidget *blockStartNumber;
    BlockBaseWidget *blockAlpha;
    BlockBaseWidget *blockPixFormat;
    BlockBaseWidget *blockSampling;
    BlockBaseWidget *blockAudioCodec;
    BlockBaseWidget *blockAudioBitrate;
    BlockBaseWidget *blockColor;
    BlockBaseWidget *blockMap;
    BlockBaseWidget *blockCustom;
    BlockBaseWidget *blockAudioSampleFormat;

    // MENUS
    QMenu *blocksMenu;
    QMenu *presetsMenu;

    void addVideoStream();
    void addAudioStream();

    void selectDefaultPreset();

    QSettings settings;
    PresetManager *_presetManager;
    MediaInfo *_mediaInfo;
    QList<BlockBaseWidget *> _customParams;
    MediaList *_inputMedias;

    bool _freezeUI;
    bool _loadingPreset;
    bool _outputPathIsCustom;

    int _index;

};

#endif // OUTPUTWIDGET_H
