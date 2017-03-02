#ifndef DEPLOYMENTMANAGER_H
#define DEPLOYMENTMANAGER_H

#include <functional>
#include <string>
#include <mutex>

#include "nodecontainer.h"

namespace zmq{class ProtoReceiver;};
namespace google{namespace protobuf{class MessageLite;}};
namespace NodeManager{class ControlMessage;};

class DeploymentManager{
    public:
        DeploymentManager(std::string library_path);
        ~DeploymentManager();

        bool SetupControlMessageReceiver(std::string pub_endpoint, std::string host_name);
        bool SetupModelLogger(std::string pub_endpoint, std::string host_name);

        void GotControlMessage(google::protobuf::MessageLite* ml);
        NodeContainer* get_deployment();
    private:
        void ProcessControlMessage(NodeManager::ControlMessage* message);
        zmq::ProtoReceiver* subscriber_ = 0;


        std::string library_path_;
        std::mutex mutex_;
        

        NodeContainer* deployment_ = 0;
};

#endif //DEPLOYMENTMANAGER_H