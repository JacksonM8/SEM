#ifndef DEPLOYMENTMANAGER_H
#define DEPLOYMENTMANAGER_H

#include <functional>
#include <string>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include <unordered_map>
#include <memory>
#include <future>

#include <core/modellogger.h>
#include "deploymentcontainer.h"
#include "controlmessage/controlmessage.pb.h"

namespace zmq{class ProtoReceiver;};
class Execution;

class DeploymentManager{
    public:
        DeploymentManager(bool on_master_node,
                            const std::string& library_path,
                            Execution* execution,
                            const std::string& experiment_id,
                            const std::string& ip_address,
                            const std::string& environment_manager_endpoint = "");
        ~DeploymentManager();

        std::string GetSlaveEndpoint();
        NodeManager::StartupResponse HandleStartup(const NodeManager::Startup startup);

        bool TeardownModelLogger();
    private:
        std::shared_ptr<DeploymentContainer> GetDeploymentContainer(const std::string& node_name);
        void GotControlMessage(const NodeManager::ControlMessage& control_message);
        bool ConfigureDeploymentContainers(const NodeManager::ControlMessage& control_message);
        void InteruptQueueThread();
        void ProcessControlQueue();

        std::string QueryEnvironmentManager();

        bool on_master_node_ = false;

        zmq::ProtoReceiver* subscriber_ = 0;

        std::string library_path_;
        std::string experiment_id_;
        std::string ip_address_;
        std::string environment_manager_endpoint_;
        
        Execution* execution_;

        std::unordered_map<std::string, std::shared_ptr<DeploymentContainer> > deployment_containers_;

        std::queue<NodeManager::ControlMessage> control_message_queue_;
        std::mutex mutex_;
        bool terminate_ = false;
        std::mutex notify_mutex_;
        
        std::thread* control_queue_thread_ = 0;
        std::condition_variable notify_lock_condition_;
};

#endif //DEPLOYMENTMANAGER_H
