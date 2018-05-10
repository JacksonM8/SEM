#include "outeventportdelegate.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::OUTEVENTPORT_DELEGATE;
const static QString kind_string = "OutEventPortDelegate";

void OutEventPortDelegate::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new OutEventPortDelegate(broker, is_temp_node);
    });
}

OutEventPortDelegate::OutEventPortDelegate(EntityFactoryBroker& broker, bool is_temp) : EventPortAssembly(broker, node_kind, is_temp){

}