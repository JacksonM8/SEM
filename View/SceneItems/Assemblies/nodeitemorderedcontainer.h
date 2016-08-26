#ifndef NODEITEMORDEREDCONTAINER_H
#define NODEITEMORDEREDCONTAINER_H

#include "../containerelementnodeitem.h"

class ContainerNodeItem : public ContainerElementNodeItem
{
    Q_OBJECT
public:
    ContainerNodeItem(NodeViewItem* viewItem, NodeItemNew* parentItem);

    bool isSortOrdered() const;
    void setSortOrdered(bool ordered);
    QRectF bodyRect() const;
    //Pure virtual functions.
    virtual QPointF getElementPosition(ContainerElementNodeItem* child) = 0;
    virtual QPoint getElementIndex(ContainerElementNodeItem* child) = 0;

    // QGraphicsItem interface
public:
    QRectF getElementRect(ELEMENT_RECT rect) const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QRectF headerRect() const;
    QRectF innerHeaderRect() const;

    QRectF headerTextRect() const;
    QRectF iconRect() const;

    QRectF topTextRect() const;
    QRectF bottomTextRect() const;

    QRectF deployedRect() const;
    QRectF qosRect() const;
    QRectF expandStateRect() const;

    bool _isSortOrdered;
    QMarginsF headerMargin;
};

#endif // NODEITEMORDEREDCONTAINER_H
