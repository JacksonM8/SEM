#include "componentassembly.h"

#include "../../edgekinds.h"

ComponentAssembly::ComponentAssembly(EntityFactory* factory) : Node(factory, NODE_KIND::COMPONENT_ASSEMBLY, "ComponentAssembly"){
	auto node_kind = NODE_KIND::COMPONENT_ASSEMBLY;
	QString kind_string = "ComponentAssembly";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ComponentAssembly();});

    RegisterDefaultData(factory, node_kind, "replicate_count", QVariant::Int, false, 1);
    RegisterDefaultData(factory, node_kind, "comment", QVariant::String);
};

ComponentAssembly::ComponentAssembly():Node(NODE_KIND::COMPONENT_ASSEMBLY)
{
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT);
    setAcceptsEdgeKind(EDGE_KIND::QOS);
}

bool ComponentAssembly::canAdoptChild(Node *child)
{
    //Can Only adopt EventPort Instances
    switch(child->getNodeKind()){
    case NODE_KIND::BLACKBOX_INSTANCE:
    case NODE_KIND::COMPONENT_ASSEMBLY:
    case NODE_KIND::COMPONENT_INSTANCE:
    case NODE_KIND::INEVENTPORT_DELEGATE:
    case NODE_KIND::OUTEVENTPORT_DELEGATE:
        break;
    default:
        return false;
    }

    return Node::canAdoptChild(child);
}

bool ComponentAssembly::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    //Can only have one Deployed Node
    if(edgeKind == EDGE_KIND::DEPLOYMENT){
        if(!getEdges(0, edgeKind).empty()){
            return false;
        }
    }

    return Node::canAcceptEdge(edgeKind, dst);
}
