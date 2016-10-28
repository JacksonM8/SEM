#include "nodeview.h"

#include <QDebug>
#include <QtMath>
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QDateTime>
#include <QOpenGLWidget>

#include "SceneItems/Node/defaultnodeitem.h"
#include "SceneItems/Node/stacknodeitem.h"
#include "SceneItems/Node/managementcomponentnodeitem.h"
#include "SceneItems/Node/hardwarenodeitem.h"

#include "SceneItems/Edge/edgeitem.h"
#include "SceneItems/Edge/edgeitem.h"
#include "theme.h"


#define ZOOM_INCREMENTOR 1.05

NodeView::NodeView(QWidget* parent):QGraphicsView(parent)
{
    setupStateMachine();
    QRectF sceneRect;
    sceneRect.setSize(QSize(10000,10000));
    sceneRect.moveCenter(QPointF(0,0));
    setSceneRect(sceneRect);

    connectLineItem = 0;
    //OPENGL
    //setViewport(new QOpenGLWidget());
    //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);


    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);

    //Set QT Options for this QGraphicsView
    setDragMode(NoDrag);
    setAcceptDrops(true);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);


    //Turn off the Scroll bars.
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    isPanning = false;
    viewController = 0;
    selectionHandler = 0;
    containedAspect = VA_NONE;
    containedNodeViewItem = 0;
    isAspectView = false;

    backgroundFont.setPixelSize(70);

    rubberband = new QRubberBand(QRubberBand::Rectangle, this);

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));

    themeChanged();

}

NodeView::~NodeView()
{
    if(containedNodeViewItem){
        QList<ViewItem*> items = containedNodeViewItem->getNestedChildren();
        items.insert(0, containedNodeViewItem);

        QListIterator<ViewItem*> it(items);

        it.toBack();
        while(it.hasPrevious()){
            ViewItem* item = it.previous();
            viewItem_Destructed(item->getID(), item);
        }
    }
}

void NodeView::setViewController(ViewController *viewController)
{
    this->viewController = viewController;
    if(viewController){

        connect(viewController, &ViewController::vc_viewItemConstructed, this, &NodeView::viewItem_Constructed);
        connect(viewController, &ViewController::vc_viewItemDestructing, this, &NodeView::viewItem_Destructed);

        selectionHandler = viewController->getSelectionController()->constructSelectionHandler(this);
        connect(selectionHandler, &SelectionHandler::itemSelectionChanged, this, &NodeView::selectionHandler_ItemSelectionChanged);
        connect(selectionHandler, &SelectionHandler::itemActiveSelectionChanged, this, &NodeView::selectionHandler_ItemActiveSelectionChanged);

        connect(this, &NodeView::toolbarRequested, viewController, &ViewController::vc_showToolbar);
        connect(this, &NodeView::triggerAction, viewController, &ViewController::vc_triggerAction);
        connect(this, &NodeView::setData, viewController, &ViewController::vc_setData);
        connect(this, &NodeView::removeData, viewController, &ViewController::vc_removeData);
        connect(this, &NodeView::editData, viewController, &ViewController::vc_editTableCell);




        connect(viewController, &ViewController::vc_centerItem, this, &NodeView::centerItem);
        connect(viewController, &ViewController::vc_fitToScreen, this, &NodeView::fitToScreen);
        connect(viewController, &ViewController::vc_selectAndCenterConnectedEntities, this, &NodeView::centerConnections);


        connect(viewController, &ViewController::vc_highlightItem, this, &NodeView::highlightItem);
    }
}

void NodeView::translate(QPointF point)
{
    QGraphicsView::translate(point.x(), point.y());
    forceViewportChange();
}

void NodeView::scale(qreal sx, qreal sy)
{
    if(sx != 1 || sy != 1){
        QGraphicsView::scale(sx, sy);
        forceViewportChange();
    }
}

void NodeView::setContainedViewAspect(VIEW_ASPECT aspect)
{
    containedAspect = aspect;
    isAspectView = true;
    themeChanged();
}

void NodeView::setContainedNodeViewItem(NodeViewItem *item)
{
    if(containedNodeViewItem){
        //Unregister
        disconnect(containedNodeViewItem, &NodeViewItem::labelChanged, this, &NodeView::viewItem_LabelChanged);
        containedNodeViewItem->unregisterObject(this);
        if(!isAspectView){
            deleteLater();
        }
    }

    containedNodeViewItem = item;

    if(containedNodeViewItem){
        containedNodeViewItem->registerObject(this);

        connect(containedNodeViewItem, &NodeViewItem::labelChanged, this, &NodeView::viewItem_LabelChanged);

        viewItem_LabelChanged(item->getData("label").toString());

        containedAspect = containedNodeViewItem->getViewAspect();

        if(!isAspectView){
            viewItem_Constructed(item);
            foreach(ViewItem* item, item->getNestedChildren()){
                viewItem_Constructed(item);
            }
        }
    }
    clearSelection();
}

QColor NodeView::getBackgroundColor()
{
    return backgroundColor;
}


QRectF NodeView::getViewportRect()
{
    return viewportRect();
}

void NodeView::resetMinimap()
{
    emit viewportChanged(viewportRect(), transform().m11());
    emit sceneRectChanged(currentSceneRect);
}

