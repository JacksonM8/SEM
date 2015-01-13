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
#include <QRubberBand>
#include <QGraphicsProxyWidget>
#include <QPushButton>

#include "../Model/node.h"
#include "../Model/graphmldata.h"
#include "attributetablemodel.h"
#include "graphmlitem.h"

class NodeEdge;
class NodeItem : public GraphMLItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    NodeItem(Node *node, NodeItem *parent);
    ~NodeItem();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    Node* node;
    double width;
    double height;
    bool drawDetail;
    bool drawObject;
    int depth;

    void addNodeEdge(NodeEdge* line);
    void removeNodeEdge(NodeEdge* line);

    float getChildSize();

    void setHidden(bool h);
    void resetSize();

    bool isExpanded();
    bool hasExpandButton();
    void removeExpandButton();

signals:
    void nodeItemPressed(Node* n, bool selected);

    void deleted(NodeItem*);
    void centreNode(NodeItem*);
    void centreNode(Node*);
    void exportSelected(Node*);

    void setNodeSelected(Node*, bool selected=true);

    void makeChildNode(QPointF centerPoint, Node * parentNode);
    void makeChildNode(QPointF centerPoint);

    void moveSelection(QPointF move);
    void makeChildNode(QString type, Node*);
    void updateData(QString key, QString value);


    void updateDockNodeItem();
    void updateDockNodeItem(bool selected);

    void updateEdges();
    void updateOpacity(qreal opacity);
    void updateSceneRect(NodeItem* nodeItem);

    void clearSelection();
    void centerModel();

    void addExpandButtonToParent();

public slots:
    void setOpacity(qreal opacity);
    void setSelected(bool selected);
    void toggleDetailDepth(int level);

    void graphMLDataUpdated(GraphMLData *data);
    void destructNodeItem();
    void updateChildNodeType(QString type);
    void updateViewAspects(QStringList aspects);
    void sortChildren();

    void setRubberbandMode(bool On);

    void addExpandButton();
    void expandItem(bool show);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    void updateBrushes();
    void setParent(NodeItem* parentItem);

    QStringList viewAspect;


    void updatePosition(QString x=0, QString y=0);
    void updateSize(QString w=0, QString h=0);
    QString toBuildType;
    QString name;
    QString kind;
    //QRectF bRec;


    bool USING_RUBBERBAND_SELECTION;

    QPointF origin;
    QPointF sceneOrigin;

    QColor selectedColor;
    QColor color;
    QBrush selectedBrush;
    QBrush brush;

    QPen pen;
    QPen selectedPen;

    float childSize;
    bool hasMoved;

    QVector<NodeEdge*> connections;

    bool isSelected;
    bool moveActionSent;
    QRubberBand* rubberBand;
    QGraphicsColorizeEffect *graphicsEffect;

    QGraphicsTextItem* label;
    QGraphicsPixmapItem* icon;

    bool CONTROL_DOWN;
    bool isPressed;
    QPointF previousPosition;

    QString ID;


    QString parentKind;
    double origWidth;
    double origHeight;
    double prevWidth;
    double prevHeight;
    bool hidden;
    bool expanded;

    QGraphicsProxyWidget *proxyWidget;
    QPushButton *expandButton;

    void updateSize(double w, double h);
    void setLabelFont();
    void setupIcon();

    int getNumberOfChildren();
    QStringList getChildrenKind();

    double getCurvedCornerWidth();

};

#endif // NODEITEM_H
