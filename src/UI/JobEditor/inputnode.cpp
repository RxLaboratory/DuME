#include "inputnode.h"

InputNode::InputNode(QGraphicsItem *parent):
    DuMENode("Input Media", parent)
{
    // Testing

    this->addVideoOutput();
    this->addStereoOutput();
    this->addSubtitleOutput();
}
