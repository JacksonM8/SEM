#include "interfacedefinitions.h"

InterfaceDefinitions::InterfaceDefinitions(): Node(NK_INTERFACE_DEFINITIONS)
{
    setNodeType(NT_ASPECT);
}

VIEW_ASPECT InterfaceDefinitions::getViewAspect()
{
    return VA_INTERFACES;
}

bool InterfaceDefinitions::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NK_IDL:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool InterfaceDefinitions::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
