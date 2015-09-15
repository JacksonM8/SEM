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
#include <QMenu>
#include <QWidgetAction>
#include <QCheckBox>
#include <QRadioButton>

#include "../../Model/node.h"
#include "../../Model/graphmldata.h"
#include "../Table/attributetablemodel.h"
#include "graphmlitem.h"
#include "editabletextitem.h"

class EdgeItem;
class NodeView;

class NodeItem : public GraphMLItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum RESIZE_TYPE{NO_RESIZE, RESIZE, HORIZONTAL_RESIZE, VERTICAL_RESIZE};
    NodeItem(Node *node, NodeItem *parent, QStringList aspects, bool IN_SUBVIEW=false);
    ~NodeItem();



    //Used Methods
    void setZValue(qreal z);
    void restoreZValue();




    void setVisibleParentForEdgeItem(QString ID, bool RIGHT = false);
    int getIndexOfEdgeItem(QString ID, bool RIGHT = false);
    int getNumberOfEdgeItems(bool RIGHT = false);
    void removeVisibleParentForEdgeItem(QString ID);

    void setGridVisible(bool visible);


    NodeItem* getParentNodeItem();

    QList<EdgeItem*> getEdgeItems();
    void setParentItem(QGraphicsItem* parent);
    QRectF boundingRect() const;
    QRectF minimumVisibleRect();
    QRectF expandedVisibleRect();
    QRectF currentItemRect();

    int getEdgeItemIndex(EdgeItem* item = 0);
    int getEdgeItemCount();

    QPointF getClosestGridPoint(QPointF childCenterPoint);

    QRectF gridRect();

    QRectF getChildBoundingRect();
    QRectF getMinimumChildRect();

    void childPosUpdated();

    QPointF getGridPosition(int x, int y);


    bool isSelected();
    bool isLocked();
    void setLocked(bool locked);
    bool isPainted();
    bool isAncestorSelected();


    void addChildNodeItem(NodeItem* child);
    void removeChildNodeItem(QString ID);
    bool intersectsRectangle(QRectF rect);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    bool hasVisibleChildren();\


    bool modelCirclePressed(QPointF mousePosition);
    bool labelPressed(QPointF mousePosition);
    bool deploymentIconPressed(QPointF mousePosition);
    bool lockIconPressed(QPointF mousePosition);
    bool labelEditable();
    bool iconPressed(QPointF mousePosition);

    bool menuArrowPressed(QPointF mousePosition);


    NodeItem::RESIZE_TYPE resizeEntered(QPointF mousePosition);



    bool isExpanded();
    bool isContracted();
    bool isHidden();
    void setHidden(bool hidden);


    void addEdgeItem(EdgeItem* line);
    void removeEdgeItem(EdgeItem* line);

    void setCenterPos(QPointF pos);
    QPointF centerPos();
    void adjustPos(QPointF delta);
    void adjustSize(QSizeF delta);

    void addChildOutline(NodeItem* nodeItem, QPointF gridPoint);
    void removeChildOutline(QString ID);


    double getWidth();
    double getHeight();
    void resetSize();
    double getChildWidth();
    double getChildHeight();

    QPointF getNextChildPos(QRectF itemRect = QRectF(), bool currentlySorting=false);

    Node* getNode();
    QString getNodeKind();
    QString getNodeLabel();
    QList<NodeItem*> getChildNodeItems();

    qreal getGridSize();

    QStringList getAspects();



    bool isInAspect();
    bool isSorted();
    void setSorted(bool isSorted);

    QMenu* getChildrenViewOptionMenu();
    QRectF geChildrenViewOptionMenuSceneRect();

    void highlightNodeItem(bool highlight);
    void showHardwareIcon(bool show);
    QList<NodeItem *> deploymentView(bool on, NodeItem* selectedItem = 0);

    int getChildrenViewMode();


signals:
    //Node Edge Signals
    void setEdgeVisibility(bool visible);
    void setEdgeSelected(bool selected);


    void model_PositionChanged();
    void NodeItem_SortModel();
    void NodeItem_MoveSelection(QPointF delta);
    void NodeItem_ResizeSelection(QString ID, QSizeF delta);
    void NodeItem_MoveFinished();
    void NodeItem_ResizeFinished(QString ID);

 	void Nodeitem_HasFocus(bool hasFocus);

    void NodeItem_showLockMenu(NodeItem* nodeItem);
    void NodeItem_lockMenuClosed(NodeItem* nodeItem);




    //void centerViewAspects();

    //DockNodeItem Signals
    void updateDockNodeItem();
    void updateOpacity(qreal opacity);




    void nodeItemMoved();

    void nodeItem_menuClicked(int viewMode);


