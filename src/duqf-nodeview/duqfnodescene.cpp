#include "duqfnodescene.h"

DuQFNodeScene::DuQFNodeScene(DuQFGrid &grid, QObject *parent):
    QGraphicsScene(parent),
    m_grid(grid)
{
    m_connectionManager = new DuQFConnectionManager(this);
    setSceneRect(-m_initialSize, -m_initialSize, m_initialSize * 2, m_initialSize * 2);
}

void DuQFNodeScene::adjustSceneRect()
{
    while (!containsAll()) {
        setSceneRect(sceneRect().adjusted(-m_initialSize, -m_initialSize, m_initialSize, m_initialSize));
    }
}

QRectF DuQFNodeScene::zoomToFit(bool isForExport) const
{
    return calculateRectangle(isForExport);
}

QList<DuQFNode *> DuQFNodeScene::nodes()
{
    QList<DuQFNode*> nodes;
    foreach(QGraphicsItem *i, items())
    {
        DuQFNode *n = qgraphicsitem_cast<DuQFNode*>(i);
        if (n) nodes << n;
    }
    return nodes;
}

QList<DuQFNode *> DuQFNodeScene::selectedNodes()
{
    QList<DuQFNode*> nodes;
    foreach(QGraphicsItem *item, selectedItems())
    {
        DuQFNode *n = qgraphicsitem_cast<DuQFNode*>(item);
        if (n) if (n->isSelected()) nodes << n;
    }
    return nodes;
}

QGraphicsItemGroup *DuQFNodeScene::createNodeGroup(QList<DuQFNode *> nodes)
{
    QGraphicsItemGroup *g = new QGraphicsItemGroup();
    addItem(g);
    foreach(DuQFNode *n, nodes) g->addToGroup(n);

    return g;
}

void DuQFNodeScene::addNode()
{
    addNode(new DuQFNode());
}

void DuQFNodeScene::addNode(DuQFNode *node, bool select)
{
    // remove selection
    foreach(QGraphicsItem *i, items()) i->setSelected(false);

    node->setGrid(&m_grid);
    this->addItem(node);

    // Move node if needed
    qreal y = 0.0;
    qreal x = 0.0;

    // Check if there's already a node here
    DuQFNode *n = nullptr;
    foreach(QGraphicsItem *i, items( QPointF(x,y) ))
    {
        n = qgraphicsitem_cast<DuQFNode*>(i);
        if (n)
        {
            y += n->sceneBoundingRect().height() + m_grid.size();
            break;
        }
    }

    while (n)
    {
        n = nullptr;
        foreach(QGraphicsItem *i, items( QPointF(x,y) ))
        {
            n = qgraphicsitem_cast<DuQFNode*>(i);
            if (n)
            {
                y += n->sceneBoundingRect().height() + m_grid.size();
                break;
            }
        }
    }

    node->setPos(x, y);
    node->setSelected(select);

    connect(node, &DuQFNode::connectionInitiated, this, &DuQFNodeScene::initiateConnection);
    connect(node, &DuQFNode::connectionMoved, this, &DuQFNodeScene::moveConnection);
    connect(node, &DuQFNode::connectionFinished, this, &DuQFNodeScene::finishConnection);
    connect(node, &DuQFNode::removed, this, &DuQFNodeScene::nodeRemoved);
}

void DuQFNodeScene::removeSelectedConnections()
{
    foreach(QGraphicsItem *item, selectedItems())
    {
        DuQFConnector *c = qgraphicsitem_cast<DuQFConnector*>(item);
        if (c) c->remove();
    }
}

void DuQFNodeScene::removeSelectedNodes()
{
    foreach(DuQFNode *n, selectedNodes()) n->remove();
}

void DuQFNodeScene::removeSelection()
{
    removeSelectedNodes();
    removeSelectedConnections();
}

void DuQFNodeScene::autoLayoutAll()
{
    clearSelection();
    autoLayoutNodes(nodes());

    // Center all
    moveAllToCenter();
}

void DuQFNodeScene::autoLayoutSelectedNodes()
{
    autoLayoutNodes(selectedNodes());
}

