#include "outeventportimpl.h"

#include "containernode.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::OUTEVENTPORT_IMPL;
const QString kind_string = "OutEventPortImpl";

void OutEventPortImpl::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new OutEventPortImpl(broker, is_temp_node);
        });
}

OutEventPortImpl::OutEventPortImpl(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addImplsDefinitionKind(NODE_KIND::OUTEVENTPORT);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);

    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    SetEdgeRuleActive(Node::EdgeRule::ALWAYS_CHECK_VALID_DEFINITIONS);
    SetEdgeRuleActive(Node::EdgeRule::ALLOW_MULTIPLE_IMPLEMENTATIONS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, "type", QVariant::String, "", true);
}

bool OutEventPortImpl::canAdoptChild(Node *child)
{
    auto child_kind = child->getNodeKind();
    
    switch(child_kind){
        case NODE_KIND::AGGREGATE_INSTANCE:{
            if(getChildrenOfKindCount(child_kind) > 0){
                return false;
            }
            break;
        default:
            break;
        }
    }
    return Node::canAdoptChild(child);
}


QSet<Node*> OutEventPortImpl::getParentNodesForValidDefinition(){
    QSet<Node*> parents;
    //Need to look at The Component's defintion
    auto component = getTopBehaviourContainer();
    if(component){
        auto component_definition = component->getDefinition(true);
        if(component_definition){
            parents << component_definition;
        }
    }
    return parents;
}

Node* OutEventPortImpl::getTopBehaviourContainer(){
    if(!top_behaviour_calculated){
        top_behaviour_container = ContainerNode::getTopBehaviourContainer(this);
        top_behaviour_calculated = true;
    }
    return top_behaviour_container;
}