public slots:
    //USED METHODS
    void graphMLDataChanged(GraphMLData *data);

    void setSelected(bool selected);
    void setVisibility(bool visible);


    void parentNodeItemMoved();
    //Model Signals




    void aspectsChanged(QStringList aspects);

    void newSort();
    void modelSort();

    //void expandNode(bool expand);
    void setNodeExpanded(bool expanded);

    void updateModelPosition();
    void updateModelSize();

    void sceneRectChanged(QRectF sceneRect);
    void labelUpdated(QString newLabel);
    void setNewLabel(QString label = "");

    //Turn off visible gridlines;
    void toggleGridLines(bool on);

    void snapToGrid();
    void snapChildrenToGrid();

 	void menuClosed();
    void updateChildrenViewMode(int viewMode = -1);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);



private:
    void updateDisplayedChildren(int viewMode);

//USED METHODS
    QRectF iconRect();
    QRectF lockIconRect();
    QRectF deploymentIconRect();
    QPolygonF resizePolygon();


    void sendSelectSignal(bool setSelected, bool controlDown);
    //USED PARAMETERS;
    bool hasIcon;
    bool showDeploymentWarningIcon;

    bool compareTo2Decimals(qreal num1, qreal num2);

    bool isInResizeMode();
    bool isMoveable();
    bool isResizeable();
    void updateModelData();


    void resizeToOptimumSize(bool updateParent=true);
    void setWidth(qreal width, bool updateParent=true);
    void setHeight(qreal height, bool updateParent=true);
    void setSize(qreal w, qreal h);
    void setPos(qreal x, qreal y);
    void setPos(const QPointF &pos);
    void calculateGridlines();

    void setupAspect();
    void setupBrushes();
    void setupIcon();
    void setupLabel();
    void setupGraphMLConnections();
    void setupChildrenViewOptionMenu();

    void setPaintObject(bool paint);
    void updateGraphMLPosition();
    void updateChildrenOnChange();
    void retrieveGraphMLData();
    void updateTextLabel(QString text="");
    void childUpdated();



    QStringList getChildrenKind();
    NodeItem* getChildNodeItemFromNode(Node* child);
    QPointF isOverGrid(const QPointF centerPosition);

    //QPixmap iconPixmap;
    //QImage iconImage;


    double getCornerRadius();
    double getChildCornerRadius();
    double getMaxLabelWidth();
    double getItemMargin() const;
    double getChildItemMargin();// const;

    double ignoreInsignificantFigures(double model, double current);

    QSizeF getModelSize();


    bool drawGridlines();


    QMenu* childrenViewOptionMenu;
    QRadioButton* allChildren;
    QRadioButton* connectedChildren;
    QRadioButton* unConnectedChildren;

    bool HARDWARE_CLUSTER;
    int CHILDREN_VIEW_MODE;
    bool sortTriggerAction;
    bool eventFromMenu;

    NodeItem* parentNodeItem;
    QStringList viewAspects;


    QString nodeKind;
    QString nodeLabel;
    QString minimumHeightStr;
    QString fileID;

    bool isNodeExpanded;

    bool isNodeSelected;
    bool isGridVisible;
    bool isInSubView;
    bool isNodeSorted;
    bool isNodeInAspect;

    bool isNodeOnGrid;
    bool nodeWasOnGrid;




    bool GRIDLINES_ON;

    bool highlighted;
    bool hidden;
    bool hasDefinition;
    bool isImplOrInstance;

    bool LOCKED_POSITION;
    bool PAINT_OBJECT;

    bool isNodeMoving;
    qreal oldZValue;
    NodeItem::RESIZE_TYPE currentResizeMode;

    QHash<QString, QRectF> outlineMap;

    //Current Width/Height
    double width;
    double height;

    double minimumHeight;
    double minimumWidth;

    double expandedHeight;
    double expandedWidth;

    QPointF previousScenePosition;

    //USED TO DETERMINE THE NUMBER OF EDGES.
    QList<EdgeItem*> connections;

    QStringList currentLeftEdgeIDs;
    QStringList currentRightEdgeIDs;


    QVector<QLineF> xGridLines;
    QVector<QLineF> yGridLines;


    QHash<QString, NodeItem*> childNodeItems;

    QStringList childrenIDs;
    //QList<NodeItem*> childNodeItems;

    //Used to store the Color/Brush/Pen for the selected Style.
    QColor selectedColor;
    QColor color;

    QBrush selectedBrush;
    QBrush brush;
    QPen pen;
    QPen selectedPen;
    bool updatedAlready;


    QPointF modelCenterPoint;

    bool hasPanned;




    QRectF currentSceneRect;


    EditableTextItem* textItem;

    QString nodeHardwareOS;
    QString nodeHardwareArch;
    bool nodeHardwareLocalHost;
};

#endif // NODEITEM_H
