#ifndef DDS_PRESENTATIONQOSPOLICY_H
#define DDS_PRESENTATIONQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_PresentationQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_PresentationQosPolicy(EntityFactory* factory);
	DDS_PresentationQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_PRESENTATIONQOSPOLICY_H


