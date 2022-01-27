#include "mixstereonode.h"

MixStereoNode::MixStereoNode(QGraphicsItem *parent):
    DuMENode("Stereo Mix", parent)
{
    // Testing

    this->addStereoOutput();

    this->addMonoInput("Left");
    this->addMonoInput("Right");
}
