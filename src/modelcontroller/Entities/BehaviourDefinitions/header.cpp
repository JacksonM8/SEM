#include "header.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"


const NODE_KIND node_kind = NODE_KIND::HEADER;
const QString kind_string = "Header";

void Header::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new Header(broker, is_temp_node);
        });
}

Header::Header(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, "code", QVariant::String, "", false);
    auto header_location = broker.AttachData(this, "header_location", QVariant::String, "CPP", false);
    header_location->addValidValues({"Class Declaration", "Header", "CPP"});
}