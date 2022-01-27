#include "unpremultiplynode.h"

UnPremultiplyNode::UnPremultiplyNode(QGraphicsItem *parent):
    DuMENode("Unpremultiply", parent)
{
    this->setIcon(":/icons/alpha");
    this->addVideoInput();
    this->addVideoOutput("");
}
