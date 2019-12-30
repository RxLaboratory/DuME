#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include "ui_outputwidget.h"

#include "Renderer/mediainfo.h"

// BLOCKS
#include "UI/Blocks/uiblockwidget.h"
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

#include <QFileDialog>
#include <QSettings>
#include <QStringList>
#include <QMenu>

class UIOutputWidget : public QWidget, private Ui::OutputWidget
{
    Q_OBJECT

public:
    explicit UIOutputWidget(FFmpeg *ff, int id, QWidget *parent = nullptr);
    MediaInfo *getMediaInfo();
    void setMediaInfo(MediaInfo *mediaInfo);
    QString getOutputPath();

signals:
    void checkSaveOutputPath(QString, UIOutputWidget*);
    /**
     * @brief console general messages to be displayed in the UI by MainWindow
     */
    void newLog( QString, LogUtils::LogType lt = LogUtils::Information );

public slots:
    void ffmpeg_init();
    void ffmpeg_loadMuxers();
    void newInputMedia(MediaInfo *input);
    void loadPresets();

private slots:
    void updateBlocksAvailability();

    void on_videoTranscodeButton_toggled(bool checked);
    void on_audioTranscodeButton_toggled(bool checked);
    void on_outputBrowseButton_clicked();
    void on_addParam_clicked();
    void on_formatsBox_currentIndexChanged(int index);
    void on_formatsFilterBox_currentIndexChanged();
    void on_presetsBox_currentIndexChanged(int index);
    void on_presetsFilterBox_activated();
    void on_videoButton_clicked(bool checked);
    void on_audioButton_clicked(bool checked);

private:
    // BLOCKS
    UIBlockWidget *addBlock(UIBlockContent *content, QAction *action);
    UIBlockWidget *blockResize;
    BlockResize *blockResizeContent;
    UIBlockWidget *blockFrameRate;
    BlockFrameRate *blockFrameRateContent;
    UIBlockWidget *blockVideoCodec;
    BlockVideoCodec *blockVideoCodecContent;
    UIBlockWidget *blockVideoBitrate;
    BlockVideoBitrate *blockVideoBitrateContent;
    UIBlockWidget *blockVideoProfile;
    BlockVideoProfile *blockVideoProfileContent;
    UIBlockWidget *blockLoops;
    BlockLoops *blockLoopsContent;
    UIBlockWidget *blockStartNumber;
    BlockStartNumber *blockStartNumberContent;
    UIBlockWidget *blockAlpha;
    BlockAlpha *blockAlphaContent;
    UIBlockWidget *blockPixFormat;
    BlockPixFormat *blockPixFormatContent;
    UIBlockWidget *blockSampling;
    BlockSampling *blockSamplingContent;
    UIBlockWidget *blockAudioCodec;
    BlockAudioCodec *blockAudioCodecContent;
    UIBlockWidget *blockAudioBitrate;
    BlockAudioBitrate *blockAudioBitrateContent;

    // MENUS
    QMenu *blocksMenu;

    void updateOutputExtension(QString outputPath);
    void addNewParam(QString name = "",QString value = "");
    void init();

    QSettings settings;
    FFmpeg *_ffmpeg;
    MediaInfo *_mediaInfo;
    QList<QLineEdit *> _customParamEdits;
    QList<QLineEdit *> _customValueEdits;

    bool _freezeUI;
    bool _loadingPreset;

    int _index;
};

#endif // OUTPUTWIDGET_H
