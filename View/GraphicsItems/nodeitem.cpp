#include "nodeitem.h"
#include "edgeitem.h"
#include "../nodeview.h"

#include <QGraphicsTextItem>
#include <QDebug>
#include <QFont>
#include <QStyleOptionGraphicsItem>
#include <QPixmap>
#include <QBitmap>

#include <QInputDialog>
#include <QTextBlockFormat>

#include <QTextCursor>
#include <cmath>

#define MODEL_WIDTH 1920
#define MODEL_HEIGHT 1080


//#define MARGIN_RATIO 0.15 //LARGE
#define MARGIN_RATIO 0.1 //NORMAL
//#define MARGIN_RATIO 0.05 //COMPACT

//#define ICON_RATIO 0.7 //LARGE
#define ICON_RATIO 0.8 //NORMAL
//#define ICON_RATIO 0.9 //COMPACT


//RATIO FROM ASPECT TO FIRST ENTITY.
#define ENTITY_SIZE_RATIO 10
//RATIO FROM MODEL TO ASPECTS
#define ASPECT_SIZE_RATIO 3

#define LABEL_RATIO (1 - ICON_RATIO)

#define SNAP_PERCENTAGE .25

#define GRID_RATIO 1.75

#define ALL 0
#define CONNECTED 1
#define UNCONNECTED 2

/**
 * @brief NodeItem::NodeItem
 * @param node
 * @param parent
 * @param aspects
 * @param IN_SUBVIEW
 */
