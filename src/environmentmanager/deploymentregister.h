#ifndef ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER
#define ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER

#include <thread>
#include <unordered_map>
#include <set>
#include <zmq.hpp>
#include <mutex>
#include <future>
#include "environment.h"
#include "deploymenthandler.h"

#include <proto/controlmessage/controlmessage.pb.h>


class DeploymentRegister{
    public:
        DeploymentRegister(const std::string& ip_addr, const std::string& registration_port, 
                            int portrange_min = 30000, int portrange_max = 40000);

        void Start();

    private:
        //Threads and thread wrappers
        void RegistrationLoop() noexcept;
        void HandleDeployment(std::promise<std::string> assigned_port, const std::string& request_info);

        //Request handlers
        void RequestHandler(NodeManager::EnvironmentMessage& message);
        void HandleAddDeployment(NodeManager::EnvironmentMessage& message);
        void HandleNodeQuery(NodeManager::EnvironmentMessage& message);
        void HandleAddLoganClient(NodeManager::EnvironmentMessage& message);
        void HandleAddLoganServer(NodeManager::EnvironmentMessage& message);

        //Helpers
        std::string TCPify(const std::string& ip_address, const std::string& port) const;
        std::string TCPify(const std::string& ip_address, int port) const;

        void ZMQSendReply(zmq::socket_t& socket, const std::string& part_two);
        std::pair<uint64_t, std::string> ZMQReceiveRequest(zmq::socket_t& socket);

        //Members
        std::unique_ptr<zmq::context_t> context_;

        std::unique_ptr<Environment> environment_;

        std::string ip_addr_;
        std::string registration_port_;

        std::thread registration_loop_;
        std::vector<std::unique_ptr<DeploymentHandler> > deployments_;
        std::vector<std::unique_ptr<DeploymentHandler> > logan_clients_;
        std::vector<std::thread*> handler_threads_;

};

#endif //ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER
