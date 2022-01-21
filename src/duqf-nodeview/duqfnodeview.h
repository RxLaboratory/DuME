
#ifndef DUQFNODEVIEW_H
#define DUQFNODEVIEW_H

#include <QGraphicsView>
#include <cmath>
#include <QGraphicsItemGroup>
#include <QGraphicsSceneHoverEvent>
#include <QGestureEvent>

#include "duqf-app/app-style.h"
#include "duqf-nodeview/duqfgrid.h"
#include "duqf-nodeview/duqfnodescene.h"

class DuQFNodeView : public QGraphicsView
{
    Q_OBJECT
public:
    enum Transformation { Zoom, Pan, None };
    Q_ENUM(Transformation)

    DuQFNodeView(QWidget *parent = nullptr);
    DuQFGrid *grid() const;
    void zoom(double amount);
    qreal currentZoom();

    DuQFNodeScene * nodeScene();

public slots:
    void frameSelected();
    void reinitTransform();
    void setZoom(int zoomPercent);

signals:
    void zoomed(qreal zoom);
    void zoomed(int zoomPercent);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    bool event(QEvent *event) override;
    bool gestureEvent(QGestureEvent *event);

private:
    void drawBackground(QPainter * painter, const QRectF & rect) override;

    // members
    DuQFGrid *m_grid;
    bool m_gridVisible = true;
    QRectF m_nodeBoundingRect;
    DuQFNodeScene *m_scene;

    // settings
    double m_zoomSensitivity = 0.2;

    // interaction
    bool m_ctrlKeyPressed = false;
    bool m_mouseMiddlePressed = false;
    Transformation m_mouseTransformation = None;
    QPointF m_mousePosition;
};

#endif // DUQFNODEVIEW_H
