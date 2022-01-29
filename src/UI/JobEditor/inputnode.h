#ifndef INPUTNODE_H
#define INPUTNODE_H

#include "UI/JobEditor/dumenode.h"
#include "UI/Blocks/inputblock.h"

class InputNode : public DuMENode
{
public:
    InputNode(QGraphicsItem *parent = nullptr);
    InputNode(QString file, QGraphicsItem *parent = nullptr);
    MediaInfo *mediaInfo() const;

private slots:
    void mediaChanged();

private:
    void connectEvents();

    InputBlock *ui_inputBlock;

    MediaInfo *m_mediaInfo;

    int m_numVideo = 0;
    int m_numAudio = 0;
    int m_num5Dot1 = 0;
    int m_numStereo = 0;
    int m_numMono = 0;
    int m_numSubtitle = 0;
};

#endif // INPUTNODE_H
