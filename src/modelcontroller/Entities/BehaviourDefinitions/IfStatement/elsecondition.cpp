#include "elsecondition.h"
#include "../containernode.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::ELSE_CONDITION;
const QString kind_string = "Else Condition";

void MEDEA::ElseCondition::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ElseCondition(broker, is_temp_node);
        });
}

MEDEA::ElseCondition::ElseCondition(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //SetupState
    setLabelFunctional(false);
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "label", QVariant::String, "else", true);
    broker.AttachData(this, "row", QVariant::Int, 0, true);
    broker.AttachData(this, "column", QVariant::Int, 1, true);
}