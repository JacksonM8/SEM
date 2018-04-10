#ifndef NODEMANAGER_ENVIRONMENTREQUESTER_H
#define NODEMANAGER_ENVIRONMENTREQUESTER_H

#include <iostream>
#include <mutex>
#include <thread>
#include <queue>
#include <future>
#include <zmq.hpp>

#include "controlmessage.pb.h"


class EnvironmentRequester{


    public:
        EnvironmentRequester(const std::string& manager_address,
                                const std::string& deployment_id);
        void Init();
        void Init(const std::string& manager_endpoint);
        void Start();
        void End();
        NodeManager::ControlMessage AddDeployment(NodeManager::ControlMessage& control_message);
        void RemoveDeployment();
        NodeManager::ControlMessage NodeQuery(const std::string& node_endpoint);
        

    private:
        struct Request{
            std::string request_data_;
            std::promise<std::string>* response_;
        };
        //Constants
        const int HEARTBEAT_PERIOD = 2000;

        const int REQUEST_TIMEOUT = 3000;

        //ZMQ endpoints
        std::string manager_address_;
        std::string manager_endpoint_;
        std::string manager_update_endpoint_;
        
        //Threads
        void HeartbeatLoop();
        std::thread* heartbeat_thread_;
        bool end_flag_ = false;

        //Request helpers
        std::future<std::string> QueueRequest(const std::string& request);
        void SendRequest(Request request);

        std::string experiment_id_;

        //Local clock
        std::mutex clock_mutex_;
        uint64_t clock_ = 0;
        uint64_t Tick();
        uint64_t SetClock(uint64_t incoming_time);
        uint64_t GetClock();

        //Request queue
        std::mutex request_queue_lock_;
        std::condition_variable request_queue_cv_;
        std::queue<Request> request_queue_;

        //ZMQ sockets and helpers
        zmq::context_t* context_;
        zmq::socket_t* update_socket_;
        void ZMQSendRequest(zmq::socket_t* socket, const std::string& request);
        std::string ZMQReceiveReply(zmq::socket_t* socket);
};

#endif //NODEMANAGER_ENVIRONMENTREQUESTER_H
