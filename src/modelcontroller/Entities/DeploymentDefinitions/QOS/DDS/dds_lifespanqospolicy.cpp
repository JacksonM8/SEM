#include "dds_lifespanqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_LIFESPAN;
const static QString kind_string = "DDS_LifespanQosPolicy";

void DDS_LifespanQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_LifespanQosPolicy(broker, is_temp_node);
    });
}

DDS_LifespanQosPolicy::DDS_LifespanQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "label", QVariant::String, "lifespan", true);
    broker.AttachData(this, "qos_dds_duration_sec", QVariant::String, "DURATION_INFINITE_SEC", false);
    broker.AttachData(this, "qos_dds_duration_nanosec", QVariant::String, "DURATION_INFINITE_NSEC", false);
}