#include "cropnode.h"

CropNode::CropNode(QGraphicsItem *parent):
    DuMENode("Crop", parent)
{
    this->setIcon(":/icons/crop");
    this->addVideoInput();
    this->addVideoOutput("");
}
