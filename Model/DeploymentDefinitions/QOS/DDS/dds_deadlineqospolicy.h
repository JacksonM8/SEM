#ifndef DDS_DEADLINEQOSPOLICY_H
#define DDS_DEADLINEQOSPOLICY_H
#include "../../../node.h"

class DDS_DeadlineQosPolicy: public Node
{
    Q_OBJECT
public:
    DDS_DeadlineQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};
#endif // DDS_DEADLINEQOSPOLICY_H


