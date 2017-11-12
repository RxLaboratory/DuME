#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include "ui_outputwidget.h"

#include <QFileDialog>
#include <QSettings>
#include <QStringList>

#include "ffmpeg.h"

class OutputWidget : public QWidget, private Ui::OutputWidget
{
    Q_OBJECT

public:
    explicit OutputWidget(FFmpeg *ff,QWidget *parent = 0);
    FFMediaInfo *getMediaInfo();
    void setMediaInfo(FFMediaInfo *mediaInfo);

public slots:
    void ffmpeg_init();
    void ffmpeg_loadCodecs();
    void ffmpeg_loadMuxers();
    void newInputMedia(FFMediaInfo *input);
    void loadPresets(QString userPath = "");

private slots:
    void on_videoTranscodeButton_toggled(bool checked);
    void on_audioTranscodeButton_toggled(bool checked);
    void on_resizeButton_toggled(bool checked);
    void on_frameRateButton_toggled(bool checked);
    void on_samplingButton_toggled(bool checked);
    void on_outputBrowseButton_clicked();
    void on_frameRateBox_activated(const QString &arg1);
    void on_frameRateEdit_valueChanged(double arg1);
    void on_videoQualitySlider_valueChanged(int value);
    void on_videoWidthButton_valueChanged(int val);
    void on_videoHeightButton_valueChanged(int val);
    void on_videoCodecsFilterBox_currentIndexChanged(int index);
    void on_audioCodecsFilterBox_currentIndexChanged(int index);
    void on_addParam_clicked();
    void on_formatsBox_currentIndexChanged(int index);
    void on_formatsFilterBox_currentIndexChanged(int index);
    void on_videoCodecButton_toggled(bool checked);
    void on_videoBitrateButton_toggled(bool checked);
    void on_videoQualityButton_toggled(bool checked);
    void on_audioCodecButton_toggled(bool checked);
    void on_audioBitrateButton_toggled(bool checked);
    void on_presetsBox_currentIndexChanged(int index);
    void on_presetsFilterBox_activated(int index);
    void on_videoCodecsBox_currentIndexChanged(int index);
    void on_videoProfileButton_toggled(bool checked);
    void on_videoLoopsButton_toggled(bool checked);
    void on_videoLoopsEdit_valueChanged(int arg1);

private:
    /**
     * @brief aspectRatio Computes the aspect ratio of the video
     */
    void aspectRatio();
    void updateOutputExtension();
    void selectDefaultVideoCodec();
    void selectDefaultAudioCodec();
    void updateVideoOptions();
    void addNewParam(QString name = "",QString value = "");
    void init();

    QSettings _settings;
    FFmpeg *_ffmpeg;
    FFMediaInfo *_mediaInfo;
    QList<QLineEdit *> _customParamEdits;
    QList<QLineEdit *> _customValueEdits;
    FFMuxer *_currentMuxer;

    bool _freezeUI;
    bool _loadingPreset;
};

#endif // OUTPUTWIDGET_H
