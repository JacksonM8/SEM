#ifndef NODEVIEWMINIMAP_H
#define NODEVIEWMINIMAP_H

#include <QGraphicsView>

class NodeViewMinimap : public QGraphicsView
{
    Q_OBJECT
public:
    explicit NodeViewMinimap(QObject *parent = 0);


public slots:
    void viewPortRectangleChanged(QRectF viewport);

private:
    QRectF viewport;

    // QGraphicsView interface
protected:
    void drawForeground(QPainter *painter, const QRectF &rect);
};

#endif // NODEVIEWMINIMAP_H
