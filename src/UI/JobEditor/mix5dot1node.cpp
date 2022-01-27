#include "mix5dot1node.h"

Mix5Dot1Node::Mix5Dot1Node(QGraphicsItem *parent):
    DuMENode("5.1 Mix", parent)
{
    // Testing

    this->add5Dot1Output();

    this->addMonoInput("Left");
    this->addMonoInput("Center");
    this->addMonoInput("Right");
    this->addMonoInput("Rear Right");
    this->addMonoInput("Rear Left");
    this->addMonoInput("LFE");
}
