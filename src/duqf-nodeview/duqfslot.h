#ifndef DUQFSLOT_H
#define DUQFSLOT_H

#include <QGraphicsObject>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>

#include "duqf-app/app-style.h"

class DuQFSlot : public QGraphicsObject
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    enum SlotType{ All = 0,
               Input = 1,
               Output = 2 };
    Q_ENUM(SlotType)

    enum { Type = UserType + 2 };
    int type() const override { return Type; }

    DuQFSlot(SlotType type = All, bool singleConnection = false, QColor color = QColor(), QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) override;

    SlotType connectorType() const;

    bool isSingleConnection() const;
    void setSingleConnection(bool singleConnection);

    SlotType slotType() const;
    void setSlotType(const SlotType &slotType);

public slots:
    void remove();

signals:
    void connectionInitiated(QPointF);
    void connectionMoved(QPointF);
    void connectionFinished(QPointF, QPointF);
    void removed();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    bool m_removing = false;

    // Attributes
    SlotType m_slotType;
    bool m_singleConnection;

    // Appearance
    QSizeF m_size;
    QColor m_color;
    QRectF m_boundingRect;

    // Interactions
    bool m_hover = false;
    bool m_connecting = false;
};

#endif // DUQFSLOT_H
