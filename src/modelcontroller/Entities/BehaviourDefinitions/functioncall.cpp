#include "functioncall.h"
#include "containernode.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::FUNCTION_CALL;
const QString kind_string = "FunctionCall";

void FunctionCall::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new FunctionCall(broker, is_temp_node);
        });
}

FunctionCall::FunctionCall(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::FUNCTION);
    SetEdgeRuleActive(Node::EdgeRule::ALWAYS_CHECK_VALID_DEFINITIONS, true);
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE);
    addInstancesDefinitionKind(NODE_KIND::FUNCTION);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "class", QVariant::String, "", true);
    broker.AttachData(this, "label", QVariant::String, "", true);
    broker.AttachData(this, "icon_prefix", QVariant::String, "", true);
    broker.AttachData(this, "icon", QVariant::String, "", true);
}

bool FunctionCall::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
        case NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE:
        case NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE:{
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

QSet<Node*> FunctionCall::getListOfValidAncestorsForChildrenDefinitions(){
    QSet<Node*> valid_ancestors;
    
    //We can create definition edges back to the recursive Definition only, because of the fact the instances are sparse
    auto definition = getDefinition(true);
    if(definition){
        valid_ancestors << definition;
    }

    return valid_ancestors;
}

QSet<Node*> FunctionCall::getParentNodesForValidDefinition(){
    QSet<Node*> parents;
    //Need to look at the CLASS_INSTANCE children contained with in the ComponentImpl/Class
    auto component = getTopBehaviourContainer();
    if(component){
        parents << component;
        for(auto node : component->getChildrenOfKind(NODE_KIND::CLASS_INSTANCE, 0)){
            parents << node;
        }
    }
    return parents;
}

Node* FunctionCall::getTopBehaviourContainer(){
    if(!top_behaviour_calculated){
        top_behaviour_container = ContainerNode::getTopBehaviourContainer(this);
        top_behaviour_calculated = true;
    }
    return top_behaviour_container;
}