void NodeView::viewItem_Constructed(ViewItem *item)
{
    if(item){
        if(item->isNode()){
            nodeViewItem_Constructed((NodeViewItem*)item);
        }else if(item->isEdge()){
            edgeViewItem_Constructed((EdgeViewItem*)item);
        }
    }
}

void NodeView::viewItem_Destructed(int ID, ViewItem *viewItem)
{


    EntityItem* item = getEntityItem(ID);
    if(item){
        topLevelGUIItemIDs.removeAll(ID);
        guiItems.remove(ID);
        if(item->scene()){
            scene()->removeItem(item);
        }

        delete item;
    }

    if(viewItem && containedNodeViewItem == viewItem){
        setContainedNodeViewItem(0);
    }
}

void NodeView::selectionHandler_ItemSelectionChanged(ViewItem *item, bool selected)
{
    if(item){
        EntityItem* e = getEntityItem(item->getID());
        if(e){
            e->setSelected(selected);
        }
        if(item == containedNodeViewItem){
            isBackgroundSelected = selected;
            update();
        }
    }
}

void NodeView::selectionHandler_ItemActiveSelectionChanged(ViewItem *item, bool isActive)
{
    if(item){
        EntityItem* e = getEntityItem(item->getID());
        if(e){
            e->setActiveSelected(isActive);
        }
    }
}


void NodeView::selectAll()
{
    _selectAll();
}

void NodeView::itemsMoved()
{
    QRectF newSceneRect = getSceneBoundingRectOfItems(getTopLevelEntityItems());
    if(newSceneRect != currentSceneRect){
        currentSceneRect = newSceneRect;
        emit sceneRectChanged(currentSceneRect);
    }
}

void NodeView::alignHorizontal()
{
    emit triggerAction("Aligning Selection Horizontally");

    QList<EntityItem*> selection = getOrderedSelectedItems();
    QRectF sceneRect = getSceneBoundingRectOfItems(selection);

    foreach(EntityItem* item, selection){
        item->setMoveStarted();
        QPointF pos = item->getPos();

        EntityItem* parent = item->getParent();
        if(!parent){
            parent = item;
        }

        pos.setY(parent->mapFromScene(sceneRect.topLeft()).y());
        pos.ry() += item->getTopLeftOffset().y();
        item->setPos(pos);

        if(item->setMoveFinished()){
            pos = item->getNearestGridPoint();
            emit setData(item->getID(), "x", pos.x());
            emit setData(item->getID(), "y", pos.y());
        }
    }
}

void NodeView::alignVertical()
{
    emit triggerAction("Aligning Selection Vertically");

    QList<EntityItem*> selection = getOrderedSelectedItems();
    QRectF sceneRect = getSceneBoundingRectOfItems(selection);

    foreach(EntityItem* item, selection){
        item->setMoveStarted();
        QPointF pos = item->getPos();

        EntityItem* parent = item->getParent();
        if(!parent){
            parent = item;
        }
        pos.setX(parent->mapFromScene(sceneRect.topLeft()).x());
        pos.rx() += item->getTopLeftOffset().x();
        item->setPos(pos);

        if(item->setMoveFinished()){
            pos = item->getNearestGridPoint();
            emit setData(item->getID(), "x", pos.x());
            emit setData(item->getID(), "y", pos.y());
        }
    }

}

void NodeView::clearSelection()
{
    _clearSelection();
}

void NodeView::themeChanged()
{

    if(isAspectView){
        backgroundColor = Theme::theme()->getAspectBackgroundColor(containedAspect);
    }else{
        backgroundColor = Theme::theme()->getAltBackgroundColor();
    }
    backgroundFontColor = backgroundColor.darker(110);


    QColor selectedColor = Qt::blue;

    //Merge to blue
    qreal ratio = 1;
    selectedBackgroundFontColor.setRed((ratio * backgroundFontColor.red() + ((1 - ratio) * selectedColor.red())));
    selectedBackgroundFontColor.setGreen((ratio *backgroundFontColor.green() + ((1 - ratio) * selectedColor.green()) ));
    selectedBackgroundFontColor.setBlue((ratio *backgroundFontColor.blue() + ((1 - ratio) * selectedColor.blue()) ));

    update();
}

void NodeView::node_ConnectMode(NodeItem *item)
{
    if(selectionHandler && selectionHandler->getSelectionCount() == 1){
        if(item->getViewItem() == selectionHandler->getActiveSelectedItem()){
            emit trans_InActive2Connecting();
        }
    }
}

void NodeView::item_EditData(ViewItem *item, QString keyName)
{
    if(selectionHandler){
        selectionHandler->setActiveSelectedItem(item);
        emit editData(item->getID(), keyName);
    }
}

void NodeView::item_RemoveData(ViewItem *item, QString keyName)
{
    if(item){
        emit removeData(item->getID(), keyName);
    }
}

void NodeView::fitToScreen()
{
    centerOnItems(getTopLevelEntityItems());
}

void NodeView::centerSelection()
{

    centerOnItems(getSelectedItems());
}

