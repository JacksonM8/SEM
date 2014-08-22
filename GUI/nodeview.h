#ifndef NODEVIEW_H
#define NODEVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include "nodeitem.h"
#include <QPointF>
class NodeView : public QGraphicsView
{
    Q_OBJECT
public:
    NodeView(QWidget *parent = 0);

public slots:
    void centreItem(NodeItem* item);
protected:
    virtual void wheelEvent(QWheelEvent* event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
signals:
    virtual void updateZoom(qreal zoom);
private:
    QRectF getVisibleRect( );
    bool CONTROL_DOWN;
    bool SHIFT_DOWN;
    qreal totalScaleFactor;
};

#endif // NODEVIEW_H
