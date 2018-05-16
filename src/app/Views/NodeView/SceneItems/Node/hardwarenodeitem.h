#ifndef HARDWARE_NODEITEM_H
#define HARDWARE_NODEITEM_H

#include "stacknodeitem.h"

class HardwareNodeItem: public StackNodeItem
{
    Q_OBJECT
public:
    HardwareNodeItem(NodeViewItem *viewItem, NodeItem *parentItem);
protected:
    void dataChanged(QString keyName, QVariant data);
private:
    QPair<QString, QString> online_icon;
    QPair<QString, QString> offline_icon;
};

#endif // HARDWARE_NODEITEM_H
