#ifndef DUQFCONNECTIONMANAGER_H
#define DUQFCONNECTIONMANAGER_H


#include "duqfconnection.h"

/**
 * @brief The DuQFConnectionManager class keesps the list of all DuQFConnections in the scene, and is responsible for their creation.
 */
class DuQFConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit DuQFConnectionManager(QObject *parent = nullptr);
    /**
     * @brief Adds a connexion to the management, without sanity checks (if it's not authorized, it will be added anyway)
     * @param connection
     */
    void addConnection(DuQFConnection *connection);
    /**
     * @brief Adds a connexion to the management, after checking if it is authorized. If not authorized, the connector is deleted.
     * @param output The output slot of the parent node
     * @param input The input slot of the child node
     * @param connection The connector used for this connection
     * @return True if the connection was accepted
     */
    DuQFConnection * addConnection(DuQFSlot *output, DuQFSlot *input);
    QList<DuQFConnection *> connections() const;

public slots:
    void removeConnection();

signals:
    void newConnection(DuQFConnection *);
    void connectionRemoved(DuQFConnection *);

private:
    QList<DuQFConnection*> m_connections;


};

#endif // DUQFCONNECTIONMANAGER_H
