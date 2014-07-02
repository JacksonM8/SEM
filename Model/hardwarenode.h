#ifndef HARDWARENODE_H
#define HARDWARENODE_H
#include "node.h"

class HardwareNode : public Node
{
public:
    HardwareNode(QString name);
    ~HardwareNode();

public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel=0);
    QString toString();
};

#endif // HARDWARENODE_H
