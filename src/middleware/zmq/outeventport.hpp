#ifndef ZMQ_OUTEVENTPORT_H
#define ZMQ_OUTEVENTPORT_H

#include <core/eventports/outeventport.hpp>
#include "zmqhelper.h"

namespace zmq{
     template <class T, class S> class OutEventPort: public ::OutEventPort<T>{
        public:
            OutEventPort(std::shared_ptr<Component> component, std::string name);
            ~OutEventPort(){
                Activatable::Terminate();
            }
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        public:
            bool tx(T* message);
        private:
            bool setup_tx();
            std::mutex control_mutex_;
            
            zmq::socket_t* socket_ = 0;
            std::shared_ptr<Attribute> end_points_;
    }; 
};

template <class T, class S>
zmq::OutEventPort<T, S>::OutEventPort(std::shared_ptr<Component> component, std::string name): ::OutEventPort<T>(component, name, "zmq"){
    end_points_ = ::OutEventPort<T>::AddAttribute(std::make_shared<Attribute>(ATTRIBUTE_TYPE::STRINGLIST, "publisher_address"));
};



template <class T, class S>
bool zmq::OutEventPort<T, S>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool valid = end_points_->StringList().size() > 0;

    if(valid && ::OutEventPort<T>::HandleConfigure()){
        return setup_tx();
    }
    return false;
};

template <class T, class S>
bool zmq::OutEventPort<T, S>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::OutEventPort<T>::HandlePassivate()){
        if(socket_){
            delete socket_;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            socket_ = 0;

        }
        return true;
    }
    return false;
};

template <class T, class S>
bool zmq::OutEventPort<T, S>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    return ::OutEventPort<T>::HandleTerminate();
};

template <class T, class S>
bool zmq::OutEventPort<T, S>::setup_tx(){
    auto helper = ZmqHelper::get_zmq_helper();
    this->socket_ = helper->get_publisher_socket();
    for(auto e: end_points_->StringList()){
        try{
            //Bind the addresses provided
            this->socket_->bind(e.c_str());
        }catch(zmq::error_t ex){
            std::cerr << "zmq::OutEventPort<T, S>::zmq_loop(): Couldn't connect to endpoint: '" << e << "'" << std::endl;
            std::cerr << ex.what() << std::endl;
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return true;
};

template <class T, class S>
bool zmq::OutEventPort<T, S>::tx(T* message){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool should_send = ::OutEventPort<T>::tx(message);

    if(should_send){
        if(socket_){
            std::string str = proto::encode(message);
            zmq::message_t data(str.c_str(), str.size());
            socket_->send(data);
            delete message;
            return true;
        }else{
            std::cerr << "Socket Dead when not meant to be" << std::endl;
        }
    }
    return false;
};

#endif //ZMQ_INEVENTPORT_H

