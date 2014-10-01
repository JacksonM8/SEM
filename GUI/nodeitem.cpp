#include "nodeitem.h"
#include <QGraphicsTextItem>
#include "nodeconnection.h"
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QRubberBand>
NodeItem::NodeItem(Node *node, NodeItem *parent):  GraphMLItem(node), QGraphicsItem()
{
    viewAspect = "";
    drawDetail = true;
    drawObject = true;
    USING_RUBBERBAND_SELECTION = false;

    hasMoved = false;
    graphicsEffect = new QGraphicsColorizeEffect(this);

    QColor blue(70,130,180);
    graphicsEffect->setColor(blue);
    graphicsEffect->setStrength(0);

    rubberBand = new QRubberBand(QRubberBand::Rectangle,0);
    QPalette palette;
    palette.setBrush(QPalette::Foreground, QBrush(Qt::green));
    palette.setBrush(QPalette::Base, QBrush(Qt::red));

    rubberBand->setPalette(palette);
    rubberBand->resize(500, 500);



    this->isPressed = false;
    this->node = node;
    this->setParentItem(parent);

    label  = new QGraphicsTextItem("NULL",this);

    if(parent == 0){
        depth = 1;
        this->width = 1000;
        this->height = 1000;
    }else{
        depth = parent->depth +1;
        this->width = parent->width/4;
        this->height =  parent->height/4;
    }

    bRec = QRect(0,0,this->width, this->height);


    GraphMLData* xData = node->getData("x");
    GraphMLData* yData = node->getData("y");
    GraphMLData* kindData = node->getData("kind");
    GraphMLData* labelData = node->getData("label");

    GraphMLData* hData = node->getData("height");
    GraphMLData* wData = node->getData("width");


    connect(xData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(updatedData(GraphMLData*)));
    connect(yData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(updatedData(GraphMLData*)));
    connect(hData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(updatedData(GraphMLData*)));
    connect(wData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(updatedData(GraphMLData*)));
    connect(labelData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(updatedData(GraphMLData*)));
    connect(kindData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(updatedData(GraphMLData*)));

    emit updatedData(xData);
    emit updatedData(yData);
    emit updatedData(kindData);
    emit updatedData(labelData);

    if(wData->getValue() == ""){
        wData->setValue(QString::number(width));
    }

    if(hData->getValue() == ""){
        hData->setValue(QString::number(height));
    }


    this->setGraphicsEffect(graphicsEffect);

   //setFlag(ItemDoesntPropagateOpacityToChildren);
    setFlag(ItemIgnoresParentOpacity);
    setFlag(ItemIsSelectable);
    if(parent == 0){
        //PARENT MODEL!
    }else{
    }

}

NodeItem::~NodeItem()
{
}


QRectF NodeItem::boundingRect() const
{
    return bRec;
}


void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);


    label->setVisible(drawDetail);

    if(drawObject){
        QRectF rectangle = boundingRect();

        QBrush Brush(Qt::white);

        if(kind == "OutEventPortIDL"){
            Brush.setColor(QColor(180,0,0));
        }else if(kind == "OutEventPort"){
            Brush.setColor(QColor(255,0,0));
        }else if(kind == "InEventPortIDL"){
            Brush.setColor(QColor(0,180,0));
        }else if(kind == "InEventPort"){
            Brush.setColor(QColor(0,255,0));
        }

        else if(kind == "ComponentInstance"){
             Brush.setColor(QColor(200,200,200));
        }else if(kind == "Attribute"){
            Brush.setColor(Qt::blue);
        }else if(kind == "HardwareNode"){
            Brush.setColor(Qt::yellow);
        }else if(kind == "HardwareCluster"){
            Brush.setColor(QColor(255,0,255));
        }else if(kind == "PeriodicEvent"){
            Brush.setColor(QColor(255,153,0));
        }else if(kind == "Component"){
            Brush.setColor(Qt::gray);

        }
        painter->fillRect(rectangle, Brush);
        painter->drawRect(rectangle);
    }
}

