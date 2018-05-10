#ifndef MEDEA_INPUTPARAMETERGROUP_INSTANCE_H
#define MEDEA_INPUTPARAMETERGROUP_INSTANCE_H
#include "../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class InputParameterGroupInstance : public Node{

   
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        InputParameterGroupInstance(EntityFactoryBroker& factory, bool is_temp_node);
    };
};

#endif // MEDEA_INPUTPARAMETERGROUP_INSTANCE_H
