#include "inputparametergroupinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE;
const QString kind_string = "Input Parameter Group Instance";


void MEDEA::InputParameterGroupInstance::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::InputParameterGroupInstance(broker, is_temp_node);
    });
}

MEDEA::InputParameterGroupInstance::InputParameterGroupInstance(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setLabelFunctional(false);
    addInstancesDefinitionKind(NODE_KIND::INPUT_PARAMETER_GROUP);
    setChainableDefinition();

    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::MEMBER_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::VECTOR_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::VOID_TYPE);
    setAcceptsNodeKind(NODE_KIND::VARIADIC_PARAMETER);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "type", QVariant::String, "", true);
    
    broker.ProtectData(this, "index", true);
    broker.AttachData(this, "row", QVariant::Int, 0, true);
    broker.AttachData(this, "column", QVariant::Int, -1, true);
}

bool MEDEA::InputParameterGroupInstance::canAdoptChild(Node* child){
    auto parent_node = getParentNodeKind();

    switch(child->getNodeKind()){
        case NODE_KIND::VARIADIC_PARAMETER:{
            if(!canAdoptVariadicParameters()){
                return false;
            }
            break;
        }
        default:
            break;
    }
    return Node::canAdoptChild(child);
}

QSet<NODE_KIND> MEDEA::InputParameterGroupInstance::getUserConstructableNodeKinds() const{
    auto node_kinds = Node::getUserConstructableNodeKinds();
    if(canAdoptVariadicParameters()){
        node_kinds += NODE_KIND::VARIADIC_PARAMETER;
    }
    return node_kinds;
}

bool MEDEA::InputParameterGroupInstance::canAdoptVariadicParameters() const{
    auto parent_node = getParentNode();

    if(parent_node && parent_node->getNodeKind() == NODE_KIND::FUNCTION_CALL){
        if(parent_node->getDataValue("is_variadic").toBool()){
            return true;
        }
    }
    return false;
}