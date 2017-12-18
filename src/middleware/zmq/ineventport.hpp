#ifndef ZMQ_INEVENTPORT_H
#define ZMQ_INEVENTPORT_H

#include <core/eventports/ineventport.hpp>
#include "zmqhelper.h"
#include <re_common/zmq/zmqutils.hpp>

#include <thread>
#include <mutex>

namespace zmq{
     template <typename T, typename S> class InEventPort: public ::InEventPort<T>{
        public:
            InEventPort(std::weak_ptr<Component> component, std::string name, std::function<void (T&) > callback_function);
            ~InEventPort(){
                Activatable::Terminate();
            }
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        private:
            void zmq_loop();
            enum class ThreadState{WAITING, STARTED, ERROR, TERMINATE};

            std::thread* zmq_thread_ = 0;
            std::string terminate_endpoint_;
            std::shared_ptr<Attribute> end_points_;

            std::mutex thread_state_mutex_;
            ThreadState thread_state_;
            std::condition_variable thread_state_condition_;
            
            std::mutex control_mutex_;
            
            std::mutex terminate_mutex_;
            zmq::socket_t* term_socket_ = 0;
            const std::string terminate_str = "TERMINATE";
    }; 
};


template <class T, class S>
zmq::InEventPort<T, S>::InEventPort(std::weak_ptr<Component> component, std::string name, std::function<void (T&) > callback_function):
::InEventPort<T>(component, name, callback_function, "zmq"){
    auto component_ = component.lock();
    auto component_name = component_ ? component_->get_name() : "??";
    terminate_endpoint_ = "inproc://term*" + component_name + "*" + name + "*";

    end_points_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_address").lock();
};

template <class T, class S>
bool zmq::InEventPort<T, S>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    bool valid = end_points_->StringList().size() >= 0;
    if(valid && ::InEventPort<T>::HandleConfigure()){
        if(!zmq_thread_){
            std::unique_lock<std::mutex> lock(thread_state_mutex_);
            thread_state_ = ThreadState::WAITING;
            zmq_thread_ = new std::thread(&zmq::InEventPort<T, S>::zmq_loop, this);
            thread_state_condition_.wait(lock, [=]{return thread_state_ != ThreadState::WAITING;});
            return thread_state_ == ThreadState::STARTED;
        }
    }
    return false;
}


template <class T, class S>
bool zmq::InEventPort<T, S>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::HandleTerminate()){
        if(zmq_thread_){
            //Join our zmq_thread
            zmq_thread_->join();
            delete zmq_thread_;
            zmq_thread_ = 0;
        }
        return true;
    }
    return false;
};

template <class T, class S>
bool zmq::InEventPort<T, S>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::HandlePassivate()){
        std::lock_guard<std::mutex> lock(terminate_mutex_);
        if(term_socket_){
            zmq::message_t term_msg(terminate_str.c_str(), terminate_str.size());
            //Send a blank message to interupt the recv loop
            term_socket_->send(term_msg);
        }
        return true;
    }
    return false;
};

template <class T, class S>
void zmq::InEventPort<T, S>::zmq_loop(){
    terminate_mutex_.lock();
    auto helper = ZmqHelper::get_zmq_helper();
    auto socket = helper->get_subscriber_socket();
    term_socket_ = helper->get_publisher_socket();

    auto state = ThreadState::STARTED;
    try{
        term_socket_->bind(terminate_endpoint_.c_str());
        socket->connect(terminate_endpoint_.c_str());
    }catch(zmq::error_t ex){
        Log(Severity::ERROR).Context(this).Func(__func__).Msg("Cannot connect to terminate endpoint: '" + terminate_endpoint_ + "' " + ex.what());
        state = ThreadState::ERROR;
    }

    for(auto e: end_points_->StringList()){
        try{
            //connect the addresses provided
            socket->connect(e.c_str());
        }catch(zmq::error_t ex){
            Log(Severity::ERROR).Context(this).Func(__func__).Msg("Cannot connect to endpoint: '" + terminate_endpoint_ + "' " + ex.what());
            state = ThreadState::ERROR;
        }
    }

   
    //Construct a new ZMQ Message to store the resulting message in.
    zmq::message_t data;

    //Sleep for 250 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //Release the temrinate mutex
    terminate_mutex_.unlock();

    //Change the state to be Configured
    {
        std::lock_guard<std::mutex> lock(thread_state_mutex_);
        thread_state_ = state;
        thread_state_condition_.notify_all();
    }

    if(state == ThreadState::STARTED && Activatable::BlockUntilStateChanged(Activatable::State::RUNNING)){
        //Log the port becoming online
        EventPort::LogActivation();
    
        while(true){
            try{
                //Wait for next message
                socket->recv(&data);
                
                auto msg_str = Zmq2String(data);
                
                if(msg_str == terminate_str){
                    break;
                }
                
                auto m = proto::decode<S>(msg_str);
                this->EnqueueMessage(m);
            }catch(zmq::error_t ex){
                Log(Severity::ERROR).Context(this).Func(__func__).Msg(ex.what());
                break;
            }
        }
        EventPort::LogPassivation();
    }

    delete socket;


    //Gain the terminate_socket to kill
    std::lock_guard<std::mutex> lock(terminate_mutex_);
    delete term_socket_;
    term_socket_ = 0;
};


#endif //ZMQ_INEVENTPORT_H
