
#ifndef MEDEA_EXTERNALTYPE_H
#define MEDEA_EXTERNALTYPE_H

#include "../node.h"
class EntityFactory;
namespace MEDEA{
    class ExternalType: public Node
    {
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ExternalType(EntityFactory* factory);
        ExternalType();
    public:
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);

    private:
        Node* getTopBehaviourContainer();
        bool top_behaviour_calculated = false;
        Node* top_behaviour_container = 0;
    };

}



#endif // MEDEA_EXTERNALTYPE_H
