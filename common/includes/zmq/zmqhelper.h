#ifndef ZMQ_ZMQHELPER_H
#define ZMQ_ZMQHELPER_H

#include <string>
#include <mutex>

//Include ZMQ Headers
#include "zmq.hpp"

namespace zmq{
    class ZmqHelper{
        public:
            //Static getter functions
            static ZmqHelper* get_zmq_helper();
            static std::mutex global_mutex_;
        
        public:
            zmq::context_t* get_context();
            zmq::socket_t* get_publisher_socket();
            zmq::socket_t* get_subscriber_socket();
        private:
            std::mutex mutex;
            zmq::context_t* context_ = 0;

            static ZmqHelper* singleton_;
    };
};

#endif //ZMQ_ZMQHELPER_H

