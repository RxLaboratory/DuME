#include "dumenode.h"

DuMENode::DuMENode(QString title, QGraphicsItem *parent):
    DuQFNode(0, 0, title, parent)
{

}

DuQFSlot *DuMENode::addVideoInput(QString name)
{
    return addInput(name, QColor(131,211,246), "video");
}

DuQFSlot *DuMENode::addVideoOutput(QString name)
{
    return addOutput(name, QColor(131,211,246), "video");
}

DuQFSlot *DuMENode::add5Dot1Input(QString name)
{
    return addInput(name, QColor(213,136,241), "5.1");
}

DuQFSlot *DuMENode::add5Dot1Output(QString name)
{
    return addOutput(name, QColor(213,136,241), "5.1");
}

DuQFSlot *DuMENode::addStereoInput(QString name)
{
    return addInput(name, QColor(241,136,186), "stereo");
}

DuQFSlot *DuMENode::addStereoOutput(QString name)
{
    return addOutput(name, QColor(241,136,186), "stereo");
}

DuQFSlot *DuMENode::addMonoInput(QString name)
{
    return addInput(name, QColor(249,105,105), "mono");
}

DuQFSlot *DuMENode::addMonoOutput(QString name)
{
    return addOutput(name, QColor(249,105,105), "mono");
}

DuQFSlot *DuMENode::addSubtitleInput(QString name)
{
    return addInput(name, QColor(216,255,140), "subtitle");
}

DuQFSlot *DuMENode::addSubtitleOutput(QString name)
{
    return addOutput(name, QColor(216,255,140), "subtitle");
}
