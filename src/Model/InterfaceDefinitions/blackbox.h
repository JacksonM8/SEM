#ifndef BLACKBOX_H
#define BLACKBOX_H
#include "../node.h"

class EntityFactory;
class BlackBox : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	BlackBox(EntityFactory* factory);
	BlackBox();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // BLACKBOX_H