NodeItem::NodeItem(Node *node, NodeItem *parent, QStringList aspects, bool IN_SUBVIEW):  GraphMLItem(node, GraphMLItem::NODE_ITEM)
{
    Q_INIT_RESOURCE(resources);
    setParentItem(parent);
    

    parentNodeItem = parent;
    showDeploymentWarningIcon = false;
    isNodeSelected = false;
    nodeWasOnGrid = false;

    
    isNodeSorted = false;
    nodeLabel = "";
    
    isInSubView = IN_SUBVIEW;
    
    
    currentResizeMode = NodeItem::NO_RESIZE;
    LOCKED_POSITION = false;
    GRIDLINES_ON = false;
    isGridVisible = false;
    isNodeOnGrid = false;

    isNodeExpanded = true;
    hidden = false;

    
    hasDefinition = false;
    isImplOrInstance = false;
    if(node){
        isImplOrInstance = node->isInstance() || node->isImpl();
    }
    highlighted = false;
    
    
    textItem = 0;
    width = 0;
    height = 0;
    expandedWidth = 0;
    expandedHeight = 0;
    minimumWidth = 0;
    minimumHeight = 0;

    nodeKind = getGraphML()->getDataValue("kind");

    HARDWARE_CLUSTER = (nodeKind == "HardwareCluster");
    CHILDREN_VIEW_MODE = CONNECTED;
    sortTriggerAction = true;
    eventFromMenu = true;
    
    hasIcon = true;
    if(nodeKind.endsWith("Definitions") || nodeKind == "Model"){
        hasIcon = false;
    }
    
    QString parentNodeKind = "";
    if (parent) {
        setVisibility(parent->isExpanded());
        
        setWidth(parent->getChildWidth());
        setHeight(parent->getChildHeight());
        
        parentNodeKind = parent->getGraphMLDataValue("kind");
        
    } else {
        setWidth(MODEL_WIDTH);
        setHeight(MODEL_HEIGHT);
    }
    //Set Minimum Size.
    if(nodeKind == "Model"){
        minimumWidth = getChildWidth();
        minimumHeight = getChildHeight();
    }else{
        minimumWidth = width;
        minimumHeight = height;
    }
    minimumHeightStr = QString::number(minimumHeight);
    
    //Set Maximum Size
    expandedWidth = width;
    expandedHeight = height;
    
    
    setupLabel();
    //Update Width and Height with values from the GraphML Model If they have them.
    retrieveGraphMLData();


    setupGraphMLConnections();
    
    setupAspect();
    setupBrushes();
    //setupIcon();

    setFlag(ItemDoesntPropagateOpacityToChildren);
    setFlag(ItemIgnoresParentOpacity);
    setFlag(ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    //QGraphicsItem::setAcceptsHoverEvents
    
    //setCacheMode(QGraphicsItem::NoCache);
    
    if(nodeKind == "Model" || nodeKind == "DeploymentDefinitions"){
        setPaintObject(false);
    }else{
        setPaintObject(true);
    }
    
    if (HARDWARE_CLUSTER) {
        setupChildrenViewOptionMenu();
    }
    

    if(IN_SUBVIEW){
        setVisibility(true);
    }else{
        updateModelData();
    }


    if(getParentNodeItem()){
        getParentNodeItem()->childPosUpdated();
        getParentNodeItem()->updateModelSize();
    }
    

    aspectsChanged(aspects);
}


/**
 * @brief NodeItem::~NodeItem
 */
NodeItem::~NodeItem()
{
    currentLeftEdgeIDs.clear();
    currentRightEdgeIDs.clear();
    if(getNodeView()){
        if(!getNodeView()->isTerminating()){
            if(parentNodeItem){
                //Only unstitch if we aren't terminating
                parentNodeItem->removeChildNodeItem(getID());
            }
        }
    }
    
    delete textItem;
}

/**
 * @brief NodeItem::setZValue Overides the QGraphicsItem::setZValue function to recurse up it's parentNodeItem and set the Z-Value on its parents.
 * @param z The ZValue to set.
 */
void NodeItem::setZValue(qreal z)
{
    oldZValue = zValue();
    if(parentNodeItem){
        parentNodeItem->setZValue(z);
    }
    QGraphicsItem::setZValue(z);
}

/**
 * @brief NodeItem::restoreZValue Resets the Z-Value back to it's original Z-Value (Calls SetZValue())
 */
void NodeItem::restoreZValue()
{
    qreal zValue = oldZValue;
    setZValue(zValue);
    oldZValue = zValue;
}


/**
 * @brief NodeItem::setVisibleParentForEdgeItem
 * @param line
 * @param RIGH
 * @return The index of the
 */
void NodeItem::setVisibleParentForEdgeItem(QString ID, bool RIGHT)
{
    if(RIGHT){
        if(!currentRightEdgeIDs.contains(ID)){
            currentRightEdgeIDs.append(ID);
            if(currentRightEdgeIDs.size() > 1){
                nodeItemMoved();
            }
        }
    }else{
        if(!currentLeftEdgeIDs.contains(ID)){
            currentLeftEdgeIDs.append(ID);
            if(currentLeftEdgeIDs.size() > 1){
                nodeItemMoved();
            }
        }
        
    }
}

int NodeItem::getIndexOfEdgeItem(QString ID, bool RIGHT)
{
    int id = -1;
    if(RIGHT){
        if(currentRightEdgeIDs.contains(ID)){
            id =  currentRightEdgeIDs.indexOf(ID);
        }
    }else{
        if(currentLeftEdgeIDs.contains(ID)){
            id = currentLeftEdgeIDs.indexOf(ID);
        }
    }
    return id;
}

int NodeItem::getNumberOfEdgeItems(bool RIGHT)
{
    if(RIGHT){
        return currentRightEdgeIDs.count();
    }else{
        return currentLeftEdgeIDs.count();
    }
}

void NodeItem::removeVisibleParentForEdgeItem(QString ID)
{
    if(!isDeleting()){
        currentRightEdgeIDs.removeOne(ID);
        currentLeftEdgeIDs.removeOne(ID);
    }
}


NodeItem *NodeItem::getParentNodeItem()
{
    return dynamic_cast<NodeItem*>(parentItem());
}


QList<EdgeItem *> NodeItem::getEdgeItems()
{
    return this->connections;
}


void NodeItem::setParentItem(QGraphicsItem *parent)
{
    NodeItem* nodeItem = dynamic_cast<NodeItem*>(parent);
    if(nodeItem){
        nodeItem->addChildNodeItem(this);
        connect(nodeItem, SIGNAL(nodeItemMoved()), this, SLOT(parentNodeItemMoved()));
    }
    QGraphicsItem::setParentItem(parent);
}


QRectF NodeItem::boundingRect() const
{
    qreal topLeftX = 0;
    qreal topLeftY = 0;
    qreal bottomRightX = width;
    qreal bottomRightY = height;
    
    float itemMargin = getItemMargin();
    bottomRightX += itemMargin;
    bottomRightY += itemMargin;
    bottomRightX += itemMargin;
    bottomRightY += itemMargin;
    
    return QRectF(QPointF(topLeftX, topLeftY), QPointF(bottomRightX, bottomRightY));
}


QRectF NodeItem::minimumVisibleRect()
{
    
    qreal totalMargin = getItemMargin() * 2;
    return QRectF(QPointF(0, 0), QPointF(minimumWidth + totalMargin, minimumHeight + totalMargin));
}

QRectF NodeItem::expandedVisibleRect()
{
    return QRectF(QPointF(getItemMargin(), getItemMargin()), QPointF(expandedWidth + getItemMargin(), expandedHeight + getItemMargin()));
}

QRectF NodeItem::currentItemRect()
{
    return QRectF(QPointF(getItemMargin(), getItemMargin()), QPointF(width + getItemMargin(), height + getItemMargin()));
}

int NodeItem::getEdgeItemIndex(EdgeItem *item)
{
    return connections.indexOf(item);
    
}

int NodeItem::getEdgeItemCount()
{
    return connections.size();
    
    
}

/**
 * @brief NodeItem::gridRect Returns a QRectF which contains the local coordinates of where the Grid lines are to be drawn.
 * @return The grid rectangle
 */
QRectF NodeItem::gridRect()
{
    QPointF topLeft = getMinimumChildRect().topLeft();
    QPointF bottomRight = expandedVisibleRect().bottomRight();
    
    //If it has an icon,
    if(hasIcon){
        topLeft += QPointF(0, minimumHeight);
    }
    //Enforce at least one grid in height. Size!
    qreal deltaY = bottomRight.y() - topLeft.y();
    if(deltaY <= 0){
        deltaY = qMax(deltaY, getGridSize());
        bottomRight.setY(bottomRight.y() + deltaY);
    }
    
    return QRectF(topLeft, bottomRight);
}

QRectF NodeItem::getChildBoundingRect()
{
    return QRectF(QPointF(0, 0), QPointF(getChildWidth() + (2* getChildItemMargin()), getChildHeight() + (2 * getChildItemMargin())));
}

void NodeItem::childPosUpdated()
{
    if(!getGraphML()){
        return;
    }
    
    QSizeF minSize = getMinimumChildRect().size();
    
    bool okay = false;
    double modelWidth = getGraphMLDataValue("width").toDouble(&okay);
    if(!okay){
        return;
    }
    double modelHeight = getGraphMLDataValue("height").toDouble(&okay);
    if(!okay){
        return;
    }
    

    
    if(nodeKind == "Model"){
        //Sort after any model changes!
        newSort();
    }

    //Maximize on the current size in the Model and the minimum child rectangle
    if(minSize.width() > modelWidth){
        setWidth(minSize.width());
    }

    if(minSize.height() > modelHeight){
        setHeight(minSize.height());
    }
}


QPointF NodeItem::getGridPosition(int x, int y)
{
    QPointF topLeft = gridRect().topLeft();
    topLeft.setX(topLeft.x() + (x * getGridSize()));
    topLeft.setY(topLeft.y() + (y * getGridSize()));
    return topLeft;
}


bool NodeItem::isSelected()
{
    return isNodeSelected;
}


bool NodeItem::isLocked()
{
    return LOCKED_POSITION;
}

void NodeItem::setLocked(bool locked)
{
    LOCKED_POSITION = locked;
}


bool NodeItem::isPainted()
{
    return PAINT_OBJECT;
}


void NodeItem::addChildNodeItem(NodeItem *child)
{
    if(!childNodeItems.contains(child->getID())){
        childNodeItems.insert(child->getID(), child);
        childrenIDs.append(child->getID());
    }
}


void NodeItem::removeChildNodeItem(QString ID)
{
    childrenIDs.removeAll(ID);
    childNodeItems.remove(ID);
    removeChildOutline(ID);
    if(childNodeItems.size() == 0){
        prepareGeometryChange();
        resetSize();
    }
}


bool NodeItem::intersectsRectangle(QRectF sceneRect)
{
    /*
    QRectF itemRectangle = boundingRect();
    itemRectangle.moveTo(scenePos());
    return sceneRect.contains(itemRectangle);
    */
    return sceneRect.contains(sceneBoundingRect());
}


void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    
    painter->setClipRect(option->exposedRect);
    
    
    
    
    if(PAINT_OBJECT){// == PAINT_OBJECT){
        QPen Pen;
        QBrush Brush;
        
        if(isNodeSelected){
            //Check current View zoom
            Brush = selectedBrush;
            Pen = selectedPen;
            if(getNodeView()){
                qreal scaleX = getNodeView()->transform().m11();
                qreal penWidth = 5.0/scaleX;
                penWidth = qMax(penWidth, 1.0);
                Pen.setWidth(penWidth);
            }
            
        }else{
            Brush = brush;
            Pen = pen;
        }
        
        
        QRectF rectangle = boundingRect();
        rectangle.setWidth(rectangle.width() - Pen.width()*2);
        rectangle.setHeight(rectangle.height() - Pen.width()*2);
        rectangle.translate(Pen.width(), Pen.width());
        
        
        
        //If the Node is over a Gridline, set the background Brush to transluscent.

        NodeView::VIEW_STATE viewState = getNodeView()->getViewState();
        if(isSelected() && isNodeOnGrid && (viewState == NodeView::VS_MOVING || viewState == NodeView::VS_RESIZING)){
            QColor brushColor = Brush.color();
            if(brushColor.alpha() > 120){
                brushColor.setAlpha(120);
            }
            Brush.setColor(brushColor);
        }
        
        if(!hasVisibleChildren() && !nodeKind.endsWith("Definitions")){
            Brush.setColor(Qt::transparent);
        }
        

        if(getNodeView() && isImplOrInstance){
            hasDefinition = getNodeView()->getDefinitionID(getID()) != "";
            if(!hasDefinition){
                Brush.setStyle(Qt::BDiagPattern);
            }
        }
        
        
        // highlighted border
        if (highlighted && getNodeView()){
            qreal penWidth = qMax(5.0/ getNodeView()->transform().m11(), 1.0);
            Pen.setWidth(penWidth);
            Pen.setStyle(Qt::DashLine);
            Pen.setColor(Qt::red);
        }
        
        
        painter->setPen(Pen);
        painter->setBrush(Brush);
        
        painter->drawRoundedRect(rectangle, getCornerRadius(), getCornerRadius());
        
        
        if(hasVisibleChildren() && textItem){
            QBrush brush;
            brush.setColor(QColor(100,100,100));
            brush.setStyle(Qt::SolidPattern);
            painter->setBrush(brush);
            
            painter->setPen(pen);
            
            QPen altPen = pen;
            altPen.setColor(brush.color());
            
            if(isExpanded()){
                //FROM THE BOTTM
                qreal yBot = gridRect().bottom();
                //FROM THE TOP
                //qreal yBot = gridRect().top();
                qreal yTop = yBot - getItemMargin();
                qreal xRight = boundingRect().right() - getItemMargin();
                qreal xLeft = xRight - getItemMargin();
                
                QLineF line = QLineF(gridRect().left(), gridRect().top(), xRight, gridRect().top());
                QPolygonF triangle;
                triangle.append(QPointF(xLeft,yTop));
                triangle.append(QPointF(xRight,yTop));
                triangle.append(QPointF(xLeft,yBot));
                painter->drawLine(line);
                //painter->setPen(pen);
                painter->setPen(altPen);
                painter->drawPolygon(triangle,Qt::WindingFill);
            }else{
                qreal yBot = gridRect().top();
                qreal yTop = yBot - getItemMargin();
                qreal xRight = getMinimumChildRect().right();
                qreal xLeft = xRight - getItemMargin();
                

                QPolygonF triangle;
                triangle.append(QPointF(xRight,yTop));
                triangle.append(QPointF(xRight,yBot));
                triangle.append(QPointF(xLeft,yBot));
                painter->setPen(pen);
                //painter->drawLine(line);
                painter->setPen(altPen);
                painter->drawPolygon(triangle,Qt::WindingFill);
            }
        }
        
        //New Code
        if(drawGridlines()){
            painter->setClipping(false);
            painter->setPen(pen);
            QPen linePen = painter->pen();
            
            linePen.setStyle(Qt::DashLine);
            linePen.setWidth(minimumWidth / 1000);
            painter->setPen(linePen);
            
            painter->drawLines(xGridLines);
            painter->drawLines(yGridLines);
            
            
        }
        
        
        
        //Draw the Outlines
        if(this->isExpanded() && GRIDLINES_ON){
            foreach(QRectF rect, outlineMap){
                QRectF newRectangle = rect;
                painter->setBrush(Qt::NoBrush);
                
                QPen linePen = pen;
                linePen.setColor(Qt::white);
                linePen.setStyle(Qt::DotLine);
                linePen.setWidth(linePen.width() *2);
                
                painter->setPen(linePen);
                double radius = getChildCornerRadius();
                painter->drawRoundedRect(newRectangle, radius, radius);
            }
        }
    }
    
    //painter->setBrush(Qt::NoBrush);
    //painter->drawRect(getMinimumChildRect());
    
    //painter->drawRect(gridRect());
    
    
    if(nodeKind == "Model" && !modelCenterPoint.isNull()){
        

        
        int offSet = 0; //100;
        double radius = getItemMargin() + offSet;
        QRect clippingCircle(QPoint(modelCenterPoint.toPoint() - QPoint(radius,radius)), QPoint(modelCenterPoint.toPoint() + QPoint(radius,radius)));
        
        QBrush circleBrush;
        painter->setClipping(true);
        QRegion clipCircle(clippingCircle, QRegion::Ellipse);
        painter->setClipRegion(clipCircle);
        
        circleBrush.setColor(Qt::white);
        circleBrush.setStyle(Qt::SolidPattern);
        painter->setBrush(circleBrush);
        
        
        double middleButton = radius / 2;

        
        QRectF tLR(modelCenterPoint - QPointF(radius,radius), modelCenterPoint);
        QRectF tRR(modelCenterPoint - QPointF(0,radius), modelCenterPoint + QPointF(radius,0));
        QRectF bLR(modelCenterPoint - QPointF(radius,0), modelCenterPoint + QPointF(0,radius));
        QRectF bRR(modelCenterPoint,  modelCenterPoint + QPointF(radius,radius));
        
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(110,210,210));
        painter->drawRect(tLR);
        painter->setBrush(QColor(254,184,126));
        painter->drawRect(tRR);
        painter->setBrush(QColor(255,160,160));
        painter->drawRect(bLR);
        painter->setBrush(QColor(110,170,220));
        painter->drawRect(bRR);

        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(QColor(170,170,170), 5));
        painter->drawEllipse(modelCenterPoint, radius, radius);
        painter->setPen(Qt::NoPen);

        //circleBrush.setColor(Qt::gray);
        circleBrush.setColor(QColor(150,150,150));
        
        painter->setBrush(circleBrush);

        
        if(isNodeSelected){
            QPen pen = selectedPen;
            if(getNodeView()){
                qreal scaleX = getNodeView()->transform().m11();
                qreal penWidth = 5.0/scaleX;
                penWidth = qMax(penWidth, 1.0);
                pen.setWidth(penWidth);
                painter->setPen(pen);
            }
        }
        
        /*
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(110,210,210));
        painter->drawRect(tLR);
        painter->setBrush(QColor(254,184,126));
        painter->drawRect(tRR);
        painter->setBrush(QColor(255,160,160));
        painter->drawRect(bLR);
        painter->setBrush(QColor(110,170,220));
        painter->drawRect(bRR);
        
        QPainterPath modelRectPath, modelEllipsePath, leftOverPath;
        modelRectPath.addRect(tLR);
        modelRectPath.addRect(tRR);
        modelRectPath.addRect(bLR);
        modelRectPath.addRect(bRR);
        modelEllipsePath.addEllipse(modelCenterPoint, radius, radius);
        leftOverPath = modelRectPath - modelEllipsePath;

        painter->setPen(Qt::NoPen);
        painter->fillPath(leftOverPath, QBrush(Qt::red));
        */

        painter->drawEllipse(modelCenterPoint, middleButton, middleButton);
        
        if(textItem){
            
            painter->setPen(Qt::NoPen);
            
            /*
             * This draws 2 gray rectangles; it makes it look
             * like the aspects are attached to the model
             */
            //double gapSize = MODEL_WIDTH / 128;
            
            //QRectF textRect = textItem->boundingRect();
            //textRect.translate(textItem->pos());
            //painter->drawRect(textRect);
            
            // vertical rect
            //QRectF trv(0, 0, textRect.height(), textRect.width());
            //trv.translate(modelCenterPoint - QPointF(textRect.height()/2, textRect.width()/2));
            //painter->drawRect(trv);
            
            // this evens out the colours around the model button
            QRectF textRect = textItem->boundingRect();
            textRect.translate(textItem->pos() + QPoint(textRect.width()/4 + selectedPen.width()/2, 0));
            textRect.setWidth(textRect.width()/2 - selectedPen.width());
            painter->drawRect(textRect);
        }
        
        
    }
    
    //Paint the Icon
    if(hasIcon){
        QString imageURL = nodeKind;
        if(nodeKind == "HardwareNode"){
            if(nodeHardwareLocalHost){
                imageURL = "localhost";
            }else{
                imageURL = nodeHardwareOS.remove(" ") + "_" + nodeHardwareArch;
            }
        }

        QRectF re = iconRect();
        painter->drawPixmap(re.x(), re.y(), re.width(), re.height(), getNodeView()->getImage("Items", imageURL));
    }

    //If a Node has a Definition, paint a Lock Icon
    if (hasDefinition){
        QRectF re = lockIconRect();

         painter->drawPixmap(re.x(), re.y(), re.width(), re.height(), getNodeView()->getImage("Actions", "Definition"));
    } else if (nodeKind == "HardwareCluster") {

        QRectF re = lockIconRect();

        painter->drawPixmap(re.x(), re.y(), re.width(), re.height(), getNodeView()->getImage("Actions", "MenuCluster"));

    }

    //If this Node has a Deployment Warning, paint a warning Icon
    if(showDeploymentWarningIcon){
         QRectF re = deploymentIconRect();
        painter->drawPixmap(re.x(), re.y(), re.width(), re.height(),
 getNodeView()->getImage("Actions", "Warning"));
    }
}


bool NodeItem::hasVisibleChildren()
{
    if(getNodeKind().endsWith("Definitions")){
        return true;
    }
    foreach(NodeItem* child, childNodeItems){
        if(!child->isHidden()){
            return true;
        }
    }
    return false;
}

bool NodeItem::modelCirclePressed(QPointF mousePosition)
{
    
    if(modelCenterPoint.isNull()){
        return false;
    }
    
    QLineF distance = QLineF(mousePosition, modelCenterPoint);
    if(distance.length() < getItemMargin()/2){
        return true;
    }
    return false;
    
}


bool NodeItem::labelPressed(QPointF mousePosition)
{
    if(textItem){
        QRectF labelRect = textItem->boundingRect();
        labelRect.translate(textItem->pos());
        if(labelRect.contains(mousePosition)){
            return true;
        }
    }
    return false;
}

