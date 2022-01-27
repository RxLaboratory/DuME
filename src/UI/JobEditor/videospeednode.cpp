#include "videospeednode.h"

VideoSpeedNode::VideoSpeedNode(QGraphicsItem *parent):
    DuMENode("Video Speed", parent)
{
    this->setIcon(":/icons/video-speed");
    this->addVideoInput();
    this->addVideoOutput("");
}
