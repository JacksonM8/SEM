#ifndef NODEITEM_H
#define NODEITEM_H

#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsColorizeEffect>
#include <QGraphicsSceneMouseEvent>
#include <QObject>
#include <QPainter>
#include <QTouchEvent>
#include <iostream>
#include <QGraphicsProxyWidget>
#include <QPushButton>

#include "../../Model/node.h"
#include "../../Model/graphmldata.h"
#include "../table/attributetablemodel.h"
#include "graphmlitem.h"
#include "editabletextitem.h"

class EdgeItem;

class NodeItem : public GraphMLItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum RESIZE_TYPE{NO_RESIZE, RESIZE, HORIZONTAL_RESIZE, VERTICAL_RESIZE};
    NodeItem(Node *node, NodeItem *parent, QStringList aspects, bool IN_SUBVIEW=false);
    ~NodeItem();

    NodeItem* getParentNodeItem();

    QList<EdgeItem*> getEdgeItems();
    void setParentItem(QGraphicsItem* parent);
    QRectF boundingRect() const;
    QRectF minimumVisibleRect();


    QPointF getClosestGridPoint(QPointF currentPosition);

    QRectF gridRect();


    QRectF getMinimumChildRect();

    void childPosUpdated();

    QPointF getGridPosition(int x, int y);
    bool isSelected();
    bool isLocked();
    void setLocked(bool locked);
    bool isPainted();
    bool isAncestorSelected();
    void setDrawGrid(bool value);

    void addChildNodeItem(NodeItem* child);
    void removeChildNodeItem(NodeItem* child);
    bool intersectsRectangle(QRectF rect);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    bool hasVisibleChildren();

    bool labelPressed(QPointF mousePosition);
    bool iconPressed(QPointF mousePosition);


    NodeItem::RESIZE_TYPE resizeEntered(QPointF mousePosition);


    bool isExpanded();
    bool isHidden();
    void setHidden(bool hidden);


    void addEdgeItem(EdgeItem* line);
    void removeEdgeItem(EdgeItem* line);

    void setCenterPos(QPointF pos);
    QPointF centerPos();
    void adjustPos(QPointF delta);
    void adjustSize(QSizeF delta);

    void drawOutline(QRectF translateOutline);
    void clearOutlines();



    double getWidth();
    double getHeight();
    void resetSize();
    double getChildWidth();

    QPointF getNextChildPos();
    void itterateNextSpace();

    Node* getNode();
    QString getNodeKind();
    QList<NodeItem*> getChildNodeItems();
    bool isPermanentlyCentered();

    qreal getGridSize();

signals:
    void NodeItem_SortModel();
    void NodeItem_MoveSelection(QPointF delta);
    void NodeItem_ResizeSelection(QSizeF delta);
    void NodeItem_MoveFinished();
    void NodeItem_ResizeFinished();

    //Node Edge Signals
    void setEdgeVisibility(bool visible);
    void setEdgeSelected(bool selected);

    void centerViewAspects();

    //DockNodeItem Signals
    void updateDockNodeItem();
    void updateOpacity(qreal opacity);



    void recentralizeAfterChange(GraphML* item);

    void nodeItemMoved();
    void itemMovedOutOfScene(NodeItem* item);

    void Nodeitem_HasFocus(bool hasFocus);


public slots:
    QPolygonF getResizePolygon();
    //QRectF
    void parentNodeItemMoved();
    //Model Signals
    void graphMLDataChanged(GraphMLData *data);
    void setOpacity(qreal opacity);
    void setSelected(bool selected);
    void setVisible(bool visible);

    void setPermanentlyCentralized(bool centralized);

    void aspectsChanged(QStringList aspects);

    void newSort();


    void expandItem(bool show);

    void updateHeight(NodeItem* child);

    void updateModelPosition();
    void updateModelSize();

    void sceneRectChanged(QRectF sceneRect);
    void labelUpdated(QString newLabel);
    void setNewLabel(QString label = "");

    //Turn off visible gridlines;
    void toggleGridLines(bool on);

    void snapToGrid();
    void snapChildrenToGrid();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);



private:
    void setWidth(qreal width);
    void setHeight(qreal height);
    void setSize(qreal w, qreal h);
    NodeItem* getChildNodeItemFromNode(Node* child);


    void updateGridLines();

    void setPermanentlyInvisible(bool isInvisible);
    void setupAspect();
    void setupBrushes();
    void setupIcon();
    void setupLabel();
    void setupGraphMLConnections();

    QPointF isOverGrid(const QPointF position);



    void setPaintObject(bool paint);

    void updateGraphMLSize();
    void updateGraphMLPosition();

    void updateChildrenOnChange();
    void retrieveGraphMLData();

    void updateTextLabel(QString text=0);

    bool expanded;
    bool permanentlyCentralized;


    QStringList getChildrenKind();

    double getCornerRadius();
    double getChildCornerRadius();
    double getMaxLabelWidth();

    QColor invertColor(QColor oldColor);

    double getItemMargin() const;



    NodeItem* parentNodeItem;
    QStringList viewAspects;

    void setPos(qreal x, qreal y);
    void setPos(const QPointF &pos);

    QGraphicsPixmapItem* icon;
    QGraphicsPixmapItem* lockIcon;
    QGraphicsProxyWidget *proxyWidget;
    QGraphicsProxyWidget *labelWidget;
    QPushButton* labelButton;

    QList<EdgeItem*> connections;

    void updateParent();
    bool nodeSelected;
    bool nodeResizing;
    bool isNodePressed;
    bool drawGrid;

    int X_GRID_POS;
    int Y_GRID_POS;

    QString nodeKind;
    QString fileID;

    NodeItem::RESIZE_TYPE currentResizeMode;

    QList<QRectF> outlines;
    //Current Width/Height
    double width;
    double height;

    //double initialWidth;
    //double initialHeight;

    double minimumHeight;
    double minimumWidth;

    double expandedHeight;
    double expandedWidth;
    //double prevWidth;
    //double prevHeight;

    QPointF nextChildPosition;

    QPointF previousScenePosition;
    //QPointF initialScenePressPosition;
    bool hasSelectionMoved;
    bool hasSelectionResized;


    QVector<QLineF> xGridLines;
    QVector<QLineF> yGridLines;
    bool GRIDLINES_VISIBLE;

    QList<NodeItem*> childNodeItems;

    //Used to store the Color/Brush/Pen for the selected Style.
    QColor selectedColor;
    QColor color;
    QBrush selectedBrush;
    QBrush brush;
    QPen pen;
    QPen selectedPen;

    int nextX;
    int nextY;

    bool hasDefinition;
    bool onGrid;

    bool LOCKED_POSITION;

    bool PAINT_OBJECT;

    bool hidden;
    int depth;

    bool permanentlyInvisible;

    bool firstReposition;

    QRectF currentSceneRect;

    QPixmap pixmap;
    QString stringLabel;

    EditableTextItem* textItem;

    // GraphMLItem interface

};

#endif // NODEITEM_H