bool NodeItem::deploymentIconPressed(QPointF mousePosition)
{
    return deploymentIconRect().contains(mousePosition);
}

bool NodeItem::lockIconPressed(QPointF mousePosition)
{
    return lockIconRect().contains(mousePosition);
}

bool NodeItem::labelEditable()
{
    if(getGraphML()){
        return getGraphML()->getData("label") && (!getGraphML()->getData("label")->isProtected());
    }
    return false;
}


bool NodeItem::iconPressed(QPointF mousePosition)
{
    if(hasIcon){
        if(iconRect().contains(mousePosition)){
            return true;
        }
    }
    return false;
}


/**
 * @brief NodeItem::menuArrowPressed
 * @param mousePosition
 * @return
 */
bool NodeItem::menuArrowPressed(QPointF mousePosition)
{
    if (HARDWARE_CLUSTER) {
        QRectF menuButtonRect = mapRectToScene(lockIconRect());
        if (menuButtonRect.contains(mousePosition)) {
            return true;
        }
    }
    return false;
}


NodeItem::RESIZE_TYPE NodeItem::resizeEntered(QPointF mousePosition)
{
    //Check if the Mouse is in the Bottom Right Corner.
    if(resizePolygon().containsPoint(mousePosition, Qt::WindingFill)){
        return RESIZE;
    }
    //if(this->nodeKind.endsWith("Definitions")){
    //    return NO_RESIZE;
    //}
    
    qreal cornerRadius = getCornerRadius();
    
    //Calculate the Corners for the Horizontal resize
    QPointF topLeft = boundingRect().topRight() + QPointF(-cornerRadius, cornerRadius);
    QPointF bottomRight = boundingRect().bottomRight() - QPointF(0, cornerRadius);
    
    //Check if the mouse is contained in the rectangle on the right of the NodeItem.
    QRectF horizontalResizeRectangle = QRectF(topLeft, bottomRight);
    if(horizontalResizeRectangle.contains(mousePosition)){
        return HORIZONTAL_RESIZE;
    }
    
    //Calculate the Corners for the Vertical resize
    QPointF bottomLeft = boundingRect().bottomLeft() + QPointF(cornerRadius, -cornerRadius);
    bottomRight = boundingRect().bottomRight() + QPointF(-cornerRadius, 0);
    
    //Check if the mouse is contained in the rectangle on the bottom of the NodeItem.
    QRectF verticalRect = QRectF(bottomLeft, bottomRight);
    if(verticalRect.contains(mousePosition)){
        return VERTICAL_RESIZE;
    }
    
    return NO_RESIZE;
}

/**
 * @brief NodeItem::isHidden
 * @return
 */
bool NodeItem::isHidden()
{
    return hidden;
}


double NodeItem::getWidth()
{
    return width;
    
}


double NodeItem::getHeight()
{
    return height;
}


void NodeItem::addEdgeItem(EdgeItem *line)
{
    connections.append(line);
}


void NodeItem::removeEdgeItem(EdgeItem *line)
{
    connections.removeAll(line);
}


void NodeItem::setCenterPos(QPointF pos)
{
    //pos is the new center Position.
    pos -= minimumVisibleRect().center();
    //QGraphicsItem::setPos(pos);
    
    setPos(pos);
}

QPointF NodeItem::centerPos()
{
    return pos() + minimumVisibleRect().center();
}


void NodeItem::adjustPos(QPointF delta)
{
    setLocked(false);
    
    QPointF currentPos = pos();
    currentPos += delta;
    //hasSelectionMoved = true;
    setPos(currentPos);
}

void NodeItem::adjustSize(QSizeF delta)
{
    qreal newWidth = getWidth() + delta.width();
    qreal newHeight = getHeight() + delta.height();
    
    setWidth(newWidth);
    setHeight(newHeight);
}


void NodeItem::addChildOutline(NodeItem *nodeItem, QPointF gridPoint)
{
    prepareGeometryChange();
    QRectF nodeRect = nodeItem->boundingRect();
    QPointF deltaPos = gridPoint - nodeItem->minimumVisibleRect().center();
    nodeRect.translate(deltaPos);
    outlineMap.insert(nodeItem->getID(), nodeRect);
}

void NodeItem::removeChildOutline(QString ID)
{
    if(outlineMap.contains(ID)){
        prepareGeometryChange();
        outlineMap.remove(ID);
    }
}



double NodeItem::getChildWidth()
{
    // added an offset of 0.35 here and in getChildHeight
    // to make the gap between the view aspects uniform
    if (nodeKind == "Model") {
        return MODEL_WIDTH / ASPECT_SIZE_RATIO;
    } else if(nodeKind.endsWith("Definitions")) {
        return minimumWidth / ENTITY_SIZE_RATIO;
    }else{
        return minimumWidth;
    }
}

double NodeItem::getChildHeight()
{
    if (nodeKind == "Model") {
        return MODEL_HEIGHT / ASPECT_SIZE_RATIO;
    } else {
        return getChildWidth();
    }
    
}


/**
 * @brief NodeItem::getNextChildPos
 * @return
 */
QPointF NodeItem::getNextChildPos(QRectF itemRect, bool currentlySorting)
{
    bool useItemRect = !itemRect.isNull();
    
    QPainterPath childrenPath = QPainterPath();
    //bool hasChildren = false;


    // add the children's bounding rectangles to the children path
    foreach(NodeItem* child, getChildNodeItems()){
        if(child && child->isInAspect() && !child->isHidden()){
            if(!currentlySorting || (currentlySorting && child->isSorted())){
                //hasChildren = true;
                QRectF childRect =  child->boundingRect();
                childRect.translate(child->pos());
                childrenPath.addRect(childRect);
            }
        }
    }

    // CATHLYNS CODE FOR THE SAME THING. TEST
    // work out how many grid cells are needed per child item
    // divide it by 2 - only need half the number of cells to fit the center of the item
    double startingGridPoint = ceil(getChildBoundingRect().width()/getGridSize()) / 2;
    double currentX = startingGridPoint;
    double currentY = startingGridPoint;
    
    double maxX = 0;
    bool xOutsideOfGrid = false;
    bool yOutsideOfGrid = false;

    while (true) {
        
        // get the next position; construct a child rect at that position
        QPointF nextPosition = getGridPosition(currentX, currentY);
        QRectF childRect = getChildBoundingRect();
        if(useItemRect){
            childRect = itemRect;
            childRect.translate(nextPosition - getChildBoundingRect().center());
        }else{
            //Translate to the center of the normal childBR
            childRect.translate(nextPosition - childRect.center());
        }
        
        // check if the child rect collides with an existing child item
        if (childrenPath.intersects(childRect)) {
            // if so, check the next x position
            currentX++;
            // collision means that current position is inside the grid
            xOutsideOfGrid = false;
            yOutsideOfGrid = false;
        } else {
            // if there is no collision and the current position is inside the grid
            // it's a valid position - return it
            //if (gridRect().contains(nextPosition)) {
            if (gridRect().contains(childRect)) {
                return nextPosition;
            }
            
            // if both currentX and currentY are outside of the grid,
            // it means that there is no available spot in the grid
            if (xOutsideOfGrid && yOutsideOfGrid) {
                // return a new position depending on which of the width/height is bigger
                QPointF finalPosition = getGridPosition(maxX, startingGridPoint);
                if (boundingRect().width() > boundingRect().height()) {
                    finalPosition = getGridPosition(startingGridPoint, currentY);
                }
                return finalPosition;
            }
            
            // because currentY was incremented when currentX was outside
            // of the grid, it means that it is now also out of the grid
            if (xOutsideOfGrid) {
                yOutsideOfGrid = true;
            } else {
                // when it gets into this case, it means that currentX is outside of the grid
                xOutsideOfGrid = true;
                // store the maximum x
                if (currentX > maxX) {
                    maxX = currentX;
                }
                // reset currentX then check the next y position
                currentX = startingGridPoint;
                currentY++;
            }
        }
    }

}





/**
 * @brief NodeItem::setSelected Sets the NodeItem as selected, notifies connected edges to highlight.
 * @param selected
 */
void NodeItem::setSelected(bool selected)
{
    //If the current state of the NodeItem's selection is different to the one provided.
    if(isNodeSelected != selected){
        isNodeSelected = selected;
        
        if(isNodeSelected){
            //Bring the item to the front if selected.
            setZValue(zValue() + 1);
        }else{
            //Restore the items previous ZValue.
            restoreZValue();
        }
        
        //Tell the QGraphicsItem to redraw.
        prepareGeometryChange();
        
        //Tell any edges which are connected to this NodeItem to be selected.
        emit setEdgeSelected(selected);
    }
}

/**
 * @brief NodeItem::setVisibilty Changes the Visibility of the NodeItem also notifies all connected edges.
 * @param visible
 */
void NodeItem::setVisibility(bool visible)
{
    QGraphicsItem::setVisible(visible);

    if(isLocked()){
        if(visible){
            //Put it back on the grid.
            this->parentNodeItem->addChildOutline(this, this->centerPos());
        }else{
            //Remove the outline.
            this->parentNodeItem->removeChildOutline(this->getID());
        }
    }

    emit setEdgeVisibility(visible);
}



/**
 * @brief NodeItem::graphMLDataChanged This method is called when any connected GraphMLData object updates their value.
 * @param data The GraphMLData which has been changed.
 */
void NodeItem::graphMLDataChanged(GraphMLData* data)
{
    //If we have a GraphML() object and the parent of the data provided is this.
    if(getGraphML() && data && data->getParent() == getGraphML() && !getGraphML()->isDeleting()){
        QString keyName = data->getKeyName();
        QString value = data->getValue();
        bool isDouble = false;
        double valueD = value.toDouble(&isDouble);
        
        if((keyName == "x" || keyName == "y") && isDouble){
            //If data is related to the position of the NodeItem
            //Get the current center position.
            QPointF oldCenter = centerPos();
            
            QPointF newCenter = centerPos();
            
            if(keyName == "x"){
                newCenter.setX(valueD);
            }else if(keyName == "y"){
                newCenter.setY(valueD);
            }
            
            //Update the center position.
            setCenterPos(newCenter);
            
            //Check if the X or Y has changed.
            newCenter = centerPos();
            
            if(keyName == "x" && (newCenter.x() != oldCenter.x())){
                //double newX = ignoreInsignificantFigures(newCenter.x(), oldCenter.x());

                //emit GraphMLItem_SetGraphMLData(getID(), "x", QString::number(newX));
                emit GraphMLItem_SetGraphMLData(getID(), "x", QString::number(newCenter.x()));
            }
            if(keyName == "y" && (newCenter.y() != oldCenter.y())){
                //double newY = ignoreInsignificantFigures(newCenter.y(), oldCenter.y());

                //emit GraphMLItem_SetGraphMLData(getID(), "y", QString::number(newY));
                emit GraphMLItem_SetGraphMLData(getID(), "y", QString::number(newCenter.y()));
            }

        }else if((keyName == "width" || keyName == "height") && isDouble){
            //If data is related to the size of the NodeItem
            if(keyName == "width"){
                setWidth(valueD);
            }else if(keyName == "height"){
                //If NodeItem is contracted and the new value is bigger than the minimum height.
                bool setExpanded = isContracted() && valueD > minimumHeight;

                //If NodeItem is expanded and the new value is equal to the minimum height (String comparison to ignore sigfigs)
                bool setContracted = isExpanded() && valueD == minimumHeight;

                if(setExpanded){
                    setNodeExpanded(true);
                }
                if(setContracted){
                    setNodeExpanded(false);
                }

                setHeight(valueD);
            }

            double newWidth = ignoreInsignificantFigures(valueD, width);
            double newHeight = ignoreInsignificantFigures(valueD, height);

            //Check if the Width or Height has changed.
            //if(keyName == "width" && (oldWidth != width || width != valueD)){
            if(keyName == "width" && newWidth != valueD){
                emit GraphMLItem_SetGraphMLData(getID(), "width", QString::number(newWidth));
            }
            //if(keyName == "height" && (oldHeight != height || height != valueD)){
            //if(keyName == "height" && (oldHeight != height || height != valueD)){
            if(keyName == "height" && newHeight != valueD){
                emit GraphMLItem_SetGraphMLData(getID(), "height", QString::number(newHeight));
            }
        }else if(keyName == "label"){
            //Update the Label
            updateTextLabel(value);
            
            // update connected dock node item
            emit updateDockNodeItem();
        }else if(keyName == "architecture"){
            nodeHardwareArch = value;
            update();
        }else if(keyName == "os"){
            nodeHardwareOS = value;
            update();
        }
    }
}