void NodeView::centerConnections(ViewItem* item)
{
    if(item){
        QList<EdgeViewItem*> edges;
        if(item->isNode()){
            edges = ((NodeViewItem*)item)->getEdges();
        }else if(item->isEdge()){
            edges.append((EdgeViewItem*)item);
        }

        QList<ViewItem*> toSelect;
        QList<EntityItem*> toCenter;

        foreach(EdgeViewItem* e, edges){
            ViewItem* s = e->getSource();
            ViewItem* d = e->getDestination();

            EntityItem* src = getEntityItem(s);
            EntityItem* dst = getEntityItem(d);
            EntityItem* edge = getEntityItem(e);

            if(src && !toSelect.contains(s)){
                toCenter.append(src);
                toSelect.append(s);
            }

            if(dst && !toSelect.contains(d)){
                toCenter.append(dst);
                toSelect.append(d);
            }

            if(edge && !toSelect.contains(e)){
                toCenter.append(edge);
                toSelect.append(e);
            }
        }
        if(!toSelect.isEmpty()){
            if(selectionHandler){
                selectionHandler->toggleItemsSelection(toSelect);
            }
            centerOnItems(toCenter);
        }else{
            clearSelection();
        }
    }
}

QList<int> NodeView::getIDsInView()
{
    return guiItems.keys();
}

void NodeView::test()
{

    //scene->setSceneRect(QRectF());
}

void NodeView::item_Selected(ViewItem *item, bool append)
{
    if(selectionHandler){
        selectionHandler->toggleItemsSelection(item, append);
    }
}

void NodeView::item_ActiveSelected(ViewItem *item)
{
    if(selectionHandler){
        selectionHandler->setActiveSelectedItem(item);
    }
}

void NodeView::item_SetExpanded(EntityItem *item, bool expand)
{
    if(item){
        int ID = item->getID();
        emit triggerAction("Expanding Selection");
        emit setData(ID, "isExpanded", expand);
    }
}

void NodeView::item_SetCentered(EntityItem *item)
{
    centerRect(item->sceneViewRect());
}

void NodeView::item_MoveSelection(QPointF delta)
{
    //Only when we are in the moving state.
    if(state_Active_Moving->active()){
        //Moves the selection.
        if(selectionHandler){

            //Validate the move for the entire selection.
            foreach(ViewItem* viewItem, selectionHandler->getOrderedSelection()){
                EntityItem* item = getEntityItem(viewItem);
                if(item){
                    delta = item->validateMove(delta);
                    //If delta is 0,0 we should ignore.
                    if(delta.isNull()){
                        break;
                    }
                }
            }

            if(!delta.isNull()){
                foreach(ViewItem* viewItem, selectionHandler->getOrderedSelection()){
                    EntityItem* item = getEntityItem(viewItem);
                    if(item){
                        //Move!
                        item->adjustPos(delta);
                    }
                }
            }
        }
    }
}

void NodeView::item_Resize(NodeItem *item, QSizeF delta, RECT_VERTEX vertex)
{
    if(state_Active_Resizing->active()){

        if(vertex == RV_TOP || vertex == RV_BOTTOM){
            delta.setWidth(0);
        }else if(vertex == RV_LEFT || vertex == RV_RIGHT){
            delta.setHeight(0);
        }

        if(vertex == RV_TOP || vertex == RV_TOPLEFT || vertex == RV_TOPRIGHT){
            //Invert the H
            delta.rheight() *= -1;
        }
        if(vertex == RV_TOPLEFT || vertex == RV_LEFT || vertex == RV_BOTTOMLEFT){
            //Invert the W
            delta.rwidth() *= -1;
        }

        QSizeF preSize = item->getExpandedSize();
        item->adjustExpandedSize(delta);
        QSizeF postSize = item->getExpandedSize();
        if(preSize != postSize){
            QSizeF deltaSize = preSize - postSize;
            QPointF offset(deltaSize.width(), deltaSize.height());

            if(vertex == RV_BOTTOM || vertex == RV_BOTTOMLEFT || vertex == RV_BOTTOMRIGHT){
                //Ignore the delta Y
                offset.setY(0);
            }
            if(vertex == RV_RIGHT || vertex == RV_BOTTOMRIGHT || vertex == RV_TOPRIGHT){
                //Ignore the delta X
                offset.setX(0);
            }
            item->adjustPos(offset);
        }
    }

}


void NodeView::minimap_Pan(QPointF delta)
{
    translate(delta);
}

void NodeView::minimap_Zoom(int delta)
{
    zoom(delta);
}

void NodeView::centerItem(int ID)
{
    EntityItem* item = getEntityItem(ID);
    if(item){
        QList<EntityItem*> items;
        items.append(item);
        centerOnItems(items);
    }
}

void NodeView::highlightItem(int ID, bool highlighted)
{
    EntityItem* item = getEntityItem(ID);
    if(item){
        item->setHighlighted(highlighted);
    }
}

