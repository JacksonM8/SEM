#ifndef NOTIFICATIONITEM_H
#define NOTIFICATIONITEM_H
#include <QGraphicsObject>
#include "../../enumerations.h"
class GraphMLItem;
class NotificationItem : public QGraphicsObject
{
public:
    NotificationItem(GraphMLItem* parent);

    void setErrorType(ERROR_TYPE errorType);

    void setBackgroundColor(QColor color);
    // QGraphicsItem interface
public:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QColor backgroundColor;
    GraphMLItem* item;
    ERROR_TYPE errorType;
};

#endif // NOTIFICATIONITEM_H
