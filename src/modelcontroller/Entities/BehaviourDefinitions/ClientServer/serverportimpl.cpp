#include "serverportimpl.h"

const NODE_KIND node_kind = NODE_KIND::SERVER_PORT_IMPL;
const QString kind_string = "ServerPortImpl";

MEDEA::ServerPortImpl::ServerPortImpl(EntityFactory* factory) : Node(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ServerPortImpl();});
};

MEDEA::ServerPortImpl::ServerPortImpl(): Node(node_kind)
{
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);

    addImplsDefinitionKind(NODE_KIND::SERVER_PORT);
}


bool MEDEA::ServerPortImpl::canAdoptChild(Node* child)
{
    auto child_node_kind = child->getNodeKind();

    switch(child_node_kind){
        case NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE:
        case NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE:{
            if(!getChildrenOfKind(child->getNodeKind(), 0).isEmpty()){
                return false;
            }
            break;
        }
        default:
            if(!ContainerNode::canAdoptChild(child)){
                return false;
            }
    }
    return Node::canAdoptChild(child);
}

bool MEDEA::ServerPortImpl::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(dst->getNodeKind() != NODE_KIND::SERVER_PORT){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
}
