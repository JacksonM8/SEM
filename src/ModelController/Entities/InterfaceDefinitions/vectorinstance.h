#ifndef VECTORINSTANCE_H
#define VECTORINSTANCE_H
#include "datanode.h"

class EntityFactory;
class VectorInstance : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	VectorInstance(EntityFactory* factory);
	VectorInstance();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};


#endif // AGGREGATE_H

