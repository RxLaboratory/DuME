#include "downmixstereotomononode.h"

DownMixStereoToMonoNode::DownMixStereoToMonoNode(QGraphicsItem *parent):
    DuMENode("Stereo to Mono Downmix", parent)
{
    this->addStereoInput();

    this->addMonoOutput();
}
