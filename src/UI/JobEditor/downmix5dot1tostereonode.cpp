#include "downmix5dot1tostereonode.h"

DownMix5Dot1ToStereoNode::DownMix5Dot1ToStereoNode(QGraphicsItem *parent):
    DuMENode("5.1 to Stereo Downmix", parent)
{
    this->add5Dot1Input();

    this->addStereoOutput();
}