void NodeView::setupConnections(EntityItem *item)
{
    connect(item, &EntityItem::req_activeSelected, this, &NodeView::item_ActiveSelected);
    connect(item, &EntityItem::req_selected, this, &NodeView::item_Selected);
    connect(item, &EntityItem::req_expanded, this, &NodeView::item_SetExpanded);
    connect(item, &EntityItem::req_centerItem, this, &NodeView::item_SetCentered);

    connect(item, &EntityItem::req_StartMove, this, &NodeView::trans_InActive2Moving);
    connect(item, &EntityItem::req_Move, this, &NodeView::item_MoveSelection);
    connect(item, &EntityItem::req_FinishMove, this, &NodeView::trans_Moving2InActive);


    connect(item, &EntityItem::req_triggerAction, this, &NodeView::triggerAction);
    connect(item, &EntityItem::req_removeData, this, &NodeView::item_RemoveData);
    connect(item, &EntityItem::req_editData, this, &NodeView::item_EditData);



    if(item->isNodeItem()){
        NodeItem* node = (NodeItem*) item;

        connect(node, &NodeItem::req_StartResize, this, &NodeView::trans_InActive2Resizing);
        connect(node, &NodeItem::req_Resize, this, &NodeView::item_Resize);
        connect(node, &NodeItem::req_FinishResize, this, &NodeView::trans_Resizing2InActive);

        //connect(node, &NodeItemNew::req_adjustSize, this, &NodeViewNew::item_Resize);
        //connect(node, &NodeItemNew::req_adjustSizeFinished, this, &NodeViewNew::item_ResizeFinished);
        connect(node, &NodeItem::req_connectMode, this, &NodeView::node_ConnectMode);


    }
}


void NodeView::centerOnItems(QList<EntityItem *> items)
{
    centerRect(getSceneBoundingRectOfItems(items));
}

QRectF NodeView::getSceneBoundingRectOfItems(QList<EntityItem *> items)
{
    QRectF itemsRect;
    foreach(EntityItem* item, items){
        if(item->isVisible()){
            itemsRect = itemsRect.united(item->sceneViewRect());
        }
    }
    return itemsRect;
}

void NodeView::centerRect(QRectF rectScene)
{
    if(rectScene.isValid()){
        //Inflate by 110%
        QRectF visibleRect = viewportRect();
        qreal widthRatio = visibleRect.width() / (rectScene.width() * 1.1);
        qreal heightRatio = visibleRect.height() / (rectScene.height() * 1.1);

        qreal scaleRatio = qMin(widthRatio, heightRatio);

        //Change the scale.
        scale(scaleRatio, scaleRatio);
        centerView(rectScene.center());
    }
}

void NodeView::centerView(QPointF scenePos)
{
    QPointF delta = viewportRect().center() - scenePos;
    translate(delta);
    viewportCenter_Scene = viewportRect().center();
}

void NodeView::forceViewportChange()
{
    emit viewportChanged(viewportRect(), transform().m11());
}

SelectionHandler *NodeView::getSelectionHandler()
{
    return selectionHandler;
}

void NodeView::viewItem_LabelChanged(QString label)
{
    backgroundText = label.toUpper();
    update();
}

QRectF NodeView::viewportRect()
{
//    return mapToScene(rect()).boundingRect();
    return mapToScene(viewport()->rect()).boundingRect();
}

