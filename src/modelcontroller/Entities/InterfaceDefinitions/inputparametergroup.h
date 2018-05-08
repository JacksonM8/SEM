#ifndef MEDEA_INPUTPARAMETERGROUP_H
#define MEDEA_INPUTPARAMETERGROUP_H
#include "../node.h"

class EntityFactory;
namespace MEDEA{
    class InputParameterGroup : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        InputParameterGroup(EntityFactory& factory, bool is_temp_node);
        void parentSet(Node* parent);
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_INPUTPARAMETERGROUP_H
