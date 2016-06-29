#ifndef NODEVIEWITEM_H
#define NODEVIEWITEM_H
#include <QObject>

#include "../Controller/nodeadapter.h"
#include "viewitem.h"

class NodeViewItem: public ViewItem
{
    Q_OBJECT
public:
    NodeViewItem(NodeAdapter* entity);
    ~NodeViewItem();

    int getParentID(int depth = 1);
    QList<int> getTreeIndex();
private:
    NodeAdapter* entity;
};
#endif // VIEWITEM_H