void NodeView::nodeViewItem_Constructed(NodeViewItem *item)
{
    if(!item || item->getViewAspect() != containedAspect){
        return;
    }

    NodeItem* parentNode = getParentNodeItem(item);

    if(!containedNodeViewItem && item->getViewAspect() == containedAspect){
        setContainedNodeViewItem(item);
        //Don't construct an aspect.
        return;
    }



    if(containedNodeViewItem){
        if(containedNodeViewItem->isAncestorOf(item)){
            //qCritical() << "CONSTRUCTING: " << item;
            int ID = item->getID();
            NodeItem* nodeItem =  0;
            Node::NODE_KIND nodeKind = item->getNodeKind();
            QString nodeKindStr = item->getData("kind").toString();

            //Ignore
            if(nodeKindStr.contains("DDS")){
                return;
            }

            bool ignorePosition = containedNodeViewItem == item;


            switch(nodeKind){
            case Node::NK_HARDWARE_NODE:
                nodeItem = new HardwareNodeItem(item, parentNode);
                break;
            case Node::NK_MANAGEMENT_COMPONENT:
                nodeItem = new ManagementComponentNodeItem(item, parentNode);
                break;
            case Node::NK_IDL:
            case Node::NK_COMPONENT:
            case Node::NK_COMPONENT_ASSEMBLY:
            case Node::NK_COMPONENT_INSTANCE:
            case Node::NK_BLACKBOX:
            case Node::NK_COMPONENT_IMPL:
            case Node::NK_BLACKBOX_INSTANCE:
                nodeItem = new DefaultNodeItem(item, parentNode);
                break;
            case Node::NK_TERMINATION:
                nodeItem = new DefaultNodeItem(item, parentNode);
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(Edge::EC_WORKFLOW);
                break;
            case Node::NK_HARDWARE_CLUSTER:
                nodeItem = new DefaultNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("ip_address");
                break;
            case Node::NK_INEVENTPORT_INSTANCE:
            case Node::NK_OUTEVENTPORT_INSTANCE:
            case Node::NK_INEVENTPORT_DELEGATE:
            case Node::NK_OUTEVENTPORT_DELEGATE:
                nodeItem = new DefaultNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(Edge::EC_ASSEMBLY);
                break;
            case Node::NK_CONDITION:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setExpandEnabled(false);
                nodeItem->setSecondaryTextKey("value");
                nodeItem->setVisualEdgeKind(Edge::EC_WORKFLOW);
                break;
            case Node::NK_ATTRIBUTE_INSTANCE:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("value");
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(Edge::EC_DATA);
                break;
            case Node::NK_AGGREGATE:
                nodeItem = new StackNodeItem(item, parentNode);
                break;
            case Node::NK_PROCESS:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setIconOverlay("Functions", item->getData("operation").toString());
                nodeItem->setIconOverlayVisible(true);
                nodeItem->setSecondaryTextKey("worker");

                break;
            case Node::NK_MEMBER_INSTANCE:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(Edge::EC_DATA);
                break;
            case Node::NK_VARIABLE:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("value");
                nodeItem->setVisualEdgeKind(Edge::EC_DATA);
                break;
            case Node::NK_ATTRIBUTE_IMPL:
            case Node::NK_AGGREGATE_INSTANCE:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setVisualEdgeKind(Edge::EC_DATA);
                break;
            case Node::NK_MEMBER:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setExpandEnabled(false);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setIconOverlay("Actions", "Key");
                break;
            case Node::NK_INEVENTPORT_IMPL:
            case Node::NK_OUTEVENTPORT_IMPL:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setVisualEdgeKind(Edge::EC_WORKFLOW);
                break;
            case Node::NK_ATTRIBUTE:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setExpandEnabled(false);
                break;
            case Node::NK_INPUTPARAMETER:
            case Node::NK_RETURNPARAMETER:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(Edge::EC_DATA);
                nodeItem->setTertiaryIcon("Items", nodeKindStr);
                nodeItem->setTertiaryIconVisible(true);
                nodeItem->setSecondaryTextKey("type");
                break;
            case Node::NK_INEVENTPORT:
            case Node::NK_OUTEVENTPORT:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                break;
            case Node::NK_PERIODICEVENT:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("frequency");
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(Edge::EC_WORKFLOW);
                break;
            case Node::NK_BRANCH_STATE:
            case Node::NK_WHILELOOP:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setVisualEdgeKind(Edge::EC_WORKFLOW);
                break;
            case Node::NK_WORKLOAD:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setVisualEdgeKind(Edge::EC_WORKFLOW);
                break;
            case Node::NK_VECTOR:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("max_size");
                break;
            case Node::NK_VECTOR_INSTANCE:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setVisualEdgeKind(Edge::EC_DATA);
                nodeItem->setSecondaryTextKey("type");
                break;

            default:

                //nodeItem = new StackNodeItem(item, parentNode);
                break;
            }

            if(nodeItem){

                if(ignorePosition){
                    nodeItem->setIgnorePosition(true);
                }

                guiItems[ID] = nodeItem;

                setupConnections(nodeItem);


                if(!scene()->items().contains(nodeItem)){
                    scene()->addItem(nodeItem);
                    topLevelGUIItemIDs.append(ID);

                    connect(nodeItem, SIGNAL(positionChanged()), this, SLOT(itemsMoved()));
                    connect(nodeItem, SIGNAL(sizeChanged()), this, SLOT(itemsMoved()));

                }
            }
        }
    }
}

void NodeView::edgeViewItem_Constructed(EdgeViewItem *item)
{

    switch(item->getEdgeKind()){
        case Edge::EC_ASSEMBLY:
        case Edge::EC_DATA:
        case Edge::EC_WORKFLOW:
            break;
        default:
            return;
    }

    if(!containedNodeViewItem || !containedNodeViewItem->isAncestorOf(item->getParentItem())){
        return;
    }



    NodeItem* parent = getParentNodeItem(item->getParentItem());
    NodeItem* source = getParentNodeItem(item->getSource());
    NodeItem* destination = getParentNodeItem(item->getDestination());

    if(source && destination){
        EdgeItem* edgeItem = new EdgeItem(item, parent,source,destination);


        if(edgeItem){
            guiItems[item->getID()] = edgeItem;

            setupConnections(edgeItem);

            if(!scene()->items().contains(edgeItem)){
                scene()->addItem(edgeItem);
            }
        }
    }
}

QList<ViewItem *> NodeView::getTopLevelViewItems() const
{
    QList<ViewItem *> items;
    foreach(EntityItem* item, getTopLevelEntityItems()){
        items.append(item->getViewItem());
    }
    return items;
}

QList<EntityItem *> NodeView::getTopLevelEntityItems() const
{
    QList<EntityItem*> items;
    foreach(int ID, topLevelGUIItemIDs){
        EntityItem* item = guiItems.value(ID, 0);
        if(item){
            items.append(item);
        }
    }
    return items;
}

QList<EntityItem *> NodeView::getSelectedItems() const
{
    QList<EntityItem*> items;
    foreach(ViewItem* item, selectionHandler->getSelection()){
        EntityItem* eItem = getEntityItem(item);
        if(eItem){
            items.append(eItem);
        }
    }
    return items;
}

QList<EntityItem *> NodeView::getOrderedSelectedItems() const
{
    QList<EntityItem*> items;
    foreach(ViewItem* item, selectionHandler->getOrderedSelection()){
        EntityItem* eItem = getEntityItem(item);
        if(eItem){
            items.append(eItem);
        }
    }
    return items;

}

NodeItem *NodeView::getParentNodeItem(NodeViewItem *item)
{
     while(item){
        int ID = item->getID();
        if(guiItems.contains(ID)){
            return (NodeItem*)guiItems[ID];
        }else{
            item = item->getParentNodeViewItem();
        }
     }
     return 0;
}

