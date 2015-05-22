#ifndef NODECONNECTION_H
#define NODECONNECTION_H

#include <QGraphicsItem>
#include <QObject>

#include "nodeitem.h"
#include "../../Model/node.h"
#include "../../Model/edge.h"
#include "../../Model/graphmldata.h"


class EdgeItem: public GraphMLItem
{
    Q_OBJECT

public:
    enum LINE_SIDE{LEFT,RIGHT};
    enum LINE_DIRECTION{UP, DOWN};
    EdgeItem(Edge *edge, NodeItem* s, NodeItem* d);
    ~EdgeItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    NodeItem* getSource();
    NodeItem* getDestination();


public slots:

    void setSelected(bool selected);
    void setVisible(bool visible);

    void updateEdge();
    void graphMLDataChanged(GraphMLData * data);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event );

private:
    void resetEdgeCenter(NodeItem* visibleSource, NodeItem* visibleDestination);
    void setLabelFont();
    void updateLabel();
    void updateLines();

    void setupBrushes();
    void forceVisible(bool visible);

    QPointF getEdgeCenterPoint();

    bool deleting;
    QPainterPath* painterPath;

    QVector<QGraphicsLineItem*> lineItems;

    NodeItem* sourceParent;
    NodeItem* destinationParent;
    NodeItem* source;
    NodeItem* destination;

    NodeItem* visibleSource;
    NodeItem* visibleDestination;


    bool IS_SELECTED;
    bool IS_INSTANCE_LINK;
    bool IS_IMPL_LINK;
    bool IS_AGG_LINK;
    bool IS_VISIBLE;
    bool IS_DEPLOYMENT_LINK;

    bool IS_COMPONENT_LINK;

    bool IS_MOVING;
    QPointF previousScenePosition;
    QPen pen;
    QPen arrowPen;
    QPen selectedPen;
    QPen selectedArrowPen;

    QPen arrowHeadPen;
    QPen arrowTailPen;

    QPolygonF arrowTail;
    QPolygonF arrowHead;

    QBrush selectedBrush;
    QBrush brush;

    QBrush tailBrush;
    QBrush headBrush;

    int circleRadius;
    int width;
    int height;

    QRect bRec;

    QLineF line;
   // QLineF arrowHead;
    QGraphicsTextItem* label;

    bool inScene;
    bool instanceLink;

    bool hasMovedFromCenter;

    // GraphMLItem interface
public slots:
    void aspectsChanged(QStringList aspects);
};



#endif // NODECONNECTION_H
