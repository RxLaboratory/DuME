#include "separate5dot1node.h"

Separate5Dot1Node::Separate5Dot1Node(QGraphicsItem *parent):
    DuMENode("Separate 5.1", parent)
{
    // Testing

    this->add5Dot1Input();

    this->addMonoOutput("Left");
    this->addMonoOutput("Center");
    this->addMonoOutput("Right");
    this->addMonoOutput("Rear Right");
    this->addMonoOutput("Rear Left");
    this->addMonoOutput("LFE");
}
