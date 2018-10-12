#ifndef ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER
#define ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER

#include <thread>
#include <unordered_map>
#include <set>
#include <mutex>
#include <future>
#include "environment.h"
#include "deploymenthandler.h"

#include <zmq/protoreplier/protoreplier.hpp>
#include <proto/controlmessage/controlmessage.pb.h>
#include <proto/environmentcontrol/environmentcontrol.pb.h>
#include <util/execution.hpp>

class DeploymentRegister{
    public:
        DeploymentRegister(Execution& execution,
                            const std::string& ip_addr,
                            const std::string& registration_port,
                            const std::string& qpid_broker_address,
                            const std::string& tao_naming_server_address,
                            int portrange_min = 30000, int portrange_max = 40000);
        ~DeploymentRegister();
    private:
        void Terminate();
        void CleanupTask();
        //Re Node Manager Functions
        std::unique_ptr<NodeManager::NodeManagerRegistrationReply> HandleNodeManagerRegistration(const NodeManager::NodeManagerRegistrationRequest& request);
        
        //Logan Managed Server Functions
        std::unique_ptr<NodeManager::LoganRegistrationReply> HandleLoganRegistration(const NodeManager::LoganRegistrationRequest& request);

        //Controller Functions
        std::unique_ptr<NodeManager::RegisterExperimentReply> HandleRegisterExperiment(const NodeManager::RegisterExperimentRequest& request);
        std::unique_ptr<EnvironmentControl::ShutdownExperimentReply> HandleShutdownExperiment(const EnvironmentControl::ShutdownExperimentRequest& message);
        std::unique_ptr<EnvironmentControl::ListExperimentsReply> HandleListExperiments(const EnvironmentControl::ListExperimentsRequest& message);

        std::unique_ptr<zmq::ProtoReplier> replier_;
        std::unique_ptr<EnvironmentManager::Environment> environment_;

        std::string environment_manager_ip_address_;

        std::mutex handler_mutex_;
        std::vector<std::unique_ptr<DeploymentHandler> > handlers_;

        Execution& execution_;

        std::mutex termination_mutex_;
        bool terminated_ = false;

        std::future<void> cleanup_future_;
        int cleanup_period_ = 1000;
        std::condition_variable cleanup_cv_;
};

#endif //ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER
