#ifndef EXECUTIONMANAGER_H
#define EXECUTIONMANAGER_H


#include <thread>
#include <string>
#include "graphmlparser.h"

#include <map>
#include <vector>

class ZMQMaster;
class ExecutionManager{
    struct HardwareNode{
        std::string id;
        std::string name;
        std::string ip_address;
        int port_count = 6000;
        int node_manager_port = 7001;
        std::vector<std::string> component_ids;
    };

    struct ComponentInstance{
        std::string node_id;

        std::string definition_id;
        std::string implementation_id;
        
        std::string id;
        std::string name;
        std::string type_name;
        
        std::vector<std::string> event_port_ids;
        std::vector<std::string> attribute_ids;
    };


    struct ComponentImpl{
        std::string id;
        std::string name;

        std::string definition_id;

        std::vector<std::string> periodic_eventports_ids;
        std::vector<std::string> instance_ids;
    };

    struct EventPort{
        std::string component_id;
        std::string id;
        std::string name;
        std::string kind;
        std::string middleware;

        std::string message_type;

        std::string frequency;

        int port_number = -1;
        std::string port_address;
        std::string topic_name;
    };

    struct Attribute{
        std::string component_id;
        std::string id;
        std::string name;
    };

    public:
        ExecutionManager(ZMQMaster *zmqmaster, std::string graphml_path);

        std::vector<std::string> GetSlaveEndpoints();
        std::string GetHostNameFromAddress(std::string address);

        void ExecutionLoop();
    private:
        std::string GetAttribute(std::string id, std::string attr_name);
        std::string GetDataValue(std::string id, std::string key_name);
        
        std::string GetDefinitionId(std::string id);
        std::string GetImplId(std::string id);
        

        ExecutionManager::HardwareNode* GetHardwareNode(std::string id);
        ExecutionManager::ComponentInstance* GetComponent(std::string id);
        ExecutionManager::EventPort* GetEventPort(std::string id);
        ExecutionManager::Attribute* GetAttribute(std::string id);

        bool ScrapeDocument();

        
        std::thread* execution_thread_;
        ZMQMaster* zmq_master_;
        GraphmlParser* graphml_parser_;

        std::map<std::string, HardwareNode*> hardware_nodes_;
        std::map<std::string, ComponentInstance*> component_instances_;
        std::map<std::string, EventPort*> event_ports_;
        std::map<std::string, Attribute*> attributes_;
        
        std::map<std::string, ComponentImpl*> component_impls_;

        

        std::vector<std::string> deployment_edge_ids_;
        std::vector<std::string> assembly_edge_ids_;
        std::vector<std::string> definition_edge_ids_;
        

        std::map<std::string, std::string> deployed_instance_map_;

        std::map<std::string, std::string> definition_ids_;
        
};

#endif //EXECUTIONMANAGER_H