EntityItem *NodeView::getEntityItem(int ID) const
{
    EntityItem* item = 0;
    if(guiItems.contains(ID)){
        item = guiItems[ID];
    }
    return item;
}

EntityItem *NodeView::getEntityItem(ViewItem *item) const
{
    EntityItem* e = 0;
    if(item){
        e = getEntityItem(item->getID());
    }
    return e;
}

NodeItem *NodeView::getNodeItem(ViewItem *item) const
{
    EntityItem* e = getEntityItem(item->getID());
    if(e && e->isNodeItem()){
        return (NodeItem*) e;
    }
    return 0;
}

void NodeView::zoom(int delta, QPoint anchorScreenPos)
{
    if(delta != 0){
        QPointF anchorScenePos;

        if(!topLevelGUIItemIDs.isEmpty()){
            anchorScenePos = getScenePosOfPoint(anchorScreenPos);
        }
        //Calculate the zoom change.
        qreal scaleFactor = pow(ZOOM_INCREMENTOR, (delta / abs(delta)));
        if(scaleFactor != 1){
            scale(scaleFactor, scaleFactor);

            if(!topLevelGUIItemIDs.isEmpty()){
                QPointF delta = getScenePosOfPoint(anchorScreenPos) - anchorScenePos;
                translate(delta);
            }
        }
    }
}

QPointF NodeView::getScenePosOfPoint(QPoint pos)
{
    if(pos.isNull()){
        //If we haven't been given a point, use the center of the viewport rect.
        pos = viewport()->rect().center();
    }
    return mapToScene(pos);
}

void NodeView::selectItemsInRubberband()
{
    QPolygonF rubberbandRect = mapToScene(rubberband->geometry());

    QList<ViewItem*> itemsToSelect;

    //Check for aspect selection.
    if(selectionHandler->getSelection().contains(containedNodeViewItem)){
        itemsToSelect.append(containedNodeViewItem);
    }

    foreach(QGraphicsItem* i, scene()->items(rubberbandRect,Qt::IntersectsItemShape)){
        EntityItem* entityItem = dynamic_cast<EntityItem*>(i);
        if(entityItem){
            itemsToSelect.append(entityItem->getViewItem());
        }
    }
    if(selectionHandler){
        selectionHandler->toggleItemsSelection(itemsToSelect, true);
    }
}


void NodeView::_selectAll()
{
    if(selectionHandler){
        EntityItem* guiItem = getEntityItem(selectionHandler->getFirstSelectedItem());

        QList<ViewItem*> itemsToSelect;

        if(guiItem){
            if(selectionHandler->getSelectionCount() == 1 && guiItem->isNodeItem()){
                NodeItem* nodeItem = (NodeItem*) guiItem;

                foreach(NodeItem* child, nodeItem->getChildNodes()){
                    itemsToSelect.append(child->getViewItem());
                }
            }
        }else{
            //Get all top level children.
            itemsToSelect = getTopLevelViewItems();
        }
        if(itemsToSelect.size() > 0){
            selectionHandler->toggleItemsSelection(itemsToSelect, false);
        }
    }
}

void NodeView::_clearSelection()
{
    if(selectionHandler){
        //Depending on the type of NodeView we are.
        if(containedNodeViewItem){
            //If we are the aspect select the aspect.
            selectionHandler->toggleItemsSelection(containedNodeViewItem);
        }else{
            //If we aren't an aspect clear the selection.
            selectionHandler->clearSelection();
        }
    }
}


qreal NodeView::distance(QPoint p1, QPoint p2)
{
    return qSqrt(qPow(p2.x() - p1.x(), 2) + qPow(p2.y() - p1.y(), 2));
}

void NodeView::setupStateMachine()
{
    viewStateMachine = new QStateMachine(this);

    state_InActive = new QState();

    state_Active_Moving = new QState();
    state_Active_Resizing = new QState();
    state_Active_RubberbandMode = new QState();
    state_Active_RubberbandMode_Selecting = new QState();
    state_Active_Connecting = new QState();

    //Add States
    viewStateMachine->addState(state_InActive);
    viewStateMachine->addState(state_Active_Moving);
    viewStateMachine->addState(state_Active_Resizing);
    viewStateMachine->addState(state_Active_RubberbandMode);
    viewStateMachine->addState(state_Active_RubberbandMode_Selecting);
    viewStateMachine->addState(state_Active_Connecting);

    viewStateMachine->setInitialState(state_InActive);

    //Setup Transitions
    state_InActive->addTransition(this, &NodeView::trans_InActive2Moving, state_Active_Moving);
    state_Active_Moving->addTransition(this, &NodeView::trans_Moving2InActive, state_InActive);

    state_InActive->addTransition(this, &NodeView::trans_InActive2RubberbandMode, state_Active_RubberbandMode);
    state_Active_RubberbandMode->addTransition(this, &NodeView::trans_RubberbandMode2InActive, state_InActive);

    state_Active_RubberbandMode->addTransition(this, &NodeView::trans_RubberbandMode2RubberbandMode_Selecting, state_Active_RubberbandMode_Selecting);
    state_Active_RubberbandMode_Selecting->addTransition(this, &NodeView::trans_RubberbandMode2InActive, state_Active_RubberbandMode);

    state_InActive->addTransition(this, &NodeView::trans_InActive2Resizing, state_Active_Resizing);
    state_Active_Resizing->addTransition(this, &NodeView::trans_Resizing2InActive, state_InActive);


    state_InActive->addTransition(this, &NodeView::trans_InActive2Connecting, state_Active_Connecting);
    state_Active_Connecting->addTransition(this, &NodeView::trans_Connecting2InActive, state_InActive);

    //Connect to states.

    connect(state_InActive, &QState::entered, this, &NodeView::state_Default_Entered);


    connect(state_Active_Moving, &QState::entered, this, &NodeView::state_Moving_Entered);
    connect(state_Active_Moving, &QState::exited, this, &NodeView::state_Moving_Exited);

    connect(state_Active_Resizing, &QState::entered, this, &NodeView::state_Resizing_Entered);
    connect(state_Active_Resizing, &QState::exited, this, &NodeView::state_Resizing_Exited);

    connect(state_Active_RubberbandMode, &QState::entered, this, &NodeView::state_RubberbandMode_Entered);
    connect(state_Active_RubberbandMode, &QState::exited, this, &NodeView::state_RubberbandMode_Exited);

    connect(state_Active_RubberbandMode_Selecting, &QState::entered, this, &NodeView::state_RubberbandMode_Selecting_Entered);
    connect(state_Active_RubberbandMode_Selecting, &QState::exited, this, &NodeView::state_RubberbandMode_Selecting_Exited);

    connect(state_Active_Connecting, &QState::entered, this, &NodeView::state_Connecting_Entered);
    connect(state_Active_Connecting, &QState::exited, this, &NodeView::state_Connecting_Exited);

    viewStateMachine->start();
}

