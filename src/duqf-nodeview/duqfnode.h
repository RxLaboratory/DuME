#ifndef DUQFNODE_H
#define DUQFNODE_H

#include <QGraphicsObject>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QGraphicsSvgItem>
#include <QApplication>

#include "duqf-app/app-style.h"

#include "duqfslot.h"
#include "duqfgrid.h"

class DuQFNode : public QGraphicsObject
{
    Q_OBJECT
public:
    DuQFNode(QString title = "Node", QGraphicsItem *parent = nullptr);
    DuQFNode(int numInputs, int numOutputs, QString title = "Node", QGraphicsItem *parent = nullptr);

    enum { Type = UserType + 3 };
    int type() const override { return Type; }

    QRectF boundingRect() const override;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) override;

    QString title() const;
    void setTitle(const QString &title);
    void setTitleColor(const QColor &color);
    QString titleToolTip() const;
    void setTitleToolTip(const QString &toolTip);
    void setIcon(QString icon);

    void addInput(QString inputName = "Input", QColor color = QColor());
    void addOutput(QString outputName = "Output", QColor color = QColor());

    void setGrid(DuQFGrid *grid);
    DuQFGrid *grid() const;

    void addChildNode(DuQFNode *childNode);
    void addParentNode(DuQFNode *parentNode);
    void removeChildNode(DuQFNode *childNode);
    void removeParentNode(DuQFNode *parentNode);

    QList<DuQFNode *> childNodes() const;
    QList<DuQFNode *> parentNodes() const;
    bool isOrphan() const;
    bool hasChildren() const;
    bool hasParents() const;

    DuQFSlot *defaultInputSlot() const;
    DuQFSlot *defaultOutputSlot() const;

    const QList<DuQFSlot *> &inputSlots() const;

    const QList<DuQFSlot *> &outputSlots() const;

public slots:
    void remove();

signals:
    void connectionInitiated(QPointF);
    void connectionMoved(QPointF);
    void connectionFinished(QPointF, QPointF);
    void removed(DuQFNode*);
    void moved(QPointF);

protected:
    //void mousePressEvent(QGraphicsSceneMouseEvent * event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    void setupUi(QString title);

    DuQFGrid *m_grid;

    bool m_removing = false;
    // Appearance
    int m_cornerRadius = 5;
    int m_padding = 5;
    QRectF m_boundingRect;

    void updateSlotsPos();

    // Children
    QGraphicsTextItem *m_titleItem;
    QGraphicsSvgItem *m_iconItem;

    // Slots
    QList<DuQFSlot*> m_inputSlots;
    QList<DuQFSlot*> m_outputSlots;

    // Connected nodes
    QList<DuQFNode*> m_childrenNodes;
    QList<DuQFNode*> m_parentNodes;
};

#endif // DUQFNODE_H
