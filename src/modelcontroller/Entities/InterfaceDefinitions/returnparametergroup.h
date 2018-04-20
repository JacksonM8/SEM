#ifndef MEDEA_RETURNPARAMETERGROUP_H
#define MEDEA_RETURNPARAMETERGROUP_H
#include "../node.h"

class EntityFactory;
namespace MEDEA{
    class ReturnParameterGroup : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ReturnParameterGroup(EntityFactory* factory);
        ReturnParameterGroup();
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_RETURNPARAMETERGROUP_H
