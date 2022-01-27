#include "motioninterpolationnode.h"

MotionInterpolationNode::MotionInterpolationNode(QGraphicsItem *parent):
    DuMENode("Motion Interpolation", parent)
{
    this->setIcon(":/icons/motion-interpo");
    this->addVideoInput();
    this->addVideoOutput("");
}