void DuQFNodeScene::autoLayoutNodes(QList<DuQFNode *> nodes)
{
    // 1- Move orphan nodes appart
    qreal x = 0.0;
    qreal y = 0.0;

    QGraphicsItemGroup *orphanGroup = new QGraphicsItemGroup();
    addItem(orphanGroup);
    for (int i = nodes.count()-1; i >= 0; i--)
    {
        DuQFNode *n = nodes.at(i);
        if (n->isOrphan())
        {
            n->setPos( x, y );
            y += n->boundingRect().height() + m_grid.size();
            orphanGroup->addToGroup(n);
            nodes.removeAt(i);
        }
    }

    x = orphanGroup->boundingRect().width()*2 + m_grid.gridSize()*3;
    y = 0.0;
    destroyItemGroup(orphanGroup);

    // 2- If we still have nodes to arrange
    if (nodes.count() > 0)
    {
        // Find ancestors
        QList<DuQFNode*> ancestors;
        foreach(DuQFNode *n, nodes)
        {
            if (!n->hasParents())
            {
                ancestors << n;
                // Move right to orphans
                n->setPos( x, y);
            }
        }

        // If no ancestor (it's a loop), pick the first node
        if (ancestors.count() == 0)
        {
            DuQFNode *n = nodes.at(0);
            n->setPos( x, y);
            ancestors << n;
        }

        // Layout each generation one after the other
        while (ancestors.count() > 0 && nodes.count() > 0)
        {
            // For each ancestor, arrange children
            QList<DuQFNode*> nextGeneration;
            QList<QGraphicsItemGroup*> trees;

            foreach (DuQFNode *n, ancestors)
            {
                // This one is now ok
                nodes.removeAll(n);

                // Get children which are still to be arranged
                QList<DuQFNode *> childrenNodes;
                foreach(DuQFNode *n, n->childNodes())
                    if (nodes.contains(n)) childrenNodes << n;
                nextGeneration.append(childrenNodes);

                // Layout'em
                x = n->scenePos().x() + n->boundingRect().width()*2 + m_grid.gridSize()*3;
                y = n->scenePos().y() + n->boundingRect().height();
                layoutNodesInColumn( childrenNodes , x, y );

                QGraphicsItemGroup *tree = createNodeGroup( childrenNodes );
                tree->addToGroup(n);
                trees << tree;
            }
            layoutGroupsInColumn( trees );

            ancestors.clear();
            foreach(DuQFNode *n, nextGeneration)
                if (nodes.contains(n)) ancestors << n;
        }
    }
}

void DuQFNodeScene::moveAllToCenter()
{
    QGraphicsItemGroup *group = createNodeGroup(nodes());
    QRectF rect = group->boundingRect();
    group->setPos(-rect.width()/2, -rect.height()/2);
    destroyItemGroup(group);
}

void DuQFNodeScene::selectAllNodes()
{
    clearSelection();
    foreach(DuQFNode *n, nodes())
    {
        n->setSelected(true);
    }
}

void DuQFNodeScene::selectNodes(QList<DuQFNode*> nodes)
{
    foreach(DuQFNode *n, nodes)
    {
        n->setSelected(true);
    }
}

void DuQFNodeScene::selectChildNodes()
{
    foreach(QGraphicsItem *i, selectedItems())
    {
        DuQFNode *n = qgraphicsitem_cast<DuQFNode*>(i);
        if (n) if (n->isSelected()) selectNodes(n->childNodes());
    }
}

void DuQFNodeScene::selectParentNodes()
{
    foreach(QGraphicsItem *i, selectedItems())
    {
        DuQFNode *n = qgraphicsitem_cast<DuQFNode*>(i);
        if (n) if (n->isSelected()) selectNodes(n->parentNodes());
    }
}

DuQFConnection * DuQFNodeScene::connectNodes(DuQFNode *o, DuQFNode *i)
{
    DuQFConnection *co = m_connectionManager->addConnection(o->defaultOutputSlot(), i->defaultInputSlot());
    //add the graphic item connector
    if (co) this->addItem( co->connector() );
    return co;
}

void DuQFNodeScene::moveConnection(QPointF to)
{
    // Snap
    // Check the item we're connecting to
    foreach(QGraphicsItem *item, items(to))
    {
        DuQFSlot *input = qgraphicsitem_cast<DuQFSlot*>(item);
        if(input)
        {
            to = input->scenePos();
            break;
        }
    }

    if (m_connecting && m_connectingItem)
    {
        m_connectingItem->setTo(to);
    }
}