EntityItem *NodeView::getEntityAtPos(QPointF scenePos)
{
    foreach(QGraphicsItem* item, scene()->items(scenePos)){
        EntityItem* entityItem =  dynamic_cast<EntityItem*>(item);
        if(entityItem){
            return entityItem;
        }
    }
    return 0;
}

void NodeView::state_Moving_Entered()
{
    setCursor(Qt::SizeAllCursor);
    if(selectionHandler){
        foreach(ViewItem* viewItem, selectionHandler->getOrderedSelection()){
            EntityItem* item = getEntityItem(viewItem);
            if(item){
                item->setMoveStarted();
            }
        }
    }
}

void NodeView::state_Moving_Exited()
{
    if(selectionHandler){
        bool anyMoved = false;

        QVector<ViewItem*> selection = selectionHandler->getOrderedSelection();

        foreach(ViewItem* viewItem, selection){
            EntityItem* item = getEntityItem(viewItem);
            if(item){
                if(item->setMoveFinished()){
                    anyMoved = true;
                }
            }
        }

        if(anyMoved){
            emit triggerAction("Moving Selection");
            foreach(ViewItem* viewItem, selection){
                EntityItem* item = getEntityItem(viewItem);
                if(item){
                    QPointF pos = item->getNearestGridPoint();
                    emit setData(item->getID(), "x", pos.x());
                    emit setData(item->getID(), "y", pos.y());
                }
            }
        }
    }
}

void NodeView::state_Resizing_Entered()
{
    qCritical() << "state_Resizing_Entered";
    if(selectionHandler){
        if(selectionHandler->getSelectionCount() != 1){
            emit trans_Resizing2InActive();
            return;
        }

        foreach(ViewItem* viewItem, selectionHandler->getOrderedSelection()){
            NodeItem* item = getNodeItem(viewItem);
            if(item){
                item->setResizeStarted();
            }
        }
        setCursor(Qt::SizeAllCursor);
    }
}

void NodeView::state_Resizing_Exited()
{
    if(selectionHandler){
        foreach(ViewItem* viewItem, selectionHandler->getOrderedSelection()){
            NodeItem* item = getNodeItem(viewItem);

            if(item && item->setResizeFinished()){
                emit triggerAction("Resizing Item");
                QSizeF size = item->getGridAlignedSize();
                emit setData(item->getID(), "width", size.width());
                emit setData(item->getID(), "height", size.height());
            }
        }
    }
}

void NodeView::state_RubberbandMode_Entered()
{
    setCursor(Qt::CrossCursor);
}

void NodeView::state_RubberbandMode_Exited()
{
}

void NodeView::state_RubberbandMode_Selecting_Entered()
{
    rubberband->setVisible(true);
}

void NodeView::state_RubberbandMode_Selecting_Exited()
{
    rubberband->setVisible(false);
    emit trans_RubberbandMode2InActive();
}

void NodeView::state_Connecting_Entered()
{
    ViewItem* vi = selectionHandler->getActiveSelectedItem();
    EntityItem* item = getEntityItem(vi);
    if(item && item->isNodeItem()){
        NodeItem* nodeItem = (NodeItem*) item;

        //Highlight things we can connect to
        QList<ViewItem*> items = viewController->getValidEdges(nodeItem->getVisualEdgeKind());
        foreach(ViewItem* item, items){
            highlightItem(item->getID(), true);
        }

        QPointF lineStart = nodeItem->scenePos();
        lineStart += nodeItem->getElementRect(EntityItem::ER_EDGE_KIND_ICON).center();

        if(!connectLineItem){
            connectLineItem = scene()->addLine(connectLine);
        }
        connectLine.setP1(lineStart);
        connectLine.setP2(lineStart);
        connectLineItem->setLine(connectLine);
        connectLineItem->setVisible(true);
    }
    qCritical() << "state_Connecting_Entered";
}