void NodeItem::notifyEdges()
{
    for(int i =0;i< connections.size();i++){
        connections[i]->updateLine();
    }

    for(int i=0;i<this->childItems().size();i++){
        NodeItem * childNode = dynamic_cast<NodeItem*>(childItems()[i]);
        if(childNode != 0){
            childNode->notifyEdges();
        }
    }
}

void NodeItem::addConnection(NodeEdge *line)
{
    connections.append(line);
}

void NodeItem::deleteConnnection(NodeEdge *line)
{
    int position = connections.indexOf(line);
    connections.remove(position);
}


QVariant NodeItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedChange)
    {
        if (value == true)
        {
            emit setNodeSelected(node, true);
        }else{
            emit setNodeSelected(node, false);
        }
        //return 0;
    }

    return QGraphicsItem::itemChange(change, value);
}




void NodeItem::setOpacity(qreal opacity)
{
    QGraphicsItem::setOpacity(opacity);
}


void NodeItem::setSelected2()
{
    if(graphicsEffect != 0){
        graphicsEffect->setStrength(1);
    }

    for(int i =0;i< connections.size();i++){
        if(connections[i] != 0){
            connections[i]->setSelected(true);
        }
    }
    this->update(this->boundingRect());
}

void NodeItem::setSelected(bool selected)
{
    if(selected){
        if(graphicsEffect != 0){
            graphicsEffect->setStrength(1);
        }

        for(int i =0;i< connections.size();i++){
            if(connections[i] != 0){
                connections[i]->setSelected(true);
            }
        }

        //itemChange(QGraphicsItem::ItemSelectedChange, true);

    }else{
        if(graphicsEffect != 0){
            graphicsEffect->setStrength(0);
        }

        for(int i =0;i< connections.size();i++){
            if(connections[i] != 0){
            connections[i]->setSelected(false);
            }
        }
        //itemChange(QGraphicsItem::ItemSelectedChange, false);


    }

}

void NodeItem::setDeselected2()
{
    if(graphicsEffect != 0){
        graphicsEffect->setStrength(0);
    }

    for(int i =0;i< connections.size();i++){
        if(connections[i] != 0){
        //connections[i]->s();
        }
    }
}

void NodeItem::toggleDetailDepth(int level)
{

    if(level>=depth){
        drawDetail = true;
    }else{
        if(level + 1 < depth){
            drawObject = false;

        }else{
            drawObject = true;
        }
        drawDetail = false;
    }
    notifyEdges();
    update();
}


void NodeItem::updatedData(GraphMLData* data)
{
    QString dataKey = data->getKey()->getName();
    QString dataValue = data->getValue();

    if(dataKey == "x"){
        updatePosition(dataValue,0);
    }else if(dataKey == "y"){
        updatePosition(0,dataValue);
    }else if(dataKey == "label"){
        dataValue = dataValue + " [" + node->getID()+"]";
        QFont font("Arial");
        font.setPointSize(1);
        QFontMetrics fm(font);

        if(dataValue !=""){
            float factor = width / fm.width(dataValue);

            font.setPointSizeF(font.pointSizeF()*factor);
            label->setFont(font);
        }


        label->setPlainText(dataValue);
    }else if(dataKey == "kind"){
        kind = dataValue;
        update();
    }
    else if(dataKey == "width"){
        updateSize(dataValue,0);
    }
    else if(dataKey == "height"){
        updateSize(0,dataValue);
    }

    if(dataKey == "x" || dataKey == "y"){
        notifyEdges();
    }
}

void NodeItem::recieveData()
{
    name = node->getDataValue("label");

    QString x = node->getDataValue("x");
    QString y = node->getDataValue("y");

    this->setPos(QPointF(x.toDouble(),y.toDouble()));
    notifyEdges();

}

void NodeItem::destructNodeItem()
{
    delete this;

}

void NodeItem::updateChildNodeType(QString type)
{
    toBuildType = type;
}

