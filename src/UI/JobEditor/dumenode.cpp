#include "dumenode.h"

DuMENode::DuMENode(QString title, QGraphicsItem *parent):
    DuQFNode(0, 0, title, parent)
{

}

void DuMENode::addVideoInput(QString name)
{
    addInput(name, QColor(131,211,246));
}

void DuMENode::addVideoOutput(QString name)
{
    addOutput(name, QColor(131,211,246));
}

void DuMENode::add5Dot1Input(QString name)
{
    addInput(name, QColor(213,136,241));
}

void DuMENode::add5Dot1Output(QString name)
{
    addOutput(name, QColor(213,136,241));
}

void DuMENode::addStereoInput(QString name)
{
    addInput(name, QColor(241,136,186));
}

void DuMENode::addStereoOutput(QString name)
{
    addOutput(name, QColor(241,136,186));
}

void DuMENode::addMonoInput(QString name)
{
    addInput(name, QColor(249,105,105));
}

void DuMENode::addMonoOutput(QString name)
{
    addOutput(name, QColor(249,105,105));
}

void DuMENode::addSubtitleInput(QString name)
{
    addInput(name, QColor(216,255,140));
}

void DuMENode::addSubtitleOutput(QString name)
{
    addOutput(name, QColor(216,255,140));
}
