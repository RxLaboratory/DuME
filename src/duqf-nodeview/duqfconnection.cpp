#include "duqfconnection.h"

DuQFConnection::DuQFConnection(DuQFSlot *output, DuQFSlot *input, QObject *parent) : QObject(parent)
{
    init(output, input, new DuQFConnector());
}

DuQFConnection::DuQFConnection(DuQFSlot *output, DuQFSlot *input, DuQFConnector *connector, QObject *parent) : QObject(parent)
{
    init(output, input, connector);
}

DuQFSlot *DuQFConnection::input() const
{
    return m_input;
}

void DuQFConnection::setInput(DuQFSlot *input)
{
    while(m_inputConnections.count() > 0) disconnect( m_inputConnections.takeLast());

    DuQFNode *outputParent = (DuQFNode*)m_output->parentItem();
    DuQFNode *inputParent = (DuQFNode*)m_input->parentItem();

    outputParent->removeChildNode(inputParent);
    inputParent->removeParentNode(outputParent);

    m_input = input;

    inputParent = (DuQFNode*)m_input->parentItem();

    outputParent->addChildNode(inputParent);
    inputParent->addParentNode(outputParent);

    m_inputConnections << connect(inputParent, &QGraphicsObject::xChanged, this, &DuQFConnection::inputMoved);
    m_inputConnections << connect(inputParent, &QGraphicsObject::yChanged, this, &DuQFConnection::inputMoved);
    m_inputConnections << connect(input, &DuQFSlot::removed, this, &DuQFConnection::remove);

    inputMoved();
}

DuQFSlot *DuQFConnection::output() const
{
    return m_output;
}

void DuQFConnection::setOutput(DuQFSlot *output)
{
    while(m_outputConnections.count() > 0) disconnect( m_outputConnections.takeLast());

    DuQFNode *outputParent = (DuQFNode*)m_output->parentItem();
    DuQFNode *inputParent = (DuQFNode*)m_input->parentItem();

    outputParent->removeChildNode(inputParent);
    inputParent->removeParentNode(outputParent);

    m_output = output;

    outputParent = (DuQFNode*)m_output->parentItem();

    outputParent->addChildNode(inputParent);
    inputParent->addParentNode(outputParent);

    m_outputConnections << connect(outputParent, &QGraphicsObject::xChanged, this, &DuQFConnection::outputMoved);
    m_outputConnections << connect(outputParent, &QGraphicsObject::yChanged, this, &DuQFConnection::outputMoved);
    m_outputConnections << connect(output, &DuQFSlot::removed, this, &DuQFConnection::remove);

    outputMoved();
}

void DuQFConnection::remove()
{
    if (m_removing) return;
    m_removing = true;
    m_connector->remove();

    DuQFNode *outputParent = (DuQFNode*)m_output->parentItem();
    DuQFNode *inputParent = (DuQFNode*)m_input->parentItem();

    outputParent->removeChildNode(inputParent);
    inputParent->removeParentNode(outputParent);

    emit removed();
    deleteLater();
}

void DuQFConnection::outputMoved()
{
    m_connector->setFrom( m_output->scenePos() );
}

void DuQFConnection::inputMoved()
{
    m_connector->setTo( m_input->scenePos() );
}

void DuQFConnection::init(DuQFSlot *output, DuQFSlot *input, DuQFConnector *connector)
{
    if (output->slotType() == DuQFSlot::Output)
    {
        m_output = output;
        m_input = input;
    }
    else
    {
        m_input = output;
        m_output = input;
    }
    m_connector = connector;

    m_input = input;
    m_output = output;

    setInput(m_input);
    setOutput(m_output);

    connect(connector, &DuQFConnector::removed, this, &DuQFConnection::remove);
}

DuQFConnector *DuQFConnection::connector() const
{
    return m_connector;
}

DuQFNode *DuQFConnection::inputNode()
{
    return (DuQFNode*)m_input->parentItem();
}

DuQFNode *DuQFConnection::outputNode()
{
    return (DuQFNode*)m_output->parentItem();
}


