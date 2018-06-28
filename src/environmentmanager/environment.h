#ifndef ENVIRONMENT_MANAGER_ENVIRONMENT
#define ENVIRONMENT_MANAGER_ENVIRONMENT

#include <set>
#include <string>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <proto/controlmessage/controlmessage.pb.h>
#include "uniquequeue.hpp"
#include "experiment.h"
#include "porttracker.h"

namespace EnvironmentManager{
class Environment{

    public:
        enum class DeploymentType{
            EXECUTION_MASTER,
            EXECTUION_SLAVE,
            LOGAN_CLIENT,
            LOGAN_SERVER
        };

        Environment(const std::string& address, int portrange_min = 30000, int portrange_max = 50000);

        std::string AddDeployment(const std::string& model_name, const std::string& ip_address, DeploymentType deployment_type);

        void RemoveExperiment(const std::string& model_name, uint64_t time);
        void RemoveLoganClientServer(const std::string& model_name, const std::string& ip_address);


        NodeManager::EnvironmentMessage* GetLoganDeploymentMessage(const std::string model_name, const std::string& ip_address);

        void FinishConfigure(const std::string& model_name);
        
        void DeclusterExperiment(NodeManager::ControlMessage& message);
        void DeclusterNode(NodeManager::Node& message);
        void AddNodeToExperiment(const std::string& model_name, const NodeManager::Node& node);
        void AddNodeToEnvironment(const NodeManager::Node& node);
        void ConfigureNodes(const std::string& model_name);
        NodeManager::ControlMessage* GetProto(const std::string& model_name);

        bool ExperimentIsDirty(const std::string& model_name);
        NodeManager::ControlMessage* GetExperimentUpdate(const std::string& model_name);

        bool ModelNameExists(const std::string& model_name) const;
        bool NodeDeployedTo(const std::string& model_name, const std::string& ip_address) const;
        
        void SetExperimentMasterIp(const std::string& model_name, const std::string& ip_address);

        std::string GetMasterPublisherAddress(const std::string& model_name);
        std::string GetMasterRegistrationAddress(const std::string& model_name);
        std::string GetNodeModelLoggerPort(const std::string& model_name, const std::string& ip_address);

        std::string GetTaoReplierServerAddress(const std::string& model_name, const NodeManager::Port& port);
        std::string GetTaoServerName(const std::string& model_name, const NodeManager::Port& port);

        std::vector<std::string> GetPublisherAddress(const std::string& model_name, const NodeManager::Port& port);
        std::string GetTopic(const std::string& model_name, const std::string& port_id);
        std::string GetOrbEndpoint(const std::string& experiment_id, const std::string& port_id);

        std::vector<std::string> CheckTopic(const std::string& model_name, const std::string& topic);

        void ExperimentLive(const std::string& deployment_id, uint64_t time_called);
        void ExperimentTimeout(const std::string& deployment_id, uint64_t time_called);

        NodeManager::Node GetDeploymentLocation(const std::string& model_name, const std::string& port_id);

        std::string GetPort(const std::string& ip_address);
        void FreePort(const std::string& ip_address, const std::string& port_number);

        
        std::string GetManagerPort();
        void FreeManagerPort(const std::string& port);

        void AddExternalPorts(const std::string& model_name, const NodeManager::ControlMessage& control_message);


        bool HasPublicEventPort(const std::string& port_id);
        std::string GetPublicEventPortEndpoint(const std::string& port_id);
        void AddPublicEventPort(const std::string& model_name, const std::string& port_id, const std::string& address_string);

        bool HasPendingPublicEventPort(const std::string& port_id);
        std::set<std::string> GetDependentExperiments(const std::string& port_id);
        void AddPendingPublicEventPort(const std::string& model_name, const std::string& port_id);

        void RemoveDependentExternalExperiment(const std::string& model_name, const std::string& port_id);

        std::string GetAmqpBrokerAddress();

        uint64_t GetClock();
        uint64_t SetClock(uint64_t clock);
        uint64_t Tick();
    private:
        uint64_t clock_;
        std::mutex clock_mutex_;

        //Port range
        int PORT_RANGE_MIN;
        int PORT_RANGE_MAX;

        int MANAGER_PORT_RANGE_MIN;
        int MANAGER_PORT_RANGE_MAX;

        std::string address_;

        //Returns management port for logan client
        std::string AddLoganClientServer();

        bool AddExperiment(const std::string& model_name);
        //model_name -> experiment data structure
        std::unordered_map<std::string, std::unique_ptr<EnvironmentManager::Experiment> > experiment_map_;

        //node_name -> node data structure
        std::unordered_map<std::string, std::unique_ptr<EnvironmentManager::PortTracker> > node_map_;

        //node_name -> node_ip map
        std::unordered_map<std::string, std::string> node_ip_map_;

        //node_ip -> node_name map
        std::unordered_map<std::string, std::string> node_name_map_;

        //event port guid -> event port data structure
        //event port guid takes form "experiment_id.{component_assembly_label}*n.component_instance_label.event_port_label"
        std::unordered_map<std::string, std::unique_ptr<EnvironmentManager::EventPort> > public_event_port_map_;

        //event port guid -> set of experiment ids
        //keeps track of experiments waiting for port of this guid to become live.
        std::unordered_map<std::string, std::set<std::string> > pending_port_map_;
        std::unordered_map<std::string, std::set<std::string> > dependent_experiment_map_;

        std::mutex port_mutex_;
        //initially allocated unique_queue of port nums from PORT_RANGE_MIN to PORT_RANGE_MAX so we can copy into each node struct
        unique_queue<int> available_ports_;

        //ports available on the environment manager, uses same port range as nodes.
        unique_queue<int> available_node_manager_ports_;
};
};

#endif //ENVIRONMENT_MANAGER_ENVIRONMENT