/**
 * @brief NodeItem::newSort
 */
void NodeItem::newSort()
{
    if(nodeKind == "Model"){
        modelSort();
        return;
    }
    
    if (sortTriggerAction) {
        // added this so sort can be un-done
        GraphMLItem_TriggerAction("NodeItem: Sorting Node");
    }

    //Get the number of un-locked items
    QMap<int, NodeItem*> toSortMap;
    QList<NodeItem*> lockedItems;
    
    foreach(NodeItem* child, getChildNodeItems()){
        Node* childNode = child->getNode();
        if(child->getChildNodeItems().size() == 0){
            //RESET SIZE.
            child->setWidth(getChildWidth());
            child->setHeight(getChildHeight());
        }
        if(!child->isVisible()){ //&& nodeKind != "Model"){
            continue;
        }
        if(child->isLocked() && GRIDLINES_ON){
            child->setSorted(true);
            lockedItems.append(child);
            continue;
        }
        //Treat sorted items as locked items.
        if(child->isSorted() && GRIDLINES_ON){
            lockedItems.append(child);
            continue;
        }
        
        Node* childParent = childNode->getParentNode();
        
        int currentSortOffset = 0;
        while(childParent){
            if(childParent == getNode()){
                break;
            }else{
                bool isInt;
                int childParentSortOrder = childParent->getDataValue("sortOrder").toInt(&isInt);
                if(isInt){
                    currentSortOffset += childParentSortOrder;
                }
            }
            childParent = childParent->getParentNode();
        }
        
        bool isInt;
        int sortPosition = childNode->getDataValue("sortOrder").toInt(&isInt);
        if(isInt){
            toSortMap.insertMulti(currentSortOffset + sortPosition, child);
        }
    }
    
    QList<NodeItem*> toSortItems = toSortMap.values();
    
    //Sort Items
    while(toSortItems.size() > 0){
        NodeItem* item = toSortItems.takeFirst();

        item->setCenterPos(getNextChildPos(item->boundingRect(), true));
        item->updateModelPosition();
        item->setSorted(true);
    }
    
    foreach(NodeItem* child, getChildNodeItems()){
        child->setSorted(false);
    }
    
}

void NodeItem::modelSort()
{
    QList<NodeItem*> children = getChildNodeItems();
    
    if(children.size() != 4 || nodeKind != "Model"){
        return;
    }
    

    
    NodeItem* topLeft = children[0];
    NodeItem* topRight = children[1];
    NodeItem* bottomLeft = children[2];
    NodeItem* bottomRight = children[3];

    QPointF gapSize = QPointF(MODEL_WIDTH / 128, MODEL_WIDTH / 128);
    
    //double margin = qMax(topLeft->boundingRect().width(), topLeft->boundingRect().height());
    //QPointF topLeftPos = QPointF(margin, margin);
    
    QPointF topLeftPos = QPointF(getItemMargin(), getItemMargin());
    qreal deltaX = topLeft->boundingRect().width() - bottomLeft->boundingRect().width();
    qreal deltaY = topLeft->boundingRect().height() - topRight->boundingRect().height();


    if(deltaX < 0){
        topLeftPos.setX(topLeftPos.x() + fabs(deltaX));
    }
    if(deltaY < 0){
        topLeftPos.setY(topLeftPos.y() + fabs(deltaY));
    }
    
    //Move top Left Item to top of Model
    topLeft->setPos(topLeftPos);
    modelCenterPoint = topLeftPos + topLeft->boundingRect().bottomRight() + gapSize;
    
    //UPDATE TEXT
    if(textItem){
        qreal labelX = modelCenterPoint.x() - (textItem->boundingRect().width()/2);
        qreal labelY = modelCenterPoint.y() - (textItem->boundingRect().height()/2);
        textItem->setPos(labelX, labelY);
    }
    
    

    
    
    //Move Top Right
    QPointF topRightPos = modelCenterPoint + QPointF(gapSize.x(), - gapSize.y() - topRight->boundingRect().height());
    topRight->setPos(topRightPos);
    
    //Move Top Right
    QPointF botLeftPos = modelCenterPoint + QPointF(-gapSize.x() - bottomLeft->boundingRect().width(),gapSize.y());
    bottomLeft->setPos(botLeftPos);
    
    
    //Move Bottom Right
    QPointF botRightPos = modelCenterPoint + gapSize;
    bottomRight->setPos(botRightPos);
    
    //Update the model size to contain the new size.
    resizeToOptimumSize(false);
}


void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //Ignore mouse Presses outside of this item
    if(!contains(event->pos()) || !PAINT_OBJECT){
        qCritical() << "NOT PAINTINT";
        if(nodeKind == "Model" && !modelCirclePressed(event->pos())){
            emit GraphMLItem_ClearSelection(true);
            return;
        }
        return;
    }

    bool control = event->modifiers().testFlag(Qt::ControlModifier);

    if(event->button() == Qt::LeftButton){
        //qCritical() << "HANDLE";

        // Check if the lock icon was clicked
        if (menuArrowPressed(event->scenePos())){
            emit NodeItem_showLockMenu(this);
            return;
        }


        currentResizeMode = resizeEntered(event->pos());

        //Enter Selected Mode.
        getNodeView()->setStateSelected();

        sendSelectSignal(true, control);




        previousScenePosition = event->scenePos();
    }
}


void NodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(isInSubView){
        return;
    }

    if(event->button() == Qt::LeftButton){
        //Handle Double-Clicking the label.
        if(labelPressed(event->pos()) && labelEditable()){
            textItem->setEditMode(true);
            return;
        }

        //Handle Double-Clicking on the Icon.
        if(iconPressed(event->pos())){
            if(hasVisibleChildren()){
                if(isExpanded()){
                    GraphMLItem_TriggerAction("Contracted Node Item");
                }else{
                    GraphMLItem_TriggerAction("Expanded Node Item");
                }

                setNodeExpanded(!isExpanded());
                updateModelSize();
            }
            return;
        }

        //Handle Double-Clicking on the Resize Icon
        if(resizeEntered(event->pos()) == RESIZE){
            if(hasVisibleChildren()){
                GraphMLItem_TriggerAction("Optimizes Size of NodeItem");
                resizeToOptimumSize();
                updateModelSize();
            }
            return;
        }

        //Handle Double-Clicking anywhere on the model.
        if(nodeKind == "Model"){
            GraphMLItem_CenterAspects();
        }
    }
}


void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    bool control = event->modifiers().testFlag(Qt::ControlModifier);
    NodeView::VIEW_STATE viewState = getNodeView()->getViewState();
    if(event->button() == Qt::LeftButton){
        if(viewState == NodeView::VS_MOVING){
            if(parentNodeItem){
                parentNodeItem->setGridVisible(false);
            }
            emit NodeItem_MoveFinished();
        }
        if(viewState == NodeView::VS_RESIZING){
            emit NodeItem_ResizeFinished(getID());
            //unsetCursor();
            currentResizeMode = NO_RESIZE;
        }
    }else if(event->button() == Qt::MiddleButton){
        if(!getNodeView()->isSubView()){
            if(control){
                GraphMLItem_TriggerAction("Sorting Node");
                newSort();
            }else{
                emit GraphMLItem_SetCentered(this);
            }
        }
    }else if(event->button() == Qt::RightButton){
        if(!(viewState == NodeView::VS_PAN || viewState == NodeView::VS_PANNING)){
            //Select before opening menu.
            sendSelectSignal(true, control);
        }
    }
}


void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!PAINT_OBJECT){
        return;
    }



    NodeView::VIEW_STATE viewState = getNodeView()->getViewState();
    if(isSelected() && event->buttons() == Qt::LeftButton){
        if(currentResizeMode != NO_RESIZE || viewState == NodeView::VS_RESIZING){
            if(viewState == NodeView::VS_SELECTED){
                getNodeView()->setStateResizing();
            }

            QPointF deltaPos = (event->scenePos() - previousScenePosition);
            previousScenePosition = event->scenePos();
            QSizeF deltaSize(deltaPos.x(), deltaPos.y());

            if(currentResizeMode == HORIZONTAL_RESIZE){
                deltaSize.setHeight(0);
            }else if(currentResizeMode == VERTICAL_RESIZE){
                deltaSize.setWidth(0);
            }

            emit NodeItem_ResizeSelection(getID(), deltaSize);
        }else if(viewState == NodeView::VS_SELECTED || viewState == NodeView::VS_MOVING){

            if(isMoveable()){
                if(viewState == NodeView::VS_SELECTED){
                    getNodeView()->setStateMoving();
                }

                QPointF deltaPos = (event->scenePos() - previousScenePosition);
                previousScenePosition = event->scenePos();

                emit NodeItem_MoveSelection(deltaPos);
            }
        }
    }
}

void NodeItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if(!contains(event->pos())){
        QGraphicsItem::hoverMoveEvent(event);
        return;
    }
    
    QString tooltip;
    
    if(hasVisibleChildren() && iconPressed(event->pos())){

        setCursor(Qt::PointingHandCursor);
        setToolTip("Double click to expand/contract entity");
        return;
    }

    if(labelPressed(event->pos())){
        return;
    }else if(lockIconPressed(event->pos())){
        QString tooltip = "Click to change the displayed hardware nodes";
        if(hasDefinition){
            tooltip = "This entity has a definition";
        }
        setToolTip(tooltip);
        return;
    }else if(deploymentIconPressed(event->pos())){
        setToolTip("Not all children entities are deployed to the same hardware node");
        return;
    }

    
    if(isNodeSelected && isResizeable()){
        currentResizeMode = resizeEntered(event->pos());

        if(currentResizeMode == RESIZE){
            setCursor(Qt::SizeFDiagCursor);
        }else if(currentResizeMode == HORIZONTAL_RESIZE){
            setCursor(Qt::SizeHorCursor);
        }else if(currentResizeMode == VERTICAL_RESIZE){
            setCursor(Qt::SizeVerCursor);
        }else if(currentResizeMode == NO_RESIZE){
            unsetCursor();
        }
    }
}

void NodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    //Unset the resize mode.
    currentResizeMode = NO_RESIZE;
    //Unset the cursor
    unsetCursor();
}


/**
 * @brief NodeItem::updateDisplayedChildren
 * @param viewMode
 */
void NodeItem::updateDisplayedChildren(int viewMode)
{
    QList<NodeItem*> childrenItems = getChildNodeItems();

    if (viewMode == ALL) {
        // show all HarwareNodes
        foreach (NodeItem* item, childrenItems) {
            item->setVisible(true && isNodeExpanded);
        }
    } else if (viewMode == CONNECTED) {
        // show connected HarwareNodes
        foreach (NodeItem* item, childrenItems) {
            if (item->getEdgeItemCount() > 0) {
                item->setVisible(true && isNodeExpanded);
            } else {
                item->setVisible(false);
            }
        }
    } else if (viewMode == UNCONNECTED) {
        // show unconnected HarwareNodes
        foreach (NodeItem* item, childrenItems) {
            if (item->getEdgeItemCount() == 0) {
                item->setVisible(true && isNodeExpanded);
            } else {
                item->setVisible(false);
            }
        }
    }

    if (viewMode != -1 && viewMode != CHILDREN_VIEW_MODE) {
        CHILDREN_VIEW_MODE = viewMode;
        sortTriggerAction = false;
        newSort();
        sortTriggerAction = true;
    }
}

