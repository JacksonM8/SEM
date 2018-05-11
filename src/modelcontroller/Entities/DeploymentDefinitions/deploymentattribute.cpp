#include "deploymentattribute.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::DEPLOYMENT_ATTRIBUTE;
const QString kind_string = "DeploymentAttribute";

void MEDEA::DeploymentAttribute::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::DeploymentAttribute(broker, is_temp_node);
        });
}


MEDEA::DeploymentAttribute::DeploymentAttribute(::EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
    //Setup State
    setDataProducer(true);
    setDataReceiver(true);
    setMultipleDataProducer(true);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    auto type_data = broker.AttachData(this, "type", QVariant::String, "String", false);
    broker.AttachData(this, "comment", QVariant::String, "", false);
    type_data->addValidValues(TypeKey::GetValidPrimitiveTypes());
}

bool MEDEA::DeploymentAttribute::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    QSet<NODE_KIND> valid_data_kinds = {NODE_KIND::ATTRIBUTE_INSTANCE, NODE_KIND::DEPLOYMENT_ATTRIBUTE};

    switch(edge_kind){
    case EDGE_KIND::DATA:{
        if(!valid_data_kinds.contains(dst->getNodeKind())){
            return false;
        }
        break;
    }
    default:
        break;
    }

    return DataNode::canAcceptEdge(edge_kind, dst);
}