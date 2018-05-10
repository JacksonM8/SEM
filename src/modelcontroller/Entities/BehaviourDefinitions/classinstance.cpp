#include "classinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::CLASS_INSTANCE;
const QString kind_string = "ClassInstance";

void MEDEA::ClassInstance::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ClassInstance(broker, is_temp_node);
        });
}

MEDEA::ClassInstance::ClassInstance(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::CLASS);
    setChainableDefinition();
    
    SetEdgeRuleActive(Node::EdgeRule::ALLOW_EXTERNAL_DEFINITIONS, true);

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::FUNCTION);
    setAcceptsNodeKind(NODE_KIND::CLASS_INSTANCE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "type", QVariant::String, "", true);
    broker.AttachData(this, "icon_prefix", QVariant::String, "", true);
    broker.AttachData(this, "icon", QVariant::String, "", true);
};

bool MEDEA::ClassInstance::ClassInstance::canAcceptEdge(EDGE_KIND edge_kind, Node* dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    auto parent_node = getParentNode();
    auto parent_node_kind = parent_node ? parent_node->getNodeKind() : NODE_KIND::NONE;

    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        switch(dst->getNodeKind()){
            case NODE_KIND::CLASS:
            case NODE_KIND::CLASS_INSTANCE:{
                if(parent_node_kind == NODE_KIND::COMPONENT_INSTANCE){
                    auto parent_node_def = parent_node->getDefinition(true);
                    bool in_ancestor = false;
                    for(auto impl : parent_node_def->getImplementations()){
                        if(impl->isAncestorOf(dst)){
                            in_ancestor = true;
                            break;
                        }
                    }
                    
                    if(!in_ancestor){
                        return false;
                    }
                }else{
                    switch (dst->getParentNode()->getNodeKind()) {
                        // Should only be able to have top level parents
                        case NODE_KIND::BEHAVIOUR_DEFINITIONS:
                        case NODE_KIND::WORKER_DEFINITIONS:
                            break;
                        default:
                            return false;
                    }
                }
                break;
            }
            default:
                return false;
        }
        break;
    }
    default:
        break;
    }

    return Node::canAcceptEdge(edge_kind, dst);
}
