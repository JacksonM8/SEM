#ifndef COMPACTNODEITEM_H
#define COMPACTNODEITEM_H

#include "basicnodeitem.h"

class CompactNodeItem : public BasicNodeItem
{
    Q_OBJECT
public:
    CompactNodeItem(NodeViewItem* viewItem, NodeItem* parentItem);
public:
    QRectF getElementRect(EntityRect rect) const;
private:
    void secondaryIconHover(bool handle, const QPointF& pos);
    QRectF innerHeaderRect() const;
    QRectF topRect() const;
    QRectF bottomRect() const;

    QRectF textRect_Primary() const;
    QRectF textRect_Secondary() const;

    QRectF iconRect_Primary() const;
    QRectF iconRect_Secondary() const;
    QRectF iconRect_Tertiary() const;
    QRectF iconRect_Union() const;
    
};

#endif // COMPACTNODEITEM_H
