#ifndef DUMENODE_H
#define DUMENODE_H

#include "duqf-nodeview/duqfnode.h"

class DuMENode : public DuQFNode
{
public:
    DuMENode(QString title, QGraphicsItem *parent = nullptr);

    void addVideoInput(QString name="Video");
    void addVideoOutput(QString name="Video");

    void add5Dot1Input(QString name="5.1");
    void add5Dot1Output(QString name="5.1");

    void addStereoInput(QString name="Stereo");
    void addStereoOutput(QString name="Stereo");

    void addMonoInput(QString name="Mono");
    void addMonoOutput(QString name="Mono");

    void addSubtitleInput(QString name="Subtitles");
    void addSubtitleOutput(QString name="Subtitles");
};

#endif // DUMENODE_H
