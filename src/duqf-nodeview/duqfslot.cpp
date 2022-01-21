#include "duqfslot.h"

DuQFSlot::DuQFSlot(SlotType type, bool singleConnection, QColor color, QGraphicsItem *parent):
    QGraphicsObject(parent)
{
    int s = DuUI::getSize("padding", "small") * 2;
    m_size = QSizeF(s,s);

    if (color.isValid()) m_color = color;
    else m_color = DuUI::getColor("dark-grey");

    m_slotType = type;
    m_singleConnection = singleConnection;

    m_boundingRect = QRectF(-m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height());

    setAcceptHoverEvents(true);
}

QRectF DuQFSlot::boundingRect() const
{
    return m_boundingRect.adjusted(-10,-10,10,10);
}

void DuQFSlot::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->save();

    // Background
    if (m_hover || m_connecting) painter->setBrush(QBrush(m_color));
    else painter->setBrush(QBrush(m_color.lighter(50)));
    QPen pen( DuUI::getColor("very-dark-grey") );
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawEllipse(m_boundingRect);

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