/**
 * @brief NodeItem::iconRect
 * @return - The QRectF which represents the position for the Icon
 */
QRectF NodeItem::iconRect()
{
    //Construct a Rectangle to represent the icon size at the origin
    QRectF icon = QRectF(0,0, ICON_RATIO * minimumWidth, ICON_RATIO * minimumHeight);
    
    //Translate to centralize the icon in the horizontal space
    icon.translate(((1 - ICON_RATIO)/2) * minimumWidth, 0);
    
    //Translate to move the icon to its position
    icon.translate(getItemMargin(), getItemMargin());
    
    return icon;
}

/**
 * @brief NodeItem::lockIconRect
 * @return The QRectF which represents the position for the Lock Icon
 */
QRectF NodeItem::lockIconRect()
{
    //Calculate the size of the lock icon
    qreal iconSize = ((1 - ICON_RATIO) * minimumWidth) / 1;
    
    //Construct a Rectangle to represent the icon size at the origin.
    QRectF lockIcon = QRectF(0,0, iconSize, iconSize);
    
    //Translate to move the icon to its position
    lockIcon.translate(getItemMargin(), getItemMargin());
    
    return lockIcon;
}

/**
 * @brief NodeItem::deploymentIconRect
 * @return The QRectF which represents the position for the Deployment Icon
 */
QRectF NodeItem::deploymentIconRect()
{
    //Calculate the size of the lock icon
    qreal iconSize = ((1 - ICON_RATIO) * minimumWidth) / 1;
    
    //Construct a Rectangle to represent the icon size at the origin.
    QRectF deploymentIcon = QRectF(0,0, iconSize, iconSize);
    
    //Translate to move the icon to its position
    deploymentIcon.translate(getItemMargin() + width - iconSize, getItemMargin());
    
    return deploymentIcon;
    
}

bool NodeItem::compareTo2Decimals(qreal num1, qreal num2)
{
    int number1To3 = qRound(num1 * 100.0);
    int number2To3 = qRound(num2 * 100.0);
    
    return number1To3 == number2To3;
}

bool NodeItem::isInResizeMode()
{
    return currentResizeMode != NO_RESIZE;
}

bool NodeItem::isMoveable()
{
    return !(nodeKind.endsWith("Definitions") || nodeKind == "Model");
}

bool NodeItem::isResizeable()
{
    return isSelected() && isExpanded() && this->hasVisibleChildren();
}


void NodeItem::updateModelData()
{
    //Give the current Width and height. update the width/height variable in the GraphML Model.
    GraphML* modelEntity = getGraphML();
    if(modelEntity){
        GraphMLData* wData = modelEntity->getData("width");
        GraphMLData* hData = modelEntity->getData("height");
        GraphMLData* xData = modelEntity->getData("x");
        GraphMLData* yData = modelEntity->getData("y");
        wData->setValue(QString::number(width));
        hData->setValue(QString::number(height));
        QPointF center = centerPos();
        xData->setValue(QString::number(center.x()));
        yData->setValue(QString::number(center.y()));
    }
}

void NodeItem::resizeToOptimumSize(bool updateParent)
{
    QRectF rect = getMinimumChildRect();

    setWidth(rect.width(), updateParent);
    setHeight(rect.height(), updateParent);
}

NodeItem *NodeItem::getChildNodeItemFromNode(Node *child)
{
    foreach(NodeItem* childNI , childNodeItems){
        if(childNI->getNode() == child){
            return childNI;
        }
    }
    return 0;
}




void NodeItem::setWidth(qreal w, bool updateParent)
{   
    bool widthChanged = false;


    if(isExpanded()){
        w = qMax(w, getMinimumChildRect().width());
        expandedWidth = w;
    }else{
        w = minimumWidth;
    }

    if(w > width || w < width){
        widthChanged = true;
    }
    
    
    prepareGeometryChange();
    width = w;
    
    updateTextLabel();
    
    calculateGridlines();
    //updateParent();

    isOverGrid(centerPos());
    
    if(getNodeKind() == "Model"){
        emit model_PositionChanged();
    }


    if(updateParent && widthChanged && getParentNodeItem()){
        getParentNodeItem()->childUpdated();
    }
    
    emit nodeItemMoved();
}


void NodeItem::setHeight(qreal h, bool updateParent)
{
    bool heightChanged = false;
    if(isExpanded()){
        h = qMax(h, getMinimumChildRect().height());
        expandedHeight = h;
    }else{
        h = minimumHeight;
    }

    if(h > height || h < height){
        heightChanged = true;
    }

    prepareGeometryChange();
    height = h;
    
    calculateGridlines();
    updateChildrenOnChange();

    isOverGrid(centerPos());
    
    if(getNodeKind() == "Model"){
        emit model_PositionChanged();
    }
    

    if(updateParent && heightChanged && getParentNodeItem()){
        getParentNodeItem()->childUpdated();
    }
    emit nodeItemMoved();
}

void NodeItem::setSize(qreal w, qreal h)
{
    setWidth(w);
    setHeight(h);
}

/**
 * @brief NodeItem::calculateGridlines Calculates the horizontal and vertical gridlines to be drawn by paint.
 */
void NodeItem::calculateGridlines()
{
    if(GRIDLINES_ON){
        QRectF boundingGridRect = gridRect();
        xGridLines.clear();
        yGridLines.clear();
        
        //Don't draw first.
        for(qreal x = boundingGridRect.left() + getGridSize(); x <= boundingGridRect.right(); x += getGridSize()){
            xGridLines << QLineF(x, boundingGridRect.top(), x, boundingGridRect.bottom());
        }
        
        for(qreal y = boundingGridRect.top() + getGridSize(); y <= boundingGridRect.bottom(); y += getGridSize()){
            yGridLines << QLineF(boundingGridRect.left(), y, boundingGridRect.right(), y);
        }
    }
}


void NodeItem::setPaintObject(bool paint)
{
    PAINT_OBJECT = paint;
    
    
    if(textItem){
        textItem->setVisible(paint);
    }
}


bool NodeItem::isAncestorSelected()
{
    if(parentNodeItem){
        if(parentNodeItem->isSelected()){
            return true;
        }else{
            return parentNodeItem->isAncestorSelected();
        }
    }
    return false;
}


/**
 * @brief NodeItem::setGridVisible Sets the grid as visible.
 * @param visible
 */
void NodeItem::setGridVisible(bool visible)
{
    prepareGeometryChange();
    isGridVisible = visible;
}


void NodeItem::updateTextLabel(QString newLabel)
{
    if(newLabel != ""){
        nodeLabel = newLabel;
    }
    
    if(!textItem){
        return;
    }
    
    
    if(nodeKind != "Model"){
        textItem->setTextWidth(width);
    }else{
        textItem->setVisible(true);
        textItem->setTextWidth(getItemMargin());
        
        //return;
    }
    
    if (newLabel != "") {
        textItem->setPlainText(newLabel);
        textItem->setParent(this);;
    }
}

QRectF NodeItem::getMinimumChildRect()
{
    qreal itemMargin = getItemMargin();
    qreal childItemMargin = getChildItemMargin(); //Use this to grow to be square.
    
    QPointF topLeft(itemMargin, itemMargin);
    QPointF bottomRight((itemMargin) + minimumWidth, (itemMargin) + minimumHeight);
    
    foreach(NodeItem* child, childNodeItems){
        if((child->isVisible() || isExpanded())){// && !child->isHidden()){
            qreal childMaxX = child->pos().x() + child->boundingRect().width() + childItemMargin;
            qreal childMaxY = child->pos().y() + child->boundingRect().height() + childItemMargin;
            
            if(childMaxX >= bottomRight.x()){
                bottomRight.setX(childMaxX);
            }
            if(childMaxY >= bottomRight.y()){
                bottomRight.setY(childMaxY);
            }
        }
    }

    
    QRectF rectangle = QRectF(topLeft, bottomRight);
    return rectangle;
}


void NodeItem::setupAspect()
{
    Node* node = getNode();
    
    while(node){
        QString nodeKind = node->getDataValue("kind");
        if(nodeKind == "ManagementComponent"){
            viewAspects.append("Hardware");
            viewAspects.append("Assembly");
        }else if(nodeKind == "HardwareDefinitions"){
            viewAspects.append("Hardware");
        }else if(nodeKind == "AssemblyDefinitions"){
            viewAspects.append("Assembly");
        }else if(nodeKind == "BehaviourDefinitions"){
            viewAspects.append("Workload");
        }else if(nodeKind == "InterfaceDefinitions"){
            viewAspects.append("Definitions");
        }
        
        if (nodeKind == "DeploymentDefintions") {
            if (!viewAspects.contains("Hardware")) {
                viewAspects.append("Hardware");
            }
            if (!viewAspects.contains("Assembly")) {
                viewAspects.append("Assembly");
            }
        }
        
        node = node->getParentNode();
    }
}


void NodeItem::setupBrushes()
{
    //QString nodeKind= getGraphML()->getDataValue("kind");
    
    
    if(nodeKind == "BehaviourDefinitions"){
        color = QColor(254,184,126);
    }
    else if(nodeKind == "InterfaceDefinitions"){
        color = QColor(110,210,210);
    }
    else if(nodeKind == "HardwareDefinitions"){
        color = QColor(110,170,220);
    }
    else if(nodeKind == "AssemblyDefinitions"){
        color = QColor(255,160,160);
    }else{
        color = QColor(200,200,200);
    }
    /*
      
    if(nodeKind== "OutEventPort"){
        color = QColor(0,250,0);
    }
    else if(nodeKind== "OutEventPortInstance"){
        color = QColor(0,200,0);
    }
    else if(nodeKind== "OutEventPortImpl"){
        color = QColor(0,150,0);
    }
    else if(nodeKind== "OutEventPortDelegate"){
        color = QColor(0,100,0);
    }
    else if(nodeKind== "InEventPort"){
        color = QColor(250,0,0);
    }
    else if(nodeKind== "InEventPortInstance"){
        color = QColor(200,0,0);
    }
    else if(nodeKind== "InEventPortImpl"){
        color = QColor(150,0,0);
    }
    else if(nodeKind== "InEventPortDelegate"){
        color = QColor(100,0,0);
    }
    else if(nodeKind== "Component"){
        color = QColor(200,200,200);
    }
    else if(nodeKind== "ComponentInstance"){
        color = QColor(150,150,150);
    }
    else if(nodeKind== "ComponentImpl"){
        color = QColor(100,100,100);
    }
    else if(nodeKind== "Attribute"){
        color = QColor(0,0,250);
    }
    else if(nodeKind== "AttributeInstance"){
        color = QColor(0,0,200);
    }
    else if(nodeKind== "AttributeImpl"){
        color = QColor(0,0,150);
    }
    else if(nodeKind== "HardwareNode"){
        color = QColor(0,250,250);
    }
    else if(nodeKind== "HardwareCluster"){
        color = QColor(200,200,200);
    }
    
    
    else if(nodeKind == "BehaviourDefinitions"){
        //color = QColor(240,240,240);
        color = QColor(254,184,126);
    }
    else if(nodeKind == "InterfaceDefinitions"){
        //color = QColor(240,240,240);
        color = QColor(110,210,210);
    }
    else if(nodeKind == "HardwareDefinitions"){
        //color = QColor(240,240,240);
        color = QColor(110,170,220);
    }
    else if(nodeKind == "AssemblyDefinitions"){
        //color = QColor(240,240,240);
        color = QColor(255,160,160);
    }
    
    else if(nodeKind== "File"){
        color = QColor(150,150,150);
    }
    else if(nodeKind== "ComponentAssembly"){
        color = QColor(200,200,200);
    }
    
    else if(nodeKind== "Aggregate"){
        color = QColor(200,200,200);
    }
    else if(nodeKind== "AggregateMember"){
        color = QColor(150,150,150);
    }
    else if(nodeKind== "Member"){
        color = QColor(100,100,100);
    }else{
        color = QColor(0,100,0);
    }*/
    
    
    selectedColor = color;
    
    brush = QBrush(color);
    selectedBrush = QBrush(selectedColor);
    
    pen.setColor(Qt::gray);
    pen.setWidth(1);
    selectedPen.setColor(Qt::blue);
    selectedPen.setWidth(24);
    
}


