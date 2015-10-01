#include "nodeviewminimap.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QPen>
#define GRID_COUNT 50
//#define LINEWIDTH 400
#define LINEWIDTH 20
#define GRACE 1000

#define ZOOM_SCALE_INCREMENTOR 1.05
#define ZOOM_SCALE_DECREMENTOR 1.0 / ZOOM_SCALE_INCREMENTOR
#define MAX_ZOOM_RATIO 50
#define MIN_ZOOM_RATIO 2
#include <QVBoxLayout>
#include <QLabel>

NodeViewMinimap::NodeViewMinimap(QObject*)
{
    isPanning = false;
    setMouseTracking(true);
    setupLayout();
}

void NodeViewMinimap::centerView()
{
    if (scene()) {
        fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
}

void NodeViewMinimap::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout();


    layout->setSpacing(0);
    layout->setMargin(0);



    QLabel* label = new QLabel("Minimap");

    //label->setStyleSheet("color:#3C3C3C; font-size:10px; font-weight: bold; background-color: rgb(210,210,210);border-bottom: 1px solid;border-color: black;padding-bottom: 1px;");
    label->setStyleSheet("color:#3C3C3C; background-color: rgb(210,210,210);border-bottom: 1px solid;border-color: rgb(50,50,50); padding: 3px;");
    label->setAlignment(Qt::AlignCenter);

    // set all gui widget fonts to this
    QFont font("Verdana");
    font.setPointSizeF(10);
    label->setFont(font);

    layout->addWidget(label);
    layout->addStretch();

    setLayout(layout);
}

void NodeViewMinimap::viewportRectChanged(QRectF viewport)
{
    this->viewport = viewport;
}

bool NodeViewMinimap::viewportContainsPoint(QPointF localPos)
{
    QPointF scenePos = mapToScene(localPos.toPoint());
    return viewport.contains(scenePos);
}


void NodeViewMinimap::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);
    // this darkens the area in the scene that's not currently visualised by the view
    // it also still draws a rectangle representing what is currently shown in the view
    if (scene()) {
        QRectF scenePath = sceneRect();
        double padding = sceneRect().width()/8;
        scenePath.adjust(-padding, -padding, padding, padding);

        QPainterPath path, viewPath;
        path.addRect(scenePath);
        viewPath.addRect(viewport);
        path -= viewPath;

        painter->setPen(Qt::NoPen);
        QBrush brush(QColor(0,0,0,100));


        painter->setBrush(brush);
        painter->drawPath(path);


        brush.setColor(QColor(0,0,0,0));
        painter->setBrush(brush);


        QPen pen(QColor(250,250,250));
        if(isPanning){
            pen.setColor(QColor(0,0,250));
        }
        pen.setWidth(LINEWIDTH);
        painter->setPen(pen);
        painter->drawRect(viewport);
    }
}


void NodeViewMinimap::mousePressEvent(QMouseEvent *event)
{
    if(viewportContainsPoint(event->pos()) && event->button() == Qt::LeftButton){
        isPanning = true;
        previousScenePos = mapToScene(event->pos());
        emit minimap_Pan();
        update();
    }
}

void NodeViewMinimap::mouseReleaseEvent(QMouseEvent *event)
{
    isPanning = false;
    emit minimap_Panned();
    update();
}

void NodeViewMinimap::mouseMoveEvent(QMouseEvent *event)
{
    if(isPanning){
        QPoint currentMousePos = event->pos();
        QPointF currentPos = mapToScene(currentMousePos);
        QPointF delta = previousScenePos - currentPos;
        emit minimap_Panning(delta);
        //Update the previous Scene position after the view has panned, such that we get smooth movement.
        previousScenePos = mapToScene(currentMousePos);

    }
}

void NodeViewMinimap::wheelEvent(QWheelEvent *event)
{
    minimap_Scrolled(event->delta());
}
