#include "duqfconnectionmanager.h"

DuQFConnectionManager::DuQFConnectionManager(QObject *parent) : QObject(parent)
{

}

void DuQFConnectionManager::addConnection(DuQFConnection *connection)
{
    connect(connection, &DuQFConnection::removed, this, &DuQFConnectionManager::removeConnection);
    m_connections << connection;
    emit newConnection(connection);
}

DuQFConnection * DuQFConnectionManager::addConnection(DuQFSlot *output, DuQFSlot *input)
{
    // If one slot is missing
    if (!input || !output) return nullptr;

    // If input and output are the same
    if (input == output) return nullptr;

    // If self connection
    if (output->node() == input->node()) return nullptr;

    // Won't connect two inputs or two outputs together
    if (input->slotType() && input->slotType() == output->slotType()) return nullptr;

    // Check if a connection already exists with the same input/output
    foreach(DuQFConnection *c, m_connections)
    {
        if (input == c->input() && output == c->output()) return c;
        if (input == c->input() && input->isSingleConnection()) return nullptr;
        if (output  == c->output() && output->isSingleConnection()) return nullptr;
    }

    DuQFConnection *co =  new DuQFConnection(output, input, this);
    addConnection( co );
    return co;
}

QList<DuQFConnection *> DuQFConnectionManager::connections() const
{
    return m_connections;
}

void DuQFConnectionManager::removeConnection()
{
    DuQFConnection *c = (DuQFConnection*)sender();
    emit connectionRemoved(c);
    m_connections.removeAll(c);
}
