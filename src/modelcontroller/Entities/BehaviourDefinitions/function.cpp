#include "function.h"
#include "../data.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::FUNCTION;
const QString kind_string = "Function";

void MEDEA::Function::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::Function(broker, is_temp_node);
        });
}

MEDEA::Function::Function(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    setChainableDefinition();
    addInstanceKind(NODE_KIND::FUNCTION_CALL);

    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP);

    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "class", QVariant::String, "", true);
    broker.AttachData(this, "type", QVariant::String, "", true);
    broker.AttachData(this, "icon_prefix", QVariant::String, "", false);
    broker.AttachData(this, "icon", QVariant::String, "", false);
}


bool MEDEA::Function::Function::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
        case NODE_KIND::INPUT_PARAMETER_GROUP:
        case NODE_KIND::RETURN_PARAMETER_GROUP:{
            if(getChildrenOfKindCount(child_kind) > 0){
                return false;
            }
            break;
        }
        default:
            break;
    }
    return Node::canAdoptChild(child);
}

void MEDEA::Function::parentSet(Node* parent){

    auto src_data = parent->getData("label");
    auto dst_data = getData("class");
    if(src_data && dst_data){
        src_data->linkData(dst_data, true);
    }

    

    auto parent_node_kind = parent->getNodeKind();
    if(parent_node_kind != NODE_KIND::COMPONENT_IMPL){
        setAcceptsNodeKind(NODE_KIND::INEVENTPORT_IMPL, false);
        setAcceptsNodeKind(NODE_KIND::OUTEVENTPORT_IMPL, false);
        setAcceptsNodeKind(NODE_KIND::SERVER_REQUEST, false);
    }

    Node::parentSet(parent);
}
