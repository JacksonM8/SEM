#include "componentimpl.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::COMPONENT_IMPL;
const QString kind_string = "Component Impl";

void ComponentImpl::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new ComponentImpl(broker, is_temp_node);
        });
}

ComponentImpl::ComponentImpl(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addImplsDefinitionKind(NODE_KIND::COMPONENT);
    setNodeType(NODE_TYPE::TOP_BEHAVIOUR_CONTAINER);

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE_IMPL);
    setAcceptsNodeKind(NODE_KIND::INEVENTPORT_IMPL);
    setAcceptsNodeKind(NODE_KIND::PERIODICEVENT);
    setAcceptsNodeKind(NODE_KIND::VARIABLE);
    setAcceptsNodeKind(NODE_KIND::HEADER);
    setAcceptsNodeKind(NODE_KIND::FUNCTION);
    setAcceptsNodeKind(NODE_KIND::CLASS_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::SERVER_PORT_IMPL);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "type", QVariant::String, "", true);

}
QSet<Node*> ComponentImpl::getDependants() const{
    auto required_nodes = Node::getDependants();
    auto definition = getDefinition(true);
    if(definition){
        for(auto d : definition->getInstances()){
            required_nodes.insert(d);
        }
    }
    return required_nodes;
}
