#include "separatestereonode.h"

SeparateStereoNode::SeparateStereoNode(QGraphicsItem *parent):
    DuMENode("Separate Stereo", parent)
{
    // Testing

    this->addStereoInput();

    this->addMonoOutput("Left");
    this->addMonoOutput("Right");
}
