#include "duqfconnector.h"

DuQFConnector::DuQFConnector(QString title, QGraphicsItem *parent):
    QGraphicsObject( parent )
{
    setupUi();

    m_from = QPointF();
    m_to = m_from;
    m_width = DuUI::getSize("margin");
    setZValue(-1000);

    setFlags(ItemIsSelectable);

    m_titleItem->setPlainText(title);
}

DuQFConnector::DuQFConnector(QPointF from, QString title)
{
    setupUi();

    m_from = from;
    m_to = m_from;
    m_width = DuUI::getSize("margin");
    setZValue(-1000);

    setFlags(ItemIsSelectable);

    m_titleItem->setPlainText(title);
}

QRectF DuQFConnector::boundingRect() const
{

    qreal x;
    qreal y;
    qreal w;
    qreal h;

    if ( m_from.x() < m_to.x() )
    {
        x = m_from.x();
        w = m_to.x() - m_from.x();
    }
    else
    {
        x = m_to.x();
        w = m_from.x() - m_to.x();
    }

    if ( m_from.y() < m_to.y() )
    {
        y = m_from.y();
        h = m_to.y() - m_from.y();
    }
    else
    {
        y = m_to.y();
        h = m_from.y() - m_to.y();
    }

    return QRectF(x, y, w, h).adjusted(-m_width - m_titleItem->boundingRect().width(), -m_width - m_titleItem->boundingRect().height(), m_width + m_titleItem->boundingRect().width(), m_width + m_titleItem->boundingRect().height());
}

QPainterPath DuQFConnector::shape() const
{
    QPainterPath path;
    QRectF rect = QRectF(m_from, m_to);

    if (rect.height() > 20 && rect.width() > 20)
    {
        QPolygonF polygon;
        polygon << m_from;
        polygon << m_fromHandle;
        polygon << m_to;
        polygon << m_toHandle;
        path.addPolygon(polygon);
    }
    else
    {
        path.addRect( rect.adjusted( -10,-10,10,10));
    }

    return path;
}

void DuQFConnector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->save();

    // Get handle coordinates
    float handleWeight = QSettings().value("nodeViews/curvature", 0.5).toFloat();
    m_fromHandle.setX((m_from.x()*(1.0-handleWeight) + m_to.x()*handleWeight));
    m_fromHandle.setY(m_from.y());
    m_toHandle.setX((m_from.x()*handleWeight + m_to.x()*(1.0-handleWeight)));
    m_toHandle.setY(m_to.y());

    // Create path
    QPainterPath path( QPointF( m_from.x(), m_from.y() ) );
    path.cubicTo( m_fromHandle, m_toHandle, m_to);

    // And draw
    painter->setBrush(QBrush(Qt::transparent));
    QPen pen;
    if (isSelected()) pen.setColor( DuUI::getColor("light-grey") );
    else pen.setColor( DuUI::getColor("less-light-grey") );
    pen.setWidth( m_width );
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);

    painter->drawPath(path);

    // Title Background
    if (title() != "")
    {
        QPainterPath path;
        const QRectF rect(m_titleItem->pos().x(),
                          m_titleItem->pos().y(),
                          m_titleItem->boundingRect().width() +2,
                          m_titleItem->boundingRect().height() + 2);
        path.addRoundedRect(rect, m_cornerRadius, m_cornerRadius);
         painter->setRenderHint(QPainter::Antialiasing);

         const QBrush brush( DuUI::getColor("less-light-grey") );
         painter->fillPath(path, brush);

         // Selection Stroke

         if (isSelected())
         {
             QPen pen( DuUI::getColor("light-grey") );
             pen.setWidth(2);
             painter->strokePath(path, pen);
         }
    }



    painter->restore();
}

QString DuQFConnector::title() const
{
    return m_titleItem->toPlainText();
}

void DuQFConnector::setTitle(const QString &title)
{
    m_titleItem->setPlainText(title);
    updateTitlePos();
}

void DuQFConnector::setTo(const QPointF &to)
{
    m_to = to;
    updateTitlePos();
    prepareGeometryChange();
}

void DuQFConnector::setFrom(const QPointF &from)
{
    m_from = from;
    updateTitlePos();
    prepareGeometryChange();
}

void DuQFConnector::remove()
{
    if (m_removing) return;
    m_removing = true;
    emit removed();
    deleteLater();
}

QVariant DuQFConnector::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedChange && scene())
        emit selected(value.toBool());

    return QGraphicsItem::itemChange(change, value);
}

void DuQFConnector::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit clicked();
    QGraphicsObject::mouseReleaseEvent(event);
}

void DuQFConnector::setupUi()
{
    m_titleItem = new QGraphicsTextItem("");
    QFont f = qApp->font();
    f.setPixelSize( DuUI::getSize("font", "size-small") );
    m_titleItem->setFont(f);
    m_titleItem->setDefaultTextColor(DuUI::getColor("dark-grey"));
    m_titleItem->setParentItem(this);
    m_padding = DuUI::getSize("padding", "small");
}

void DuQFConnector::updateTitlePos()
{
    QPointF p = (m_from + m_to) / 2;
    // Adjust with text size
    p = p - m_titleItem->boundingRect().center();
    m_titleItem->setPos(p);
    //m_titleItem->setPos(boundingRect().width() / 2 - m_titleItem->boundingRect().width() / 2 + pos().x(),
    //                    boundingRect().height() / 2 - m_titleItem->boundingRect().height() / 2 + pos().y());
}
