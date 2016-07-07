#ifndef NODEITEMNEW_H
#define NODEITEMNEW_H

#include "entityitemnew.h"
#include "../nodeviewitem.h"
#include "edgeitemnew.h"
#include <QRectF>
#include <QMarginsF>
#include <QPainter>
#include <QPointF>
class NodeItemNew: public EntityItemNew
{
    Q_OBJECT
public:
    enum KIND{DEFAULT_ITEM, MODEL_ITEM, ASPECT_ITEM, PARAMETER_ITEM, QOS_ITEM};
    NodeItemNew(NodeViewItem *viewItem, NodeItemNew* parentItem, KIND kind);
    ~NodeItemNew();

    NodeItemNew* getParentNodeItem() const;
    KIND getNodeItemKind();

    void addChildNode(NodeItemNew* nodeItem);
    void removeChildNode(int ID);

    QList<NodeItemNew*> getChildNodes();
    QList<EntityItemNew*> getChildEntities() const;

    void addChildEdge(EdgeItemNew* edgeItem);
    void removeChildEdge(int ID);
    QList<EdgeItemNew*> getChildEdges();

    void addProxyEdge(EdgeItemNew* edgeItem);
    void removeProxyEdge(int ID);
    QList<EdgeItemNew*> getProxyEdges();

    void setGridEnabled(bool enabled);
    bool isGridEnabled() const;
    void setGridVisible(bool visible);
    bool isGridVisible() const;

    void setResizeEnabled(bool enabled);
    bool isResizeEnabled();

    virtual QRectF sceneBoundingRect() const;


    //RECTS
    virtual QRectF boundingRect() const;
    virtual QRectF contractedRect() const;
    virtual QRectF expandedRect() const;
    virtual QRectF currentRect() const;
    virtual QRectF gridRect() const;
    virtual QRectF moveRect() const;

    QRectF childrenRect() const;

    QSizeF getSize() const;

    void setMinimumWidth(qreal width);
    void setMinimumHeight(qreal height);

    //Size/Position Functions
    void setExpandedWidth(qreal width);
    void setExpandedHeight(qreal height);

    qreal getExpandedWidth() const;
    qreal getExpandedHeight() const;
    qreal getMinimumWidth() const;
    qreal getMinimumHeight() const;

    void setMargin(QMarginsF margin);
    void setPadding(QMarginsF padding);

    QPointF getMarginOffset() const;
    QPointF getTopLeftSceneCoordinate() const;

    qreal getWidth() const;
    qreal getHeight() const;

    QPointF getCenter() const;
    void setCenter(QPointF center);

    virtual void setPos(const QPointF &pos);


    void setAspect(VIEW_ASPECT aspect);
    VIEW_ASPECT getAspect();


    QMarginsF getMargin() const;
    QMarginsF getPadding() const;


    bool isExpanded() const;

signals:
    void req_adjustSize(NodeViewItem* item, QSizeF delta, RECT_VERTEX vertex);
    void req_adjustSizeFinished();

    //Inform of Changes
    void gotChildNodes(bool);
    void gotChildProxyEdges(bool);

    void posChanged(QPointF topLeft);
    void sizeChanged(QSizeF newSize);

public slots:
    void setExpanded(bool);
    virtual void dataChanged(QString keyName, QVariant data);
private:
    int getGridSize() const;
    int getMajorGridCount() const;
    void updateGridLines();
    NodeViewItem* nodeViewItem;
    KIND nodeItemKind;


    qreal minimumWidth;
    qreal minimumHeight;
    qreal expandedWidth;
    qreal expandedHeight;

    QMarginsF margin;
    QMarginsF padding;

    bool _isExpanded;

    bool gridEnabled;
    bool gridVisible;

    bool resizeEnabled;

    RECT_VERTEX hoveredResizeVertex;
    RECT_VERTEX selectedResizeVertex;

    QPointF previousMovePoint;
    QPointF previousResizePoint;
    VIEW_ASPECT aspect;

    QHash<int, NodeItemNew*> childNodes;
    QHash<int, EdgeItemNew*> childEdges;
    QHash<int, EdgeItemNew*> proxyChildEdges;

    QVector<QLineF> gridLines_Minor_Horizontal;
    QVector<QLineF> gridLines_Major_Horizontal;
    QVector<QLineF> gridLines_Minor_Vertical;
    QVector<QLineF> gridLines_Major_Vertical;

    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // EntityItemNew interface
public:
    virtual QRectF getElementRect(ELEMENT_RECT rect) = 0;
    virtual QRectF getResizeRect(RECT_VERTEX vert);

    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);


    // QGraphicsItem interface
protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
};
#endif
