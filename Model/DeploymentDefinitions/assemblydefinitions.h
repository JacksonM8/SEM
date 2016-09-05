#ifndef ASSEMBLYDEFINITIONS_H
#define ASSEMBLYDEFINITIONS_H
#include "../node.h"

class AssemblyDefinitions: public Node
{
    Q_OBJECT
public:
    AssemblyDefinitions();
    VIEW_ASPECT getViewAspect() const;
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // AssemblyDefinitions_H
