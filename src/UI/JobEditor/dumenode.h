#ifndef DUMENODE_H
#define DUMENODE_H

#include "duqf-nodeview/duqfnode.h"

class DuMENode : public DuQFNode
{
public:
    DuMENode(QString title, QGraphicsItem *parent = nullptr);

    DuQFSlot *addVideoInput(QString name="Video");
    DuQFSlot *addVideoOutput(QString name="Video");

    DuQFSlot *add5Dot1Input(QString name="5.1");
    DuQFSlot *add5Dot1Output(QString name="5.1");

    DuQFSlot *addStereoInput(QString name="Stereo");
    DuQFSlot *addStereoOutput(QString name="Stereo");

    DuQFSlot *addMonoInput(QString name="Mono");
    DuQFSlot *addMonoOutput(QString name="Mono");

    DuQFSlot *addSubtitleInput(QString name="Subtitles");
    DuQFSlot *addSubtitleOutput(QString name="Subtitles");
};

#endif // DUMENODE_H
