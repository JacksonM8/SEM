#include "periodicevent.h"



PeriodicEvent::PeriodicEvent(EntityFactory* factory) : ContainerNode(factory, NODE_KIND::PERIODICEVENT, "PeriodicEvent"){
	auto node_kind = NODE_KIND::PERIODICEVENT;
	QString kind_string = "PeriodicEvent";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new PeriodicEvent();});

    RegisterDefaultData(factory, node_kind, "frequency", QVariant::Double, false, 1);
};

PeriodicEvent::PeriodicEvent():ContainerNode(NODE_KIND::PERIODICEVENT){
    //setNodeType(NODE_TYPE::DEFINITION);
    //setNodeType(NODE_TYPE::INSTANCE);
    //setAcceptsEdgeKind(EDGE_KIND::DEFINITION);

    //setDefinitionKind(NODE_KIND::PERIODICEVENT);
    //setInstanceKind(NODE_KIND::PERIODICEVENT);
}
#include <QDebug>
bool PeriodicEvent::canAdoptChild(Node* child)
{
    qCritical() << "TRYING TO ADOPT: " << child->toString() << " = " << (ContainerNode::canAdoptChild(child) ? "YES" : "NO");
    return ContainerNode::canAdoptChild(child);
}

bool PeriodicEvent::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return ContainerNode::canAcceptEdge(edgeKind, dst);
}