void NodeItem::updateViewAspect(QString aspect)
{
    viewAspect = aspect;

    bool isVisible = node->inAspect(viewAspect);

    this->setVisible(isVisible);

    foreach(NodeEdge* edge, connections){
        edge->setVisible(isVisible);
    }

    update();

}

void NodeItem::sortChildren()
{
    emit actionTriggered("Sorting Children");
    int currentX  = width/10;
    int currentY = height/5;

    foreach(QGraphicsItem* children, this->childItems()){

        NodeItem* nodeItem = dynamic_cast<NodeItem*>(children);
        if(nodeItem != 0){

        if((currentX + nodeItem->width * 1.1) > width){
            currentY += nodeItem->height * 1.1;
            currentX = width/10;
        }


        //nodeItem->node->updateDataValue("x",QString::number(currentX));
        //nodeItem->node->updateDataValue("y",QString::number(currentY));

        emit updateGraphMLDataValue(nodeItem->getGraphML(),"x",QString::number(currentX));
        emit updateGraphMLDataValue(nodeItem->getGraphML(),"y",QString::number(currentY));

        currentX += nodeItem->width * 1.1;
        }
    }
}

void NodeItem::setRubberbandMode(bool On)
{
    USING_RUBBERBAND_SELECTION = On;
}


void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(!drawObject){
        return;
    }

    switch (event->button()) {

    case Qt::MiddleButton:{
        sortChildren();
        emit centreNode(node);
        //emit centreNode(this);
        break;
    }
    case Qt::LeftButton:{
        if(USING_RUBBERBAND_SELECTION){
            origin = event->screenPos();
            sceneOrigin = mapToScene(event->pos());
            rubberBand->setGeometry(QRect(origin.toPoint(), QSize()));
            rubberBand->show();
        }else{
            //Left and Right buttons should target this line.
            previousPosition = event->scenePos();
            hasMoved = false;
            isPressed = true;
            //emit triggerSelected(this);
            emit setItemSelected(node);
//            emit setNodeSelected(node);
        }
        break;
    }
    case Qt::RightButton:{
        //Select this node, and construct a child node.
        emit setItemSelected(node, true);
        emit makeChildNode(event->pos());
        break;

    }

    default:{

        break;
    }
    }
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(!drawObject){
        return;
    }

    switch (event->button()) {
    case Qt::LeftButton:{
        if(USING_RUBBERBAND_SELECTION){
            QPointF scenePos = mapToScene(event->pos());
            QRect selectionRectangle(sceneOrigin.toPoint(), scenePos.toPoint());
            QPainterPath pp;
            pp.addRect(selectionRectangle);

            scene()->setSelectionArea(pp,Qt::ContainsItemBoundingRect);
            rubberBand->hide();
        }else{
            hasMoved = false;
            isPressed = false;
        }
        break;
    }
    default:
        break;

    }
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!drawObject){
        return;
    }

    if(USING_RUBBERBAND_SELECTION){
        if(boundingRect().contains(event->pos())){
            rubberBand->setGeometry(QRect(origin.toPoint(), event->screenPos()).normalized());
        }
    }else if(isPressed){
        if(hasMoved == false){
            emit actionTriggered("Moving Selection");
        }
        QPointF delta = (event->scenePos() - previousPosition);
        this->setPos(pos() + delta);

        emit moveSelection(delta);
        hasMoved = true;
        previousPosition = event->scenePos();
    }
}

void NodeItem::updatePosition(QString x, QString y)
{
    qreal xR = pos().x();
    qreal yR = pos().y();
    if(x !=0){
        xR = x.toDouble();
    }
    if(y != 0){
        yR = y.toDouble();
    }
    this->setPos(xR,yR);
}

void NodeItem::updateSize(QString w, QString h)
{
    if(w != 0){
        width = w.toDouble();
    }
    if(h != 0){
        height = h.toDouble();
    }
     bRec = QRect(0,0,width, height);
     update();

}