void NodeView::state_Connecting_Exited()
{
    ViewItem* vi = selectionHandler->getActiveSelectedItem();
    EntityItem* item = getEntityItem(vi);
    if(item && item->isNodeItem()){
        NodeItem* nodeItem = (NodeItem*) item;

        Edge::EDGE_KIND edgeKind = nodeItem->getVisualEdgeKind();

        //Unhighlight things we can connect to
        QList<ViewItem*> items = viewController->getValidEdges(edgeKind);
        foreach(ViewItem* item, items){
            highlightItem(item->getID(), false);
        }

        if(connectLineItem){
            QPointF scenePos = connectLine.p2();
            EntityItem* otherItem = getEntityAtPos(scenePos);
            if(otherItem){
                emit triggerAction("Constructing Edge");
                emit viewController->vc_constructEdge(selectionHandler->getSelectionIDs().toList(), otherItem->getID(), edgeKind);
            }
            connectLineItem->setVisible(false);
        }
    }
}

void NodeView::state_Default_Entered()
{
    unsetCursor();
}

void NodeView::keyPressEvent(QKeyEvent *event)
{
    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    if(CONTROL && SHIFT){
        emit trans_InActive2RubberbandMode();
    }
}

void NodeView::keyReleaseEvent(QKeyEvent *event)
{
    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    if(!(CONTROL && SHIFT)){
        emit trans_RubberbandMode2InActive();
    }
}

void NodeView::wheelEvent(QWheelEvent *event)
{
    //Call Zoom
    if(viewController->isModelReady()){
        zoom(event->delta(), event->pos());
    }
}

void NodeView::mousePressEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    bool handledEvent = false;

    if(event->button() == Qt::RightButton){
        isPanning = true;
        pan_lastPos = event->pos();
        pan_lastScenePos = scenePos;
        pan_distance = 0;
        handledEvent = true;
    }

    if(event->button() == Qt::LeftButton){
        emit trans_RubberbandMode2RubberbandMode_Selecting();

        if(state_Active_RubberbandMode_Selecting->active()){
            rubberband_lastPos = event->pos();
            if(rubberband){
                rubberband->setGeometry(QRect(rubberband_lastPos, rubberband_lastPos));
            }
            handledEvent = true;
        }else{
            EntityItem* item = getEntityAtPos(scenePos);
            if(!item){
                clearSelection();
                handledEvent = true;
            }
        }
    }

    if(!handledEvent){
        QGraphicsView::mousePressEvent(event);
    }
}

void NodeView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    bool handledEvent = false;

    if(isPanning){
        //Calculate the distance in screen pixels travelled
        pan_distance += distance(event->pos(), pan_lastPos);
        //Pan the Canvas
        translate(scenePos - pan_lastScenePos);
        pan_lastPos = event->pos();
        pan_lastScenePos = mapToScene(event->pos());
        handledEvent = true;
    }

    if(state_Active_RubberbandMode_Selecting->active()){
        rubberband->setGeometry(QRect(rubberband_lastPos, event->pos()).normalized());
        handledEvent = true;
    }else if(state_Active_Connecting->active()){
        connectLine.setP2(mapToScene(event->pos()));
        connectLineItem->setLine(connectLine);
    }

    //Only pass down if we haven't handled it.
    if(!handledEvent){
        QGraphicsView::mouseMoveEvent(event);
    }
}

void NodeView::mouseReleaseEvent(QMouseEvent *event)
{
    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    //bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    bool handledEvent = false;

    //Exit pan mode yo
    if(isPanning && event->button() == Qt::RightButton){
        isPanning = false;

        //Popup Toolbar if there is an item.
        if(pan_distance < 10){
            QPointF itemPos = mapToScene(event->pos());
            EntityItem* item = getEntityAtPos(itemPos);
            if(item){
                itemPos = item->mapFromScene(itemPos);
                if(!item->isSelected()){
                    selectionHandler->toggleItemsSelection(item->getViewItem(), CONTROL);
                }
            }
            //Check for item under mouse.
            emit toolbarRequested(event->globalPos(), itemPos);
        }
        handledEvent = true;
    }

    if(state_Active_RubberbandMode_Selecting->active() && event->button() == Qt::LeftButton){
        rubberband->setGeometry(QRect(rubberband_lastPos, event->pos()).normalized());
        selectItemsInRubberband();
        emit trans_RubberbandMode2InActive();
        handledEvent = true;
    }

    if(state_Active_Connecting->active() && event->button() == Qt::LeftButton){
        emit trans_Connecting2InActive();
        handledEvent = true;
    }


    if(!handledEvent){
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void NodeView::drawBackground(QPainter *painter, const QRectF &)
{
    painter->resetTransform();
    painter->setPen(Qt::NoPen);
    painter->setBrush(backgroundColor);
    painter->drawRect(rect());

    if(backgroundText != ""){
        painter->setFont(backgroundFont);
        if(isBackgroundSelected){
            painter->setPen(selectedBackgroundFontColor);
        }else{
            painter->setPen(backgroundFontColor);
        }
        painter->drawText(rect(), Qt::AlignHCenter | Qt::AlignBottom, backgroundText);
    }
}

void NodeView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    forceViewportChange();
}

