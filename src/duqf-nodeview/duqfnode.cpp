#include "duqfnode.h"

DuQFNode::DuQFNode(QString title, QGraphicsItem *parent):
    QGraphicsObject(parent)
{
    setupUi(title);
    addInput();
    addOutput();
}

DuQFNode::DuQFNode(int numInputs, int numOutputs, QString title, QGraphicsItem *parent):
    QGraphicsObject(parent)
{
    setupUi(title);

    // Add slots
    for (int i = 0; i < numInputs; i++) addInput();
    for (int i = 0; i < numOutputs; i++) addOutput();
}

QRectF DuQFNode::boundingRect() const
{
    // Icon rect
    QRectF rect = m_iconItem->boundingRect();
    rect.setWidth( rect.width() * m_iconItem->scale() );

    //add padding between icon & text
    if (m_iconItem->boundingRect().width() > 0)
        rect.adjust(0,0,m_padding, 0);

    //add text width
    rect.adjust(0,0, + m_titleItem->boundingRect().width(),0);

    //set correct height
    rect.setHeight( m_titleItem->boundingRect().height() );

    // Add slots
    int numSlots = std::max(m_inputSlots.count(), m_outputSlots.count());

    rect.setHeight(rect.height() + numSlots*(m_padding + DuQFSlot::height));

    //set margins
    rect.adjust(-m_padding-7, -m_padding-3, m_padding+7, m_padding+3);

    return rect;
}

void DuQFNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->save();

    // Background

    QPainterPath path;
    //const QRectF rect(-m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height());
    const QRectF rect = boundingRect().adjusted(2,2,-2,-2);
    path.addRoundedRect(rect, m_cornerRadius, m_cornerRadius);
    painter->setRenderHint(QPainter::Antialiasing);

    const QBrush brush( DuUI::getColor("medium-grey") );
    painter->fillPath(path, brush);

    // Selection Stroke

    if (isSelected())
    {
        QPen pen( DuUI::getColor("less-light-grey") );
        pen.setWidth(2);
        painter->strokePath(path, pen);
    }

    // Inputs

    painter->restore();
}

QVariant DuQFNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene() && m_grid)
    {
        emit moved(value.toPointF());
        return m_grid->snapToGrid(value.toPointF());
    }

    return QGraphicsItem::itemChange(change, value);
}

void DuQFNode::setupUi(QString title)
{
    setAcceptHoverEvents(true);
    m_grid = nullptr;

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10.0);
    shadow->setColor(DuUI::getColor("obsidian"));
    shadow->setOffset(5.0,5.0);
    setGraphicsEffect(shadow);

    m_cornerRadius = DuUI::getSize( "padding", "small" );

    setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable | ItemSendsGeometryChanges);
    setSelected(false);

    // Title

    m_titleItem = new QGraphicsTextItem(title);
    QFont f = qApp->font();
    f.setPixelSize( DuUI::getSize("font", "size-small") );
    f.setWeight(QFont::Bold);
    m_titleItem->setFont(f);
    m_titleItem->setDefaultTextColor(DuUI::getColor("light-grey"));
    m_titleItem->setParentItem(this);
    m_padding = DuUI::getSize("padding", "small");
    //m_titleItem->setPos(m_padding, m_padding);

    // Icon

    m_iconItem = new QGraphicsSvgItem(this);

    setTitle(title);

    this->setObjectName( "DuQFNode" );
}

void DuQFNode::updateSlotsPos()
{
    QRectF rect = m_titleItem->boundingRect().adjusted(-m_padding-7, -m_padding-3, m_padding+7, m_padding+3);

    // Update slots position
    qreal right = rect.right()-2;
    if (m_iconItem->boundingRect().width() != 0)
        right += m_iconItem->boundingRect().right() * m_iconItem->scale() + m_padding;
    qreal left = rect.left()+2;
    qreal top = rect.bottom() + m_padding;

    for (int i = 0; i < m_inputSlots.count(); i++)
    {
        DuQFSlot *slot = m_inputSlots.at(i);
        slot->setPos(left, top);
        top += DuQFSlot::height + m_padding;
    }

    top = rect.bottom() + m_padding;
    for (int i = 0; i < m_outputSlots.count(); i++)
    {
        DuQFSlot *slot = m_outputSlots.at(i);
        slot->setPos(right, top);
        top += DuQFSlot::height + m_padding;
    }
}

const QList<DuQFSlot *> &DuQFNode::outputSlots() const
{
    return m_outputSlots;
}

