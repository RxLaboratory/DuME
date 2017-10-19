#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include "ui_outputwidget.h"

#include <QFileDialog>

#include "ffmpeg.h"

class OutputWidget : public QWidget, private Ui::OutputWidget
{
    Q_OBJECT

public:
    explicit OutputWidget(FFmpeg *ff,QWidget *parent = 0);
    FFMediaInfo *getMediaInfo();

public slots:
    void ffmpeg_init();
    void ffmpeg_loadCodecs();
    void ffmpeg_loadMuxers();
    void newInputMedia(FFMediaInfo *input);

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
    void on_videoQualitySlider_sliderReleased();
    void on_videoBitRateEdit_valueChanged(double arg1);
    void on_audioQualitySlider_sliderReleased();
    void on_audioQualitySlider_valueChanged(int value);
    void on_audioBitRateEdit_valueChanged(int arg1);
    void on_videoWidthButton_valueChanged(int val);
    void on_videoHeightButton_valueChanged(int val);
    void on_videoCodecsFilterBox_currentIndexChanged(int index);
    void on_audioCodecsFilterBox_currentIndexChanged(int index);
    void on_addParam_clicked();
    void on_addAudioParam_clicked();
    void on_formatsBox_currentIndexChanged(int index);
    void on_formatsFilterBox_currentIndexChanged(int index);

private:
    /**
     * @brief aspectRatio Computes the aspect ratio of the video
     */
    void aspectRatio();
    FFmpeg *_ffmpeg;
    FFMediaInfo *_mediaInfo;
    QList<QLineEdit *> _customVideoParamEdits;
    QList<QLineEdit *> _customVideoValueEdits;
    QList<QLineEdit *> _customAudioParamEdits;
    QList<QLineEdit *> _customAudioValueEdits;

    bool _freezeUI;
};

#endif // OUTPUTWIDGET_H
