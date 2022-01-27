#include "outputnode.h"

OutputNode::OutputNode(QGraphicsItem *parent):
    DuMENode("Output Media", parent)
{
    // Testing

    this->addVideoInput();
    this->addStereoInput();
    this->addSubtitleInput();
}
