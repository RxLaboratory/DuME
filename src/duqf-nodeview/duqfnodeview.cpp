#include "duqfnodeview.h"

DuQFNodeView::DuQFNodeView(QWidget *parent): QGraphicsView(parent)
{
    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);

    setRenderHint(QPainter::Antialiasing);

    setFrameShape(QFrame::NoFrame);

    setDragMode(QGraphicsView::RubberBandDrag);

    setFocusPolicy(Qt::StrongFocus);

    // Gestures (maybe later)
    //grabGesture(Qt::PanGesture);
    //grabGesture(Qt::PinchGesture);

    m_grid = new DuQFGrid(this);
    m_scene = new DuQFNodeScene(*m_grid);
    setScene(m_scene);

    connect(m_grid, SIGNAL(gridSizeChanged()), this, SLOT(update()));

    this->setAcceptDrops(true);
}

void DuQFNodeView::setDropOK(bool ok)
{
    m_acceptDrops = ok;
}

DuQFGrid *DuQFNodeView::grid() const
{
    return m_grid;
}

void DuQFNodeView::zoom(double amount)
{
    qreal s = 1.0 + amount / 10.0 * m_zoomSensitivity;
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    double z = currentZoom();
    if (z*s < 0.25) return;

    scale( s, s );

    z = currentZoom();
    emit zoomed(z);
    emit zoomed(int(z*100));
}

qreal DuQFNodeView::currentZoom()
{
    QPoint p1 = mapFromScene(QPointF(1000.0,0.0));
    QPoint p2 = mapFromScene(QPointF(0.0,0.0));
    return qreal(p1.x() - p2.x()) / 1000.0;
}

DuQFNodeScene *DuQFNodeView::nodeScene()
{
    return m_scene;
}

void DuQFNodeView::frameSelected()
{
    QRectF boundingRect;
    QGraphicsItemGroup *group = nullptr;
    if (scene()->selectedItems().length() > 0)
    {
        group = scene()->createItemGroup( scene()->selectedItems() );
        boundingRect = group->boundingRect().adjusted(-30,-30,30,30);
    }
    else
    {
        boundingRect = scene()->itemsBoundingRect().adjusted(-30,-30,30,30);
    }

    fitInView( boundingRect, Qt::KeepAspectRatio);

    if (group) scene()->destroyItemGroup(group);

    double z = currentZoom();
    emit zoomed(z);
    emit zoomed(int(z*100));
}

void DuQFNodeView::reinitTransform()
{
    setTransform(QTransform());
    centerOn(QPointF(0.0,0.0));

    double z = currentZoom();
    emit zoomed(z);
    emit zoomed(int(z*100));
}

void DuQFNodeView::setZoom(int zoomPercent)
{
    QTransform t = transform();
    qreal s = qreal(zoomPercent) / 100.0;
    t.setMatrix(s, t.m12(), t.m13(), t.m21(), s, t.m23(), t.m31(), t.m32(), t.m33() );
    setTransform(t);
}

void DuQFNodeView::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta();

    if (!numPixels.isNull())
    {
        zoom( numPixels.y() );
    }
    else if (!numDegrees.isNull())
    {
        zoom( double(numDegrees.y())/90.0 );
    }

    event->accept();
}

void DuQFNodeView::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();

    if (key == Qt::Key_Period || key == Qt::Key_F) {
        frameSelected();
        event->accept();
        return;
    }

    if (key == Qt::Key_Home) {
        reinitTransform();
        event->accept();
        return;
    }

    if (key == Qt::Key_Backspace || key == Qt::Key_Delete) {
        m_scene->removeSelection();
        event->accept();
        return;
    }

    // Default action
    QGraphicsView::keyPressEvent(event);

    event->ignore();
}

void DuQFNodeView::keyReleaseEvent(QKeyEvent *event)
{
    // Default action
    QGraphicsView::keyReleaseEvent(event);

    event->ignore();
}

void DuQFNodeView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MidButton)
    {
        m_mouseMiddlePressed = true;
        if (event->modifiers().testFlag(Qt::ControlModifier))
        {
            m_mouseTransformation = Zoom;
        }
        else
        {
            m_mouseTransformation = Pan;
        }
        m_mousePosition = event->pos();
        event->accept();
        return;
    }

    // Default action
    QGraphicsView::mousePressEvent(event);

    event->ignore();
}

void DuQFNodeView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MidButton)
    {
        m_mouseMiddlePressed = false;
        m_mouseTransformation = None;
        event->accept();
        return;
    }

    // Default action
    QGraphicsView::mouseReleaseEvent(event);

    event->ignore();
}

void DuQFNodeView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mouseTransformation == Zoom)
    {
        const qreal dist = m_mousePosition.y() - event->pos().y();
        zoom(dist);
        m_mousePosition = event->pos();
    }
    else if (m_mouseTransformation == Pan)
    {
        qreal zoomFactor = currentZoom();
        qreal x = event->pos().x() - m_mousePosition.x();
        qreal y = event->pos().y() - m_mousePosition.y();
        setTransformationAnchor(QGraphicsView::NoAnchor);
        translate(x / zoomFactor, y / zoomFactor);
        m_mousePosition = event->pos();
    }

    // Default action
    QGraphicsView::mouseMoveEvent(event);

    event->ignore();
}

bool DuQFNodeView::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
            return gestureEvent(static_cast<QGestureEvent*>(event));

    return QGraphicsView::event(event);
}

void DuQFNodeView::dropEvent(QDropEvent *event)
{
    if (m_acceptDrops) {
        setCursor(Qt::ArrowCursor);
        event->acceptProposedAction();
        emit drop(event);
    }
}

void DuQFNodeView::dragEnterEvent(QDragEnterEvent *event)
{
    if (m_acceptDrops) {
        event->acceptProposedAction();
        setCursor(Qt::DragMoveCursor);
        emit dragEnter(event);
    }
}

void DuQFNodeView::dragMoveEvent(QDragMoveEvent *event)
{
    if (m_acceptDrops) {
        event->acceptProposedAction();
        emit dragMove(event);
    }
}

void DuQFNodeView::dragLeaveEvent(QDragLeaveEvent *event)
{
    if (m_acceptDrops) {
        event->accept();
        emit dragLeave(event);
    }
}


bool DuQFNodeView::gestureEvent(QGestureEvent *event)
{
    if (QGesture *pan = event->gesture(Qt::PanGesture))
    {
        QPanGesture *gesture = static_cast<QPanGesture*>( pan );
        qDebug() << gesture->state();
    }

    return false;
}

void DuQFNodeView::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->save();

    painter->fillRect(rect, QBrush( DuUI::getColor("very-dark-grey")  ) );

    const int gridSize = m_grid->size();

    if (m_gridVisible && gridSize != 0)
    {
        const qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
        const qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

        QVector<QLineF> lines;


        const qreal lineLength = 2.5;
        qreal x = left;
        while (x < rect.right() )
        {
            qreal y = top;
            while (y < rect.bottom() )
            {
                lines.append(QLineF( x-lineLength, y, x+lineLength, y));
                lines.append(QLineF( x, y-lineLength, x, y+lineLength));
                y += gridSize;
            }
            x += gridSize;
        }

        if (hasFocus())
        {
             QPen pen;
            pen.setColor( DuUI::getColor("dark-grey") );
            pen.setWidthF(1 / currentZoom());
            painter->setPen(pen);
            painter->drawLines(lines);
        }
    }

    painter->restore();
}