void NodeItem::setPos(qreal x, qreal y)
{
    setPos(QPointF(x,y));
}


void NodeItem::setPos(const QPointF &pos)
{
    if(pos != this->pos()){
        prepareGeometryChange();
        QGraphicsItem::setPos(pos);
        
        // need to check if GRID is turned on
        isOverGrid(centerPos());
        

        //updateParent();
        
        if(getNodeKind() == "Model"){
            emit model_PositionChanged();
        }

        if(getParentNodeItem()){
            getParentNodeItem()->childPosUpdated();
        }
        emit nodeItemMoved();
    }
}



/**
 * @brief NodeItem::setupChildrenViewOptionMenu
 */
void NodeItem::setupChildrenViewOptionMenu()
{
    childrenViewOptionMenu = new QMenu();

    QFont font = childrenViewOptionMenu->font();
    font.setPointSize(9);
    
    childrenViewOptionMenu->setFont(font);
    childrenViewOptionMenu->setFixedSize(115, 68);
    childrenViewOptionMenu->setAttribute(Qt::WA_TranslucentBackground);
    childrenViewOptionMenu->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    childrenViewOptionMenu->setStyleSheet("QRadioButton::checked{ color: darkRed; font-weight: bold; }"
                                          "QRadioButton{ padding: 3px; }"
                                          "QMenu{ padding: 5px;"
                                          "border-radius: 8px;"
                                          "background-color: rgba(240,240,240,245); }");

    allChildren = new QRadioButton("All");
    connectedChildren = new QRadioButton("Connected");
    unConnectedChildren = new QRadioButton("Unconnected");

    QWidgetAction* a1 = new QWidgetAction(this);
    QWidgetAction* a2 = new QWidgetAction(this);
    QWidgetAction* a3 = new QWidgetAction(this);

    a1->setDefaultWidget(allChildren);
    a2->setDefaultWidget(connectedChildren);
    a3->setDefaultWidget(unConnectedChildren);

    childrenViewOptionMenu->addAction(a1);
    childrenViewOptionMenu->addAction(a2);
    childrenViewOptionMenu->addAction(a3);

    connect(allChildren, SIGNAL(clicked()), this, SLOT(updateChildrenViewMode()));
    connect(connectedChildren, SIGNAL(clicked()), this, SLOT(updateChildrenViewMode()));
    connect(unConnectedChildren, SIGNAL(clicked()), this, SLOT(updateChildrenViewMode()));
    connect(allChildren, SIGNAL(clicked()), childrenViewOptionMenu, SLOT(hide()));
    connect(connectedChildren, SIGNAL(clicked()), childrenViewOptionMenu, SLOT(hide()));
    connect(unConnectedChildren, SIGNAL(clicked()), childrenViewOptionMenu, SLOT(hide()));
    connect(childrenViewOptionMenu, SIGNAL(aboutToHide()), this, SLOT(menuClosed()));
    
    // set the intial mode to only show connected HarwareNodes
    //connectedChildren->setChecked(true);
}



//Dont be N-Factorial
void NodeItem::childUpdated()
{
    if(!getGraphML()){
        return;
    }

    QSizeF minSize = getMinimumChildRect().size();

    bool okay = false;
    double modelWidth = getGraphMLDataValue("width").toDouble(&okay);
    if(!okay){
        return;
    }
    double modelHeight = getGraphMLDataValue("height").toDouble(&okay);
    if(!okay){
        return;
    }

    //bool widthChanged = false;
    //bool heightChanged = false;
    //Maximize on the current size in the Model and the minimum child rectangle
    if(minSize.width() > modelWidth){
        setWidth(minSize.width());
        //widthChanged = true;
    }

    if(minSize.height() > modelHeight){
        setHeight(minSize.height());
        //heightChanged = true;
    }

    if(nodeKind == "Model"){
        //Sort after any model changes!
        newSort();
    }
    //if(widthChanged){
    //   emit GraphMLItem_SetGraphMLData(getID(), "width", QString::number(width));
    //}

    //if(heightChanged){
    //    emit GraphMLItem_SetGraphMLData(getID(), "height", QString::number(height));
    //}

}


void NodeItem::aspectsChanged(QStringList visibleAspects)
{

    bool visible = true;
    foreach(QString requiredAspect, viewAspects){
        if(!visibleAspects.contains(requiredAspect)){
            visible = false;
        }
    }

    isNodeInAspect = visible;
    
    // still need to update the isInAspect state in both these cases
    // just don't change the visibility of the node item
    if(hidden || !PAINT_OBJECT){
        return;
    }
    if(getParentNodeItem() && !getParentNodeItem()->isExpanded()){
        return;
    }

    if (isNodeInAspect) {
        if (HARDWARE_CLUSTER) {
            updateDisplayedChildren(CHILDREN_VIEW_MODE);
        } else if (nodeKind == "HardwareNode"){
            if (parentNodeItem && parentNodeItem->getNodeKind() == "HardwareCluster") {
                // only show the HardwareNode if it matches its parent cluster's view mode
                int viewMode = parentNodeItem->getChildrenViewMode();
                if (viewMode == CONNECTED && getEdgeItemCount() == 0) {
                    visible = false;
                }
            }
        }
    }
    
    //bool prevVisible = isVisible();
    
    setVisibility(visible);
    
    
    // if not visible, unselect node item
    if (!isVisible()) {
        setSelected(false);
    }
}


/**
 * @brief NodeItem::setupIcon
 * This sets up the scale and tranformation of this item's icon
 * if it has one. It also updates the pos for this item's label.
 */
void NodeItem::setupIcon()
{
    /*
    QString nodeKind = getNodeKind();
    
    if (nodeKind == "HardwareNode") {
        QString hardwareOS = (getNode()->getDataValue("os")).remove(QChar::Space);
        QString hardwareArch = getNode()->getDataValue("architecture");
        nodeKind = hardwareOS + "_" + hardwareArch;
    }
    
    // get the icon images
    QImage image (":/Actions/" + nodeKind + ".png");
    if (!image.isNull() && !nodeKind.endsWith("Definitions")) {
        icon = new QGraphicsPixmapItem(QPixmap::fromImage(image), this);
        icon->setTransformationMode(Qt::SmoothTransformation);
        icon->setToolTip("Double Click to Expand/Contract Node");
    }
    QImage lockImage (":/Actions/lock.png");
    if (!lockImage.isNull()){
        lockIcon = new QGraphicsPixmapItem(QPixmap::fromImage(lockImage), this);
        lockIcon->setTransformationMode(Qt::SmoothTransformation);
    }
    
    if (icon) {
        //The amount of space should be 1 - the FONT_RATIO
        
        qreal iconHeight = icon->boundingRect().height();
        qreal iconWidth = icon->boundingRect().width();
        
        //Calculate the Scale Factor for the icon.
        qreal scaleFactor = (ICON_RATIO * minimumHeight) / iconHeight;
        icon->setScale(scaleFactor);
        
        //Update the icon sizes post scale.
        iconWidth *= scaleFactor;
        iconHeight *= scaleFactor;
        
        //Calculate the x such that it would be in the horizontal center of the minimumVisibleRect
        qreal iconX = (minimumVisibleRect().width() - iconWidth) /2;
        //Calculate the y such that it would be in the vertical center of the iconRatio * minimumVisibleRect
        qreal iconY = getItemMargin() + ((ICON_RATIO * minimumHeight) - iconHeight) /2;
        
        icon->setPos(iconX, iconY);
    }
    
    if(lockIcon){
        //ICON SHOULD FIT ON THE LEFT OF
        qreal iconSpace = ((1 - ICON_RATIO) * minimumHeight)/2;
        
        
        qreal iconWidth = lockIcon->boundingRect().width();
        qreal iconHeight = lockIcon->boundingRect().height();
        
        //Calculate the Scale Factor for the icon.
        qreal scaleFactor = (iconSpace / iconWidth);
        lockIcon->setScale(scaleFactor);
        
        //Update the icon sizes post scale.
        iconWidth *= scaleFactor;
        iconHeight *= scaleFactor;
        
        lockIcon->setPos(getItemMargin(), getItemMargin());
    }
    
    
    QImage hardwareImage (":/Actions/redHardwareNode.png");
    if (!hardwareImage.isNull()) {
        hardwareIcon = new QGraphicsPixmapItem(QPixmap::fromImage(hardwareImage), this);
        hardwareIcon->setTransformationMode(Qt::SmoothTransformation);
        hardwareIcon->setToolTip("Not all children entities are deployed to the same hardware node.");
        
        qreal iconSpace = ((1 - ICON_RATIO) * minimumHeight)/2;
        qreal iconWidth = hardwareIcon->boundingRect().width();
        qreal scaleFactor = (iconSpace / iconWidth);
        
        hardwareIcon->setScale(scaleFactor);
        hardwareIcon->setPos(boundingRect().width() - getItemMargin() - iconWidth*scaleFactor, getItemMargin());
        hardwareIcon->setVisible(false);
    }
    */
}


/**
 * @brief NodeItem::setupLabel
 * This sets up the font and size of the label.
 */
void NodeItem::setupLabel()
{
    // this updates this item's label
    if (nodeKind.endsWith("Definitions")) {
        return;
    }
    
    float fontSize = qMax((LABEL_RATIO / 2) * minimumHeight, 1.0);
    if(nodeKind == "Model"){
        fontSize = qMax(getItemMargin() * LABEL_RATIO, 1.0);
    }
    
    QFont font("Arial", fontSize);
    
    textItem = new EditableTextItem(this);
    connect(textItem, SIGNAL(textUpdated(QString)),this, SLOT(labelUpdated(QString)));
    connect(textItem, SIGNAL(editableItem_hasFocus(bool)), this, SIGNAL(Nodeitem_HasFocus(bool)));
    
    textItem->setTextWidth(minimumWidth);
    
    if(nodeKind == "Model"){
        textItem->setCenterJustified();
        textItem->setTextWidth(getItemMargin());
    } else {
        textItem->setToolTip("Double click to edit label.");
    }
    
    qreal labelX = (minimumVisibleRect().width() - textItem->boundingRect().width()) /2;
    qreal labelY = getItemMargin() + (ICON_RATIO * minimumHeight);
    
    textItem->setFont(font);
    textItem->setPos(labelX, labelY);
    
    updateTextLabel(getGraphMLDataValue("label"));
}


/**
 * @brief NodeItem::setupGraphMLConnections
 */
