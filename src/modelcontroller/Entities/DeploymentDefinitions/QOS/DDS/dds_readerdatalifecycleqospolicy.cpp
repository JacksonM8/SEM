#include "dds_readerdatalifecycleqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_READERDATALIFECYCLE;
const static QString kind_string = "DDS_ReaderDataLifecycleQosPolicy";

void DDS_ReaderDataLifecycleQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_ReaderDataLifecycleQosPolicy(broker, is_temp_node);
    });
}

DDS_ReaderDataLifecycleQosPolicy::DDS_ReaderDataLifecycleQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);




    //Setup Data
    broker.AttachData(this, "label", QVariant::String, "reader_data_lifecycle", true);
    broker.AttachData(this, "qos_dds_autopurge_nowriter_samples_delay_sec", QVariant::String, "DURATION_INFINITE_SEC", false);
    broker.AttachData(this, "qos_dds_autopurge_nowriter_samples_delay_nanosec", QVariant::String, "DURATION_INFINITE_NSEC", false);
    broker.AttachData(this, "qos_dds_autopurge_disposed_samples_delay_sec", QVariant::String, "DURATION_INFINITE_SEC", false);
    broker.AttachData(this, "qos_dds_autopurge_disposed_samples_delay_nanosec", QVariant::String, "DURATION_INFINITE_NSEC", false);
}