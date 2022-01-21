#ifndef DUQFCONNECTION_H
#define DUQFCONNECTION_H

#include <QObject>

#include "duqfnode.h"
#include "duqfconnector.h"

/**
 * @brief The DuQFConnection class represents a connexion between two slots and transmits the move signals to the DuQFConnector
 */
class DuQFConnection : public QObject
{
    Q_OBJECT
public:
    explicit DuQFConnection(DuQFSlot *output, DuQFSlot *input, QObject *parent = nullptr);
    explicit DuQFConnection(DuQFSlot *output, DuQFSlot *input, DuQFConnector *connector, QObject *parent = nullptr);

    DuQFSlot *input() const;
    void setInput(DuQFSlot *input);

    DuQFSlot *output() const;
    void setOutput(DuQFSlot *output);

    DuQFConnector *connector() const;

    DuQFNode *inputNode();
    DuQFNode *outputNode();

public slots:
    void remove();

signals:
    void removed();

private slots:
    void outputMoved();
    void inputMoved();

private:
    void init(DuQFSlot *output, DuQFSlot *input, DuQFConnector *connector);
    bool m_removing = false;
    DuQFSlot *m_input;
    DuQFSlot *m_output;
    DuQFConnector *m_connector;
    QList<QMetaObject::Connection> m_inputConnections;
    QList<QMetaObject::Connection> m_outputConnections;
};

#endif // DUQFCONNECTIONMANAGER_H
