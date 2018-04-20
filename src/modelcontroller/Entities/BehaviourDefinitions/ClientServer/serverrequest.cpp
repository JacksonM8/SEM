#include "serverrequest.h"

const NODE_KIND node_kind = NODE_KIND::SERVER_REQUEST;
const QString kind_string = "ServerRequest";

MEDEA::ServerRequest::ServerRequest(EntityFactory* factory) : Node(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ServerRequest();});
};

MEDEA::ServerRequest::ServerRequest(): Node(node_kind)
{
    
    setNodeType(NODE_TYPE::IMPLEMENTATION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);

    setDefinitionKind(NODE_KIND::CLIENT_PORT);

    //Allow links from within things like InEventPortImpls back to the
    SetEdgeRuleActive(EdgeRule::MIRROR_PARENT_DEFINITION_HIERARCHY, false);
}


bool MEDEA::ServerRequest::canAdoptChild(Node* child)
{
    auto child_node_kind = child->getNodeKind();

    switch(child_node_kind){
        case NODE_KIND::INPUT_PARAMETER_GROUP:
        case NODE_KIND::RETURN_PARAMETER_GROUP:{
            if(!getChildrenOfKind(child->getNodeKind(), 0).isEmpty()){
                return false;
            }
            break;
        }
        default:
            return false;
    }
    return Node::canAdoptChild(child);
}

bool MEDEA::ServerRequest::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
{
    if(!acceptsEdgeKind(edge_kind)){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(dst->getNodeKind() != NODE_KIND::CLIENT_PORT){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
}
