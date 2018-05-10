#include "serverinterface.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::SERVER_INTERFACE;
const QString kind_string = "ServerInterface";

void MEDEA::ServerInterface::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
	broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ServerInterface(broker, is_temp_node);
        });
};


MEDEA::ServerInterface::ServerInterface(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addInstanceKind(NODE_KIND::SERVER_PORT);
    addInstanceKind(NODE_KIND::CLIENT_PORT);
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP);
    
    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Children
    auto input_params = broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER_GROUP);
    auto return_params = broker.ConstructChildNode(*this, NODE_KIND::RETURN_PARAMETER_GROUP);

    broker.AttachData(input_params, "label", QVariant::String, "Request", false);
    broker.AttachData(return_params, "label", QVariant::String, "Reply", false);
}


bool MEDEA::ServerInterface::canAdoptChild(Node* child)
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
        return false;
    };
    return Node::canAdoptChild(child);
}