const QList<DuQFSlot *> &DuQFNode::inputSlots() const
{
    return m_inputSlots;
}

QList<DuQFNode *> DuQFNode::parentNodes() const
{
    return m_parentNodes;
}

bool DuQFNode::isOrphan() const
{
    return !hasChildren() && !hasParents();
}

bool DuQFNode::hasChildren() const
{
    return m_childrenNodes.count() != 0;
}

bool DuQFNode::hasParents() const
{
    return m_parentNodes.count() != 0;
}

QList<DuQFNode *> DuQFNode::childNodes() const
{
    return m_childrenNodes;
}

DuQFGrid *DuQFNode::grid() const
{
    return m_grid;
}

void DuQFNode::addChildNode(DuQFNode *childNode)
{
    m_childrenNodes << childNode;
}

void DuQFNode::addParentNode(DuQFNode *parentNode)
{
    m_parentNodes << parentNode;
}

void DuQFNode::removeChildNode(DuQFNode *childNode)
{
    m_childrenNodes.removeAll(childNode);
}

void DuQFNode::removeParentNode(DuQFNode *parentNode)
{
    m_parentNodes.removeAll(parentNode);
}

void DuQFNode::setGrid(DuQFGrid *grid)
{
    m_grid = grid;
}

QString DuQFNode::title() const
{
    return m_titleItem->toPlainText();
}

void DuQFNode::setTitle(const QString &title)
{
    m_titleItem->setPlainText(title);
    updateSlotsPos();
}

void DuQFNode::setTitleColor(const QColor &color)
{
    m_titleItem->setDefaultTextColor(color.lighter(115));
}

QString DuQFNode::titleToolTip() const
{
    return m_titleItem->toolTip();
}

void DuQFNode::setTitleToolTip(const QString &toolTip)
{
    m_titleItem->setToolTip(toolTip);
}

void DuQFNode::setIcon(QString icon)
{
    qreal size = m_titleItem->boundingRect().height();

    m_iconItem->deleteLater();
    m_iconItem = new QGraphicsSvgItem(icon, this);

    qreal currentHeight = m_iconItem->boundingRect().height();
    qreal width = m_iconItem->boundingRect().width();

    if (size != 0 && currentHeight != 0)
    {
        qreal scale = size/currentHeight;
        m_iconItem->setScale(scale);
        width = width*scale;
    }

    m_titleItem->setPos(width + m_padding, 0);

    updateSlotsPos();
}

void DuQFNode::addInput(QString inputName, QColor color)
{
    if (!color.isValid()) color =  DuUI::getColor("light-blue");

    // Create Slot
    DuQFSlot *slot = new DuQFSlot( this, DuQFSlot::Input, false, color );
    slot->setTitle(inputName);
    slot->setParentItem(this);
    m_inputSlots.append(slot);

    updateSlotsPos();

    // Connect
    connect(slot, &DuQFSlot::connectionInitiated, this, &DuQFNode::connectionInitiated);
    connect(slot, &DuQFSlot::connectionMoved, this, &DuQFNode::connectionMoved);
    connect(slot, &DuQFSlot::connectionFinished, this, &DuQFNode::connectionFinished);
}

void DuQFNode::addOutput(QString outputName, QColor color)
{
    if (!color.isValid()) color =  DuUI::getColor("light-green");

    // Create Slot
    DuQFSlot *slot = new DuQFSlot( this, DuQFSlot::Output, false, color );
    slot->setTitle(outputName);
    slot->setParentItem(this);
    m_outputSlots.append(slot);

    updateSlotsPos();

    // Connect
    connect(slot, &DuQFSlot::connectionInitiated, this, &DuQFNode::connectionInitiated);
    connect(slot, &DuQFSlot::connectionMoved, this, &DuQFNode::connectionMoved);
    connect(slot, &DuQFSlot::connectionFinished, this, &DuQFNode::connectionFinished);
}

void DuQFNode::remove()
{
    qDebug() << "Removing DuQFNode";
    qDebug().noquote() << "- " + this->objectName();
    if (m_removing) return;
    qDebug() << "> Accepted";
    m_removing = true;
    this->prepareGeometryChange();

    while (!m_inputSlots.isEmpty())  m_inputSlots.takeLast()->remove();
    while (!m_outputSlots.isEmpty()) m_outputSlots.takeLast()->remove();

    emit removed(this);
    qDebug() << "> DuQFNode removed";

    // There seems to be an issue with deleting QGraphicsObject from itself
    // The containing scene will handle that using the removed signal
    //this->deleteLater();
}
