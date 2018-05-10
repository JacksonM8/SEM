#include "dds_durabilityqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_DURABILITY;
const static QString kind_string = "DDS_DurabilityQosPolicy";

void DDS_DurabilityQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_DurabilityQosPolicy(broker, is_temp_node);
    });
}

DDS_DurabilityQosPolicy::DDS_DurabilityQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    QList<QVariant> values;
    values << "VOLATILE_DURABILITY_QOS";
    values << "TRANSIENT_LOCAL_DURABILITY_QOS";
    values << "TRANSIENT_DURABILITY_QOS";
    values << "PERSISTENT_DURABILITY_QOS";
    broker.AttachData(this, "label", QVariant::String, "durability", true);
    auto dds_kind_data = broker.AttachData(this, "qos_dds_kind", QVariant::String, values.first(), true);
    dds_kind_data->addValidValues(values);
}