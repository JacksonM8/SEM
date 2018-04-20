#include "classinstance.h"

const NODE_KIND node_kind = NODE_KIND::CLASS_INSTANCE;
const QString kind_string = "ClassInstance";

MEDEA::ClassInstance::ClassInstance(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ClassInstance();});

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "icon_prefix", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "icon", QVariant::String, true);
};

MEDEA::ClassInstance::ClassInstance(): Node(node_kind)
{
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    setNodeType(NODE_TYPE::DEFINITION);

    setDefinitionKind(NODE_KIND::CLASS);
    setInstanceKind(NODE_KIND::CLASS_INSTANCE);

    SetEdgeRuleActive(Node::EdgeRule::MIRROR_PARENT_DEFINITION_HIERARCHY, false);
}

bool MEDEA::ClassInstance::ClassInstance::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
        case NODE_KIND::ATTRIBUTE_INSTANCE:
        case NODE_KIND::FUNCTION:
        case NODE_KIND::CLASS_INSTANCE:
            break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool MEDEA::ClassInstance::ClassInstance::canAcceptEdge(EDGE_KIND edge_kind, Node* dst)
{
    if(!requiresEdgeKind(edge_kind)){
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
