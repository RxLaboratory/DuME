#include "downmix5dot1tomononode.h"

DownMix5Dot1ToMonoNode::DownMix5Dot1ToMonoNode(QGraphicsItem *parent):
    DuMENode("5.1 to Mono Downmix", parent)
{
    this->add5Dot1Input();

    this->addMonoOutput();
}
