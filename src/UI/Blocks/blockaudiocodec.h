#ifndef BLOCKAUDIOCODEC_H
#define BLOCKAUDIOCODEC_H

#include "ui_blockaudiocodec.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockAudioCodec : public UIBlockContent, private Ui::BlockAudioCodec
{
    Q_OBJECT  
public:
    explicit BlockAudioCodec(FFmpeg *ffmpeg, MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void setActivated( bool activate );
    void update();
private slots:
    void on_audioCodecsBox_currentIndexChanged(int index);
    void on_audioCodecsFilterBox_currentIndexChanged(int index);
    void listCodecs();
private:
    FFmpeg *_ffmpeg;
    void setDefaultCodec();
    void setCodec(QString name, bool tryWithoutFilter = true);

};

#endif // BLOCKAUDIOCODEC_H