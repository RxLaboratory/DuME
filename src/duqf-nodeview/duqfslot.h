#ifndef DUQFSLOT_H
#define DUQFSLOT_H

#include <QGraphicsObject>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QStaticText>

#include "duqf-app/app-style.h"

class DuQFNode;

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

    DuQFSlot(DuQFNode *node, SlotType type = All, bool singleConnection = false, QColor color = QColor(), QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) override;

    SlotType connectorType() const;

    bool isSingleConnection() const;
    void setSingleConnection(bool singleConnection);

    SlotType slotType() const;
    void setSlotType(const SlotType &slotType);

    // UI Settings
    static int height;

    void setTitle(const QString &newTitle);

    DuQFNode *node() const;

    const QString &userType() const;
    void setUserType(const QString &newUserType);

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
    QString m_title;
    QString m_userType;

    // Appearance
    QColor m_color;
    QRectF m_boundingRect;

    // Interactions
    bool m_hover = false;
    bool m_connecting = false;

    // Node
    DuQFNode *m_node = nullptr;
};

#endif // DUQFSLOT_H
