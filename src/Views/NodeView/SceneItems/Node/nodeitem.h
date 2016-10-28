#ifndef NODEITEM_H
#define NODEITEM_H

#include "../entityitem.h"
#include "../../../../Controllers/ViewController/nodeviewitem.h"
#include "../Edge/edgeitem2.h"

class NodeItem: public EntityItem
{
    Q_OBJECT
public:
    enum KIND{DEFAULT_ITEM, MODEL_ITEM, ASPECT_ITEM, PARAMETER_ITEM, QOS_ITEM, HARDWARE_ITEM, CONTAINER_ITEM};
    enum NODE_READ_STATE{NORMAL, READ_ONLY_INSTANCE, READ_ONLY_DEFINITION};
    NodeItem(NodeViewItem *viewItem, NodeItem* parentItem, KIND kind);
    ~NodeItem();

    KIND getNodeItemKind();
    NodeViewItem* getNodeViewItem() const;

    Node::NODE_KIND getNodeKind() const;

    NODE_READ_STATE getReadState() const;


    virtual void setRightJustified(bool isRight);
    bool isRightJustified() const;

    virtual void addChildNode(NodeItem* nodeItem);



    void removeChildNode(NodeItem *nodeItem);

    int getSortOrder() const;

    bool hasChildNodes() const;
    QList<NodeItem*> getChildNodes() const;
    QList<NodeItem*> getOrderedChildNodes() const;
    QList<EntityItem*> getChildEntities() const;

    QPainterPath getChildNodePath();

    void addChildEdge(EdgeItem2* edgeItem);
    void removeChildEdge(int ID);
    QList<EdgeItem2*> getChildEdges() const;

    QRectF getNearestGridOutline();
    QPointF getNearestGridPointToCenter();


    void setGridEnabled(bool enabled);
    bool isGridEnabled() const;
    void setGridVisible(bool visible);
    bool isGridVisible() const;

    void setResizeEnabled(bool enabled);
    bool isResizeEnabled();

    void setChildMoving(EntityItem* child, bool moving);

    void setMoveStarted();
    bool setMoveFinished();

    void setResizeStarted();
    bool setResizeFinished();






    //RECTS
    virtual QRectF viewRect() const;
    virtual QRectF boundingRect() const;
    virtual QRectF contractedRect() const;
    virtual QRectF expandedRect() const;
    virtual QRectF currentRect() const;
    QRectF gridRect() const;
    QRectF expandedGridRect() const;
    virtual QRectF bodyRect() const;
    virtual QRectF headerRect() const;


    QRectF childrenRect() const;

    QSizeF getSize() const;

    void adjustExpandedSize(QSizeF delta);

    void setMinimumWidth(qreal width);
    void setMinimumHeight(qreal height);

    //Size/Position Functions
    void setExpandedWidth(qreal width);
    void setExpandedHeight(qreal height);
    void setExpandedSize(QSizeF size);

    qreal getExpandedWidth() const;
    qreal getExpandedHeight() const;
    QSizeF getExpandedSize() const;
    qreal getMinimumWidth() const;
    qreal getMinimumHeight() const;

    void setMargin(QMarginsF margin);
    void setBodyPadding(QMarginsF bodyPadding);

    QPointF getMarginOffset() const;
    QPointF getBottomRightMarginOffset() const;
    QPointF getTopLeftSceneCoordinate() const;

    qreal getWidth() const;
    qreal getHeight() const;


    QPointF getCenterOffset() const;

    virtual QPointF getSceneEdgeTermination(bool left) const;


    //virtual void setPos(const QPointF &pos);


    void setAspect(VIEW_ASPECT aspect);
    VIEW_ASPECT getAspect();

    void setManuallyAdjusted(RECT_VERTEX aspect);


    QMarginsF getMargin() const;
    QMarginsF getBodyPadding() const;


    void setPrimaryTextKey(QString key);
    void setSecondaryTextKey(QString key);

    void setVisualEdgeKind(Edge::EDGE_KIND kind);
    Edge::EDGE_KIND getVisualEdgeKind() const;

    QString getPrimaryTextKey() const;
    QString getSecondaryTextKey() const;
    bool gotPrimaryTextKey() const;
    bool gotSecondaryTextKey() const;


    QString getPrimaryText() const;
    QString getSecondaryText() const;

    virtual void setExpanded(bool expand);


    QSizeF getGridAlignedSize(QSizeF size=QSizeF()) const;
    int getVertexAngle(RECT_VERTEX vert) const;
signals:
    //Request changes
    void req_connectMode(NodeItem* item);

    void req_StartResize();
    void req_Resize(NodeItem* item, QSizeF delta, RECT_VERTEX vert);
    void req_FinishResize();

    //Inform of Changes
    void gotChildNodes(bool);
    void gotChildProxyEdges(bool);
public slots:
    virtual void dataChanged(QString keyName, QVariant data);
    virtual void propertyChanged(QString propertyName, QVariant data);
    virtual void dataRemoved(QString keyName);
    virtual void childPosChanged();
private:
    void edgeAdded(Edge::EDGE_KIND kind);
    void edgeRemoved(Edge::EDGE_KIND kind);


    void updateReadState();
    void setUpColors();

    void resizeToChildren();
    int getResizeArrowRotation(RECT_VERTEX vert) const;


    void updateGridLines();
    NodeViewItem* nodeViewItem;
    KIND nodeItemKind;

    Edge::EDGE_KIND visualEdgeKind;
    QString visualEdgeIcon;

    NODE_READ_STATE readState;

    qreal minimumWidth;
    qreal minimumHeight;
    qreal expandedWidth;
    qreal expandedHeight;

    QMarginsF margin;
    QMarginsF bodyPadding;


    QRectF _childRect;
    bool gridEnabled;
    bool gridVisible;
    bool ignorePosition;


    bool _isResizing;
    QSizeF sizePreResize;


    bool _rightJustified;

    bool resizeEnabled;


    bool hoveredConnect;
    RECT_VERTEX hoveredResizeVertex;
    RECT_VERTEX selectedResizeVertex;

    QPointF previousMovePoint;
    QPointF previousResizePoint;
    VIEW_ASPECT aspect;

    QHash<int, NodeItem*> childNodes;
    QHash<int, EdgeItem2*> childEdges;

    QPainterPath gridLines;

    QColor readOnlyInstanceColor;
    QColor readOnlyDefinitionColor;
    QColor resizeColor;

    qreal modelWidth;
    qreal modelHeight;
    QPen gridLinePen;

    QString primaryTextKey;
    QString secondaryTextKey;

    QPair<QString, QString> tertiaryIcon;
    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // EntityItemNew interface
public:
    virtual QRectF getElementRect(ELEMENT_RECT rect) const;
    virtual QPainterPath getElementPath(ELEMENT_RECT rect) const;
    virtual QRectF getResizeRect(RECT_VERTEX vert) const;
    virtual QRectF getResizeArrowRect() const;


    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);



    // EntityItemNew interface
public:
    QColor getBodyColor() const;
    QColor getHeaderColor() const;

    // EntityItemNew interface
public:
    QPointF getTopLeftOffset() const;
};
#endif //NODEITEM_H
