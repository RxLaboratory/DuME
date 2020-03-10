#ifndef BLOCKAUDIOCODEC_H
#define BLOCKAUDIOCODEC_H

#include "global.h"

#include "ui_blockaudiocodec.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockAudioCodec : public BlockContentWidget, private Ui::BlockAudioCodec
{
    Q_OBJECT  
public:
    explicit BlockAudioCodec( MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void on_audioCodecsBox_currentIndexChanged(int index);
    void on_audioCodecsFilterBox_currentIndexChanged(int index);
    void listCodecs();
private:
    void setDefaultCodec();
    void setCodec(QString name, bool tryWithoutFilter = true);

};

#endif // BLOCKAUDIOCODEC_H
