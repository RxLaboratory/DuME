#include "deinterlacenode.h"

DeinterlaceNode::DeinterlaceNode(QGraphicsItem *parent):
    DuMENode("Deinterlace", parent)
{
    this->setIcon(":/icons/interlace");
    this->addVideoInput();
    this->addVideoOutput("");
}