void NodeItem::setupGraphMLConnections()
{
    GraphML* modelEntity = getGraphML();
    if(modelEntity){
        GraphMLData* xData = modelEntity->getData("x");
        GraphMLData* yData = modelEntity->getData("y");
        GraphMLData* hData = modelEntity->getData("height");
        GraphMLData* wData = modelEntity->getData("width");
        
        GraphMLData* kindData = modelEntity->getData("kind");
        GraphMLData* labelData = modelEntity->getData("label");

        GraphMLData* osData = modelEntity->getData("os");
        GraphMLData* archData = modelEntity->getData("architecture");
        GraphMLData* localNode = modelEntity->getData("localhost");
        
        if(nodeKind == "HardwareNode"){
            if(osData){
                connect(osData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
                nodeHardwareOS = osData->getValue();
            }
            if(archData){
                connect(archData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
                nodeHardwareArch = archData->getValue();
            }
            if(localNode){
                QString value = localNode->getValue();
                nodeHardwareLocalHost = value == "true";
            }else{
                nodeHardwareLocalHost = false;
            }
        }
        
        if(xData){
            connect(xData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
        }
        
        if(yData){
            connect(yData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
        }
        
        if(hData){
            connect(hData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
        }
        
        if(wData){
            connect(wData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
        }
        
        if(labelData){
            connect(labelData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
        }
        
        if(kindData){
            connect(kindData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
        }
    }
}

QPointF NodeItem::isOverGrid(const QPointF centerPosition)
{  

    if(!GRIDLINES_ON || !parentNodeItem || nodeKind.endsWith("Definitions")){
        if(parentNodeItem){
            isNodeOnGrid = false;
            parentNodeItem->removeChildOutline(getID());
        }
        return QPointF();
    }

    QPointF gridPoint = parentNodeItem->getClosestGridPoint(centerPosition);

    //Calculate the distance between the centerPosition and the closestGrid
    qreal distance = QLineF(centerPosition, gridPoint).length();

    bool isMoving = getNodeView()->getViewState() == NodeView::VS_MOVING || getNodeView()->getViewState() == NodeView::VS_RESIZING;
    //If the distance is less than the SNAP_PERCENTAGE
    if((distance / minimumWidth) <= SNAP_PERCENTAGE){
        if(isNodeOnGrid || isMoving){
            isNodeOnGrid = true;
            parentNodeItem->addChildOutline(this, gridPoint);
        }
        return gridPoint;
    }else{
        isNodeOnGrid = false;

        if(isMoving){
            parentNodeItem->removeChildOutline(getID());
        }
        return QPointF();
    }

}




void NodeItem::toggleGridLines(bool on)
{
    GRIDLINES_ON = on;
    if(on){
        calculateGridlines();
    }
}




void NodeItem::snapToGrid()
{
    if(parentNodeItem && isVisible()){
        QPointF gridPosition = parentNodeItem->getClosestGridPoint(centerPos());
        
        setCenterPos(gridPosition);
    }
}


void NodeItem::snapChildrenToGrid()
{
    foreach(NodeItem *child, childNodeItems){
        if(child->isVisible()){
            child->snapToGrid();
            /*
            QPointF localPosition = child->minimumVisibleRect().center();
            QPointF childPosition = child->pos() + localPosition;
            QPointF newPosition = getClosestGridPoint(childPosition);
            
            if(childPosition != newPosition){
                child->setPos(newPosition - localPosition);
                // this needs to be called because setPos isn't immediately updating the node's pos
                // if it's not called, the item jumps back to it's prev pos when dragged
                //child->updateGraphMLPosition();
            }
            */
        }
    }
}


/**
 * @brief NodeItem::menuClosed
 */
void NodeItem::menuClosed()
{
    emit NodeItem_lockMenuClosed(this);
}


/**
 * @brief NodeItem::updateChildrenViewMode
 * @param viewMode
 */
void NodeItem::updateChildrenViewMode(int viewMode)
{
    if (HARDWARE_CLUSTER) {

        // if the sender was an action, it means that this was triggered from the menu
        QRadioButton* action = qobject_cast<QRadioButton*>(QObject::sender());

        if (action) {

            if (action == allChildren) {
                viewMode = ALL;
            } else if (action == connectedChildren) {
                viewMode = CONNECTED;
            } else if (action == unConnectedChildren) {
                viewMode = UNCONNECTED;
            } else {
                qWarning() << "NodeItem::updateChildrenViewMode - There is no action for this option";
                return;
            }

            if (eventFromMenu) {
                emit nodeItem_menuClicked(viewMode);
            } else {
                eventFromMenu = true;
            }

        } else {

            // this happens when this function is called from the context toolbar
            if (viewMode != -1) {
                CHILDREN_VIEW_MODE = viewMode;
                eventFromMenu = false;
            }

            switch (CHILDREN_VIEW_MODE) {
            case ALL:
                allChildren->click();
                break;
            case CONNECTED:
                connectedChildren->click();
                break;
            case UNCONNECTED:
                unConnectedChildren->click();
                break;
            default:
                qWarning() << "NodeItem::updateChildrenViewMode - There is no action for this state";
                break;
            }

            return;
        }

        updateDisplayedChildren(viewMode);
    }
}


void NodeItem::updateGraphMLPosition()
{
    //Give the current Width and height. update the width/height variable in the GraphML Model.
    GraphML* modelEntity = getGraphML();
    if(modelEntity){
        GraphMLData* xData = modelEntity->getData("x");
        GraphMLData* yData = modelEntity->getData("y");
        QPointF center = centerPos();
        xData->setValue(QString::number(center.x()));
        yData->setValue(QString::number(center.y()));
    }
    
}


void NodeItem::updateChildrenOnChange()
{
    nodeItemMoved();
}


void NodeItem::retrieveGraphMLData()
{
    
    if(!getGraphML()){
        return;
    }
    
    if(!getGraphML()->isDeleting()){
        graphMLDataChanged(getGraphML()->getData("width"));
        graphMLDataChanged(getGraphML()->getData("height"));
        graphMLDataChanged(getGraphML()->getData("x"));
        graphMLDataChanged(getGraphML()->getData("y"));
        graphMLDataChanged(getGraphML()->getData("label"));
    }
}


/**
 * @brief NodeItem::getChildNodeItems
 * @return
 */
QList<NodeItem *> NodeItem::getChildNodeItems()
{
    QList<NodeItem*> insertOrderList;
    
    foreach(QString ID, childrenIDs){
        insertOrderList.append(childNodeItems[ID]);
    }
    return insertOrderList;
    
}


qreal NodeItem::getGridSize()
{
    return getChildBoundingRect().width() / GRID_RATIO;
}


/**
 * @brief NodeItem::getAspects
 * This returns the view aspects that this node item belongs to.
 * @return
 */
QStringList NodeItem::getAspects()
{
    return viewAspects;
}



bool NodeItem::isInAspect()
{
    return isNodeInAspect;
}


/**
 * @brief NodeItem::isInAspect
 * This returns whether this node item is in the currently viewed aspects or not.
 * @return
 */
/*
bool NodeItem::isInAspect()
{
    // if this node item doesn't belong to any view aspects,
    // or there are currently no view aspects turned on, return false
    if (viewAspects.count() == 0 || currentViewAspects.count() == 0) {
        return false;
    }
    
    // otherwise, check the list of currently viewed aspects to see if it contains
    // all of the aspects that need to be turned on for this item to be visible
    bool inAspect = true;
    
    for(int i = 0; i < viewAspects.size();i++){
        QString aspect = viewAspects[i];
        if(!currentViewAspects.contains(aspect)){
            inAspect = false;
            break;
        }
    }
    
    return inAspect;
}
*/




/**
 * @brief NodeItem::higlightNodeItem
 * If highlight is true, this method highlights this node item to show that
 * the hardware node that it's deployed to is different to its parent's.
 * This is also used to remove the highlight when the deployment view is turned off.
 * @param highlight
 */
void NodeItem::highlightNodeItem(bool highlight)
{
    highlighted = highlight;
}


/**
 * @brief NodeItem::showHardwareIcon
 * When the deployment view is on, this shows the red hardware icon denoting
 * that this item has a child that is deployed to a different hardware node.
 * @param show
 */
void NodeItem::showHardwareIcon(bool show)
{
    showDeploymentWarningIcon = show;
    //update(deploymentIconRect());
}


/**
 * @brief NodeItem::deploymentView
 * @param on
 * @param selectedItem
 * @return
 */
QList<NodeItem*> NodeItem::deploymentView(bool on, NodeItem *selectedItem)
{
    QList<NodeItem*> chlidrenDeployedToDifferentNode;
    
    if (on) {
        
        Node* deploymentLink = 0;
        
        // get the hardware node that this item is deployed to
        foreach (Edge* edge, getNode()->getEdges(0)) {
            if (edge->isDeploymentLink()) {
                deploymentLink = edge->getDestination();
                break;
            }
        }
        
        // if this item isn't connected to a hardware node, do nothing
        if (!deploymentLink) {
            return chlidrenDeployedToDifferentNode;
        }
        
        // check this item's children's deployment links
        foreach (NodeItem* childItem, getChildNodeItems()) {
            foreach (Edge* edge, childItem->getNode()->getEdges(0)) {
                if (edge->isDeploymentLink() && edge->getDestination() != deploymentLink) {
                    if (selectedItem && selectedItem == this) {
                        childItem->highlightNodeItem(true);
                    }
                    chlidrenDeployedToDifferentNode.append(childItem);
                    break;
                }
            }
        }
        
        // if there are children deployed to a different node, show red hardware icon
        if (chlidrenDeployedToDifferentNode.isEmpty()) {
            showHardwareIcon(false);
        } else {
            showHardwareIcon(true);
        }
        
    } else {
        
        // remove highlight and hide the red hradware icon
        foreach (NodeItem* childItem, getChildNodeItems()) {
            childItem->highlightNodeItem(false);
        }
        
        showHardwareIcon(false);
    }
    
    // need to update here otherwise the visual changes aren't applied till the mouse is moved
    update();
    
    return chlidrenDeployedToDifferentNode;
}


/**
 * @brief NodeItem::getChildrenViewMode
 * @return
 */
int NodeItem::getChildrenViewMode()
{
   return CHILDREN_VIEW_MODE;
}



bool NodeItem::isSorted()
{
    return isNodeSorted;
}

void NodeItem::setSorted(bool isSorted)
{
    isNodeSorted = isSorted;
}


/**
 * @brief NodeItem::getChildrenViewOptionMenu
 * @return
 */
QMenu *NodeItem::getChildrenViewOptionMenu()
{
    return childrenViewOptionMenu;
}


/**
 * @brief NodeItem::geChildrenViewOptionMenuSceneRect
 * @return
 */
QRectF NodeItem::geChildrenViewOptionMenuSceneRect()
{
    if (HARDWARE_CLUSTER) {
        QRectF menuButtonRect = mapRectToScene(lockIconRect());
        return menuButtonRect;
    }
    return QRectF();
}


/**
 * @brief NodeItem::resizePolygon
 * @return Gets a QPolygonF which represents the bottom right triangle resize area for a NodeItem.
 */
QPolygonF NodeItem::resizePolygon()
{
    QPointF bottomRight = boundingRect().bottomRight();
    qreal cornerRadius = getCornerRadius();
    qreal extraDistance = cornerRadius * .25;
    QVector<QPointF> points;
    
    //Top Left
    points << (bottomRight - QPointF(cornerRadius, cornerRadius));
    //Top Right
    points << (bottomRight - QPointF(0, cornerRadius));
    //Top Right Bottom
    points << (bottomRight - QPointF(0, cornerRadius - extraDistance));
    //Bottom Left Right
    points << (bottomRight - QPointF(cornerRadius - extraDistance,0));
    //Bottom Left
    points << (bottomRight - QPointF(cornerRadius, 0));
    
    return QPolygonF(points);
}

void NodeItem::sendSelectSignal(bool setSelected, bool controlDown)
{
    if(isSelected() && controlDown){
        //DeSelect on control click.
        setSelected = false;
    }

    if(isSelected() != setSelected){
        if(setSelected && !controlDown){
            emit GraphMLItem_ClearSelection();
        }
        if(setSelected){
            emit GraphMLItem_AppendSelected(this);
        }else{
            emit GraphMLItem_RemoveSelected(this);
        }
    }
}




void NodeItem::parentNodeItemMoved()
{
    //If parent moved, this moved, and thus children of this moved.
    nodeItemMoved();
}
/*
  
void NodeItem::graphMLDataChanged(QString keyName, QString type, QString value)
{
    QString previousValue;
    
    if(keyName == "x" || keyName == "y"){
    
        //Update the Position
        QPointF newCenter = centerPos();
        
        if(keyName == "x"){
            previousValue = QString::number(newCenter.x());
            newCenter.setX(value.toDouble());
        }else if(keyName == "y"){
            previousValue = QString::number(newCenter.y());
            newCenter.setY(value.toDouble());
        }
        
        setCenterPos(newCenter);
        
        if(previousValue != value){
            updateModelPosition();
        }
        
        
    }else if(keyName == "width" || keyName == "height"){
        if(value == "inf"){
            value = QString::number(MODEL_WIDTH);
        }
        
        if(keyName == "width"){
            previousValue = QString::number(width);
            setWidth(value.toDouble());
        }else if(keyName == "height"){
            previousValue = QString::number(height);
            
            bool expand = value.toDouble() > height;
            bool contract = value == minimumHeightStr;
            
            //If the value of the height is bigger than the minimumHeight, we should expand.
            if(!isExpanded() && expand){
                setNodeExpanded(true);
            }else if(isExpanded() && contract){
                setNodeExpanded(false);
            }
            
            //Then set the height.
            setHeight(value.toDouble());
            
        }
        if(previousValue != value){
            updateortize();
        }
        
    }
    else if(keyName == "label"){
    
        if(value != ""){
            updateTextLabel(value);
        }
        
        // update connected dock node item
        emit updateDockNodeItem();
    }
    
    
    
}*/


/**
 * @brief NodeItem::getChildKind
 * This returns a list of kinds of all this item's children.
 * @return
 */
QStringList NodeItem::getChildrenKind()
{
    QStringList childrenKinds;
    Node *node = dynamic_cast<Node*>(getGraphML());
    if (node) {
        
        foreach(Node* child, node->getChildren(0)){
            childrenKinds.append(child->getDataValue("kind"));
        }
    }
    return childrenKinds;
}


/**
 * @brief NodeItem::getCornerRadius
 * @return
 */
double NodeItem::getCornerRadius()
{
    double itemMargin = getItemMargin();
    //if(nodeKind.endsWith("Definitions")){
    //    itemMargin *= 2;
    //}
    return itemMargin;
}

double NodeItem::getChildCornerRadius()
{
    double itemMargin = getItemMargin();
    if(nodeKind.endsWith("Definitions")){
        itemMargin /= ENTITY_SIZE_RATIO;
    }
    return itemMargin;
}


/**
 * @brief NodeItem::getMaxLabelWidth
 * @return
 */
double NodeItem::getMaxLabelWidth()
{
    // calculate font metrics here
    return 0;
}

QSizeF NodeItem::getModelSize()
{
    
    float graphmlHeight = 0;
    float graphmlWidth = 0;
    
    graphmlWidth = getGraphMLDataValue("width").toDouble();
    graphmlHeight = getGraphMLDataValue("height").toDouble();
    
    
    
    
    return QSizeF(graphmlWidth, graphmlHeight);
}



bool NodeItem::drawGridlines()
{
    return isGridVisible && GRIDLINES_ON;
}


double NodeItem::getItemMargin() const
{
    
    return (MARGIN_RATIO * minimumWidth);
}

double NodeItem::getChildItemMargin()
{
    if(nodeKind.endsWith("Definitions")){
        return getItemMargin() / ENTITY_SIZE_RATIO;
    }
    return getItemMargin();
    
}

double NodeItem::ignoreInsignificantFigures(double model, double current)
{
    double absDifferences = fabs(model-current);
    if(absDifferences > .1){
        return current;
    }
    return model;
}


/**
 * @brief NodeItem::expandItem
 * @param show
 */
void NodeItem::setNodeExpanded(bool expanded)
{
    //Can't Contract a Definition or Model
    if(!getGraphML() || nodeKind.endsWith("Definitions") || nodeKind == "Model"){
        isNodeExpanded = true;
        return;
    }
    
    //If our state is already set, don't do anything!
    if(isNodeExpanded == expanded){
        return;
    }
    
    isNodeExpanded = expanded;

    // if expanded, only show the HardwareNodes that match the current chidldren view mode
    if (HARDWARE_CLUSTER && expanded) {

        // this will show/hide HardwareNodes depending on the current view mode
        updateDisplayedChildren(CHILDREN_VIEW_MODE);

        // this sets the width and height to their expanded values
        setWidth(expandedWidth);
        setHeight(expandedHeight);

        return;
    }
    
    //Show/Hide the non-hidden children.
    foreach(NodeItem* nodeItem, childNodeItems){
        qCritical() << nodeItem->getNode();
        if (!nodeItem->isHidden()){
            nodeItem->setVisibility(expanded);
        }
    }
    
    if(isExpanded()){
        //Set the width/height to their expanded values.
        setWidth(expandedWidth);
        setHeight(expandedHeight);

    } else {
        //Set the width/height to their minimum values.
        setWidth(minimumWidth);
        setHeight(minimumHeight);
    }
}






void NodeItem::updateModelPosition()
{
    //Update the Parent Model's size first to make sure that the undo states are correct.
    if(nodeKind.endsWith("Definitions")){
        return;
    }

    bool isMoving = getNodeView()->getViewState() == NodeView::VS_MOVING || getNodeView()->getViewState() == NodeView::VS_RESIZING;

    //if we are over a grid line (or within a snap ratio)
    QPointF gridPoint = isOverGrid(centerPos());
    if(!gridPoint.isNull()){
        //Setting new Center Point
        setCenterPos(gridPoint);
        //If the node moved via the mouse, lock it.
        if(isSelected() && isMoving){
            setLocked(isNodeOnGrid);
        }
    }
    if(parentNodeItem){
        parentNodeItem->updateModelSize();
    }
    
    GraphMLItem_SetGraphMLData(getID(), "x", QString::number(centerPos().x()));
    GraphMLItem_SetGraphMLData(getID(), "y", QString::number(centerPos().y()));
    
    
    if(!isNodeOnGrid && parentNodeItem){
        parentNodeItem->removeChildOutline(getID());
    }

    
    GraphMLItem_PositionSizeChanged(this);
}

void NodeItem::updateModelSize()
{
    if(nodeKind == "Model"){
        //return;
    }
    if(this->parentNodeItem){
        parentNodeItem->updateModelSize();
    }
    //Update the Size in the model.
    GraphMLItem_SetGraphMLData(getID(), "width", QString::number(width));
    GraphMLItem_SetGraphMLData(getID(), "height", QString::number(height));
    
    bool isMoving = getNodeView()->getViewState() == NodeView::VS_MOVING || getNodeView()->getViewState() == NodeView::VS_RESIZING;

    //If we are over a gridline already.
    if(isNodeOnGrid){
        //Update the gridPoint.
        isOverGrid(centerPos());
    }
    
    
    if(isMoving){
        setLocked(isNodeOnGrid);
    }
    
    //if (width > prevWidth || height > prevHeight) {
    GraphMLItem_PositionSizeChanged(this, true);
    //}
}


/**
 * @brief NodeItem::updateSceneRect
 * This gets called everytime there has been a change to the view's sceneRect.
 * @param sceneRect
 */
void NodeItem::sceneRectChanged(QRectF sceneRect)
{
    currentSceneRect = sceneRect;
}

void NodeItem::labelUpdated(QString newLabel)
{
    if(getGraphML()){
        
        QString currentLabel = getGraphMLDataValue("label");
        
        if(currentLabel != newLabel){
            if(getGraphML() && !getGraphML()->getData("label")->isProtected()){
                GraphMLItem_TriggerAction("Set New Label");
                GraphMLItem_SetGraphMLData(getID(), "label", newLabel);
            }
        }
    }
}

void NodeItem::setNewLabel(QString newLabel)
{
    if(getGraphML()){
        if(newLabel != ""){
            if(getGraphML() && !getGraphML()->getData("label")->isProtected()){
                GraphMLItem_TriggerAction("Set New Label");
                GraphMLItem_SetGraphMLData(getID(), "label", newLabel);
            }
        }else{
            if(textItem){
                if(getGraphML() && !getGraphML()->getData("label")->isProtected()){
                    
                    textItem->setEditMode(true);
                }
            }
        }
    }
    
}


Node *NodeItem::getNode()
{
    //if(!IS_DELETING){

    return (Node*)getGraphML();
    //}
    //return 0;
}


/**
 * @brief NodeItem::getNodeKind
 * @return
 */
QString NodeItem::getNodeKind()
{
    return nodeKind;
}

QString NodeItem::getNodeLabel()
{
    return nodeLabel;
}


/**
 * @brief NodeItem::setHidden
 * This method is used to prevent this item from being shown
 * when the view aspects are changed. If this item is meant to
 * be hidden no matter the view aspect, this keeps it hidden.
 */
void NodeItem::setHidden(bool h)
{
    hidden = h;
    //setVisible(!h);
    
    // when the item is no longer hidden, we needs to check if it's in aspect
    // and if its parent is visible before we can display it in the view
    bool parentExpanded = true;
    if (getParentNodeItem()) {
        parentExpanded = getParentNodeItem()->isExpanded();
    }
    setVisibility(!h && isInAspect() && parentExpanded);
}


/**
 * @brief NodeItem::resetSize
 * Reset this node item's size to its default size.
 */
void NodeItem::resetSize()
{
    GraphMLItem_SetGraphMLData(getID(), "height", QString::number(minimumHeight));
    GraphMLItem_SetGraphMLData(getID(), "width", QString::number(minimumWidth));
}


/**
 * @brief NodeItem::isExpanded
 */
bool NodeItem::isExpanded()
{
    return isNodeExpanded;
}

bool NodeItem::isContracted()
{
    if(this->childItems().count() ==0){
        return true;
    }
    return !isExpanded();
}

/**
 * @brief NodeItem::getClosestGridPoint
 * @param centerPoint - The Center Point of a newly constructed child
 * @return The closest Grid Point which can contain a child!
 */
QPointF NodeItem::getClosestGridPoint(QPointF centerPoint)
{
    //Get the gridSize
    qreal gridSize = getGridSize();
    
    //Offset the centerPoint by the starting position of the gridRect so we can do easy division.
    QPointF gridOffset = gridRect().topLeft();
    if(nodeKind == "Model"){
        //qCritical() << gridOffset;
    }
    centerPoint -= gridOffset;
    
    //Find the closest gridline to the centerPoint, then add the offset again.
    centerPoint.setX((qRound(centerPoint.x() / gridSize) * gridSize) + gridOffset.x());
    centerPoint.setY((qRound(centerPoint.y() / gridSize) * gridSize) + gridOffset.y());
    
    //Calculate the Bounding Rect of the the child.
    QPointF childOffset = QPointF(getChildWidth()/2, getChildHeight()/2);
    QRectF childRect = QRectF(centerPoint - childOffset, centerPoint + childOffset);
    
    //Check for collision with the parent boudning box.
    if(!gridRect().contains(childRect)){
        //Make sure the Top of the child is fully contained!
        while(childRect.top() <= gridRect().top()){
            childRect.translate(0, gridSize);
        }
        //Make sure the Left of the child is fully contained!
        while(childRect.left() <= gridRect().left()){
            childRect.translate(gridSize, 0);
        }
        return childRect.center();
    }
    
    return centerPoint;
}