void DuQFNodeScene::finishConnection(QPointF to, QPointF from)
{
    m_connecting = false;

    // Check the item we're connecting to
    DuQFSlot *input = nullptr;
    foreach(QGraphicsItem *item, items(to))
    {
        input = qgraphicsitem_cast<DuQFSlot*>(item);
        if(input) break;
    }

    // Find the item we're connecting from
    DuQFSlot *output = nullptr;
    foreach(QGraphicsItem *item, items(from))
    {
        output = qgraphicsitem_cast<DuQFSlot*>(item);
        if(output) break;
    }

    // Connect
    m_connectionManager->addConnection(output, input);
    m_connectingItem->remove();
}

void DuQFNodeScene::nodeRemoved(DuQFNode *node)
{
    this->removeItem(node);
    //node->deleteLater();
}

void DuQFNodeScene::layoutNodesInColumn(QList<DuQFNode *> nodes, QPointF center)
{
    layoutNodesInColumn(nodes, center.x(), center.y());
}

void DuQFNodeScene::layoutNodesInColumn(QList<DuQFNode *> nodes, qreal x, qreal y)
{
    if (nodes.count() == 0) return;

    // Get the total height (initialize with spacing)
    qreal h = m_grid.gridSize() * (nodes.count() -1)/2;
    foreach(DuQFNode *node, nodes) h += node->boundingRect().height();

    y -= h/2;
    // set y
    foreach(DuQFNode *node, nodes)
    {
        node->setPos( x,y );
        y += node->boundingRect().height() + m_grid.gridSize()/2;
    }
}

void DuQFNodeScene::layoutGroupsInColumn(QList<QGraphicsItemGroup *> groups)
{
    if (groups.count() == 0) return;
    // for each item, arrange vertically
    // Get the total height (initialize with spacing)
    qreal h = m_grid.gridSize() * (groups.count() -1)/2;
    qreal y = 0.0;
    qreal x = 0.0;
    foreach(QGraphicsItemGroup *item, groups)
    {
        h += item->boundingRect().height();
        y += item->scenePos().y();
        x += item->scenePos().x();
    }
    // Get the mean pos as initial coordinate
    y /= groups.count();
    x /= groups.count();
    // First item y pos
    y -= h/2;
    // set y
    foreach(QGraphicsItemGroup *item, groups)
    {
        item->setPos( x, y );
        y += item->boundingRect().height() + m_grid.gridSize()/2;
        destroyItemGroup(item);
    }
}

void DuQFNodeScene::initiateConnection(QPointF from)
{
    m_connecting = true;

    DuQFConnector *c = new DuQFConnector(from);
    this->addItem(c);
    m_connectingItem = c;
}

bool DuQFNodeScene::containsAll() const
{
    //const double testMarginX = sceneRect().width() * m_margin;
    //const double testMarginY = sceneRect().height() * m_margin;
    //const QRectF testRect = sceneRect().adjusted(testMarginX, testMarginY, -testMarginX, -testMarginY);
    /*for (auto && item : items()) {
        if (dynamic_cast<Node *>(item)) {
            if (!testRect.contains(item->sceneBoundingRect())) {
                return false;
            }
        }
    }*/
    return true;
}

QRectF DuQFNodeScene::calculateRectangle(bool isForExport) const
{
    double nodeArea = 0;
    QRectF rect;
    int nodes = 0;
    /*for (auto && item : scene.items()) {
        if (auto node = dynamic_cast<Node *>(item)) {
            const auto nodeRect = node->placementBoundingRect();
            rect = rect.united(nodeRect.translated(node->pos().x(), node->pos().y()));
            nodeArea += nodeRect.width() * nodeRect.height();
            nodes++;
        }
    }*/

    const int margin = 60;

    if (isForExport) {
        return rect.adjusted(-margin, -margin, margin, margin);
    }

    // This "don't ask" heuristics tries to calculate a "nice" zoom-to-fit based on the design
    // density and node count. For example, if we have just a single node we don't want it to
    // be super big and cover the whole screen.
    const double density = nodeArea / rect.width() / rect.height();
    const double adjust = 3.0 * std::max(density * rect.width(), density * rect.height()) / pow(nodes, 1.5);
    return rect.adjusted(-adjust / 2, -adjust / 2, adjust / 2, adjust / 2).adjusted(-margin, -margin, margin, margin);
}

DuQFConnectionManager *DuQFNodeScene::connectionManager() const
{
    return m_connectionManager;
}

