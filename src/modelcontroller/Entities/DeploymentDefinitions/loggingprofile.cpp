#include "loggingprofile.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::LOGGINGPROFILE;
const static QString kind_string = "LoggingProfile";

void LoggingProfile::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new LoggingProfile(broker, is_temp_node);
    });
}

LoggingProfile::LoggingProfile(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::LOGGING);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::SOURCE);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE);



    
    broker.AttachData(this, "processes_to_log", QVariant::String, "", false);
    broker.AttachData(this, "frequency", QVariant::Double, 1, false);
    auto data_mode = broker.AttachData(this, "mode", QVariant::String, "CACHED", false);
    data_mode->addValidValues({"CACHED", "OFF", "LIVE"});
}

bool LoggingProfile::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }
    switch(edge_kind){
    case EDGE_KIND::ASSEMBLY:{
        //Can't connect to something that isn't an EventPortAssembly
        if(dst->getNodeKind() != NODE_KIND::LOGGINGSERVER){
            return false;
        }
        break;
    }
    default:
        break;
    }

    return Node::canAcceptEdge(edge_kind, dst);
}
