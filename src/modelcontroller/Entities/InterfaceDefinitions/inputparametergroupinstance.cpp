#include "inputparametergroupinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE;
const QString kind_string = "InputParameterGroupInstance";


void MEDEA::InputParameterGroupInstance::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::InputParameterGroupInstance(broker, is_temp_node);
    });
}

MEDEA::InputParameterGroupInstance::InputParameterGroupInstance(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setLabelFunctional(false);
    addInstancesDefinitionKind(NODE_KIND::INPUT_PARAMETER_GROUP);
    setChainableDefinition();

    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::MEMBER_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::VECTOR_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::VOID_TYPE);



    //Setup Data
    broker.AttachData(this, "type", QVariant::String, "", true);
    broker.AttachData(this, "index", QVariant::Int, -1, false);
}