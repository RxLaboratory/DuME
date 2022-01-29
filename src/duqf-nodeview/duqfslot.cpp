#include "duqfslot.h"

int DuQFSlot::height = 16;

DuQFSlot::DuQFSlot(DuQFNode *node, SlotType type, bool singleConnection, QColor color, QGraphicsItem *parent):
    QGraphicsObject(parent)
{
    m_node = node;

    if (color.isValid()) m_color = color;
    else m_color = DuUI::getColor("dark-grey");

    m_boundingRect = QRectF(-DuQFSlot::height/2,-DuQFSlot::height/2,DuQFSlot::height,DuQFSlot::height);

    m_slotType = type;
    m_singleConnection = singleConnection;

    if (type == Input) setTitle("Input");
    else if (type == Output) setTitle("Output");

    setAcceptHoverEvents(true);
}

QRectF DuQFSlot::boundingRect() const
{
    return m_boundingRect;
}

void DuQFSlot::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->save();

    // Circle
    QRect circleRect = QRect(-DuQFSlot::height/2,-DuQFSlot::height/2,DuQFSlot::height,DuQFSlot::height);

    if (m_hover || m_connecting) painter->setBrush(QBrush(m_color));
    else painter->setBrush(QBrush(m_color.lighter(50)));
    QPen pen( DuUI::getColor("very-dark-grey") );
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawEllipse(circleRect.adjusted(3,3,-3,-3));

    m_boundingRect = circleRect;

    // Title
    if (m_title != "")
    {
        QStaticText titleText(m_title);
        int h = titleText.size().height();
        int w = titleText.size().width();
        int top = -h/2-2;
        int left;
        if (m_slotType == Output) left = circleRect.left() - 3 - w;
        else left = circleRect.right() + 3;

        QPen textPen( DuUI::getColor("less-light-grey") );
        QFont textFont( qApp->font() );
        painter->setPen( textPen );
        painter->setFont( textFont );

        painter->drawStaticText(left, top, titleText);

        // Add to bounds
        m_boundingRect = m_boundingRect.united( QRectF(left, top, w, h));
    }

    painter->restore();
}

void DuQFSlot::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit connectionInitiated(this->scenePos());
        m_connecting = true;
        event->accept();
        return;
    }

    QGraphicsObject::mousePressEvent(event);
    event->ignore();
}

void DuQFSlot::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_connecting)
    {
        emit connectionMoved(event->scenePos());
        event->accept();
        return;
    }

    QGraphicsObject::mousePressEvent(event);
    event->ignore();
}

void DuQFSlot::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_connecting)
    {
        emit connectionFinished(event->scenePos(), this->scenePos());
        event->accept();
        m_connecting = false;
        return;
    }

    QGraphicsObject::mousePressEvent(event);
    event->ignore();
}

void DuQFSlot::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    m_hover = true;
    update();
}

void DuQFSlot::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    m_hover = false;
    update();
}

const QString &DuQFSlot::userType() const
{
    return m_userType;
}

void DuQFSlot::setUserType(const QString &newUserType)
{
    m_userType = newUserType;
}

DuQFNode *DuQFSlot::node() const
{
    return m_node;
}

void DuQFSlot::setTitle(const QString &newTitle)
{
    m_title = newTitle;
    this->update();
}

DuQFSlot::SlotType DuQFSlot::slotType() const
{
    return m_slotType;
}

void DuQFSlot::setSlotType(const SlotType &slotType)
{
    m_slotType = slotType;
}

void DuQFSlot::remove()
{
    if (m_removing) return;
    m_removing = true;
    emit removed();
    deleteLater();
}

bool DuQFSlot::isSingleConnection() const
{
    return m_singleConnection;
}

void DuQFSlot::setSingleConnection(bool singleConnection)
{
    m_singleConnection = singleConnection;
}

DuQFSlot::SlotType DuQFSlot::connectorType() const
{
    return m_slotType;
}
