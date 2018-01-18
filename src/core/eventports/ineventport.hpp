#ifndef INEVENTPORT_HPP
#define INEVENTPORT_HPP

#include <condition_variable>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <iostream>

#include "eventport.h"
#include "../modellogger.h"
#include "../component.h"

//Interface for a standard templated InEventPort
template <class T> class InEventPort : public EventPort{
    public:
        InEventPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<void (T&) > callback_function, const std::string& middleware);
        ~InEventPort();
        void SetMaxQueueSize(const int max_queue_size);
    protected:
        virtual bool HandleConfigure();
        virtual bool HandlePassivate();
        virtual bool HandleTerminate();

        void EnqueueMessage(T* t);
        int GetQueuedMessageCount();
    private:
        bool rx(T* t, bool process_message = true);
        void receive_loop();
    private:
        std::function<void (T&) > callback_function_;

        
        //Queue Mutex responsible for these Variables
        std::mutex queue_mutex_;
        std::queue<T*> message_queue_;
        int max_queue_size_ = -1;
        int processing_count_ = 0;

        const int terminate_timeout_ms_ = 10000;

        std::mutex notify_mutex_;
        bool terminate_ = false;
        std::condition_variable notify_lock_condition_;

        
        std::mutex rx_setup_mutex_;
        bool rx_setup_ = false;
        std::condition_variable rx_setup_condition_;

        std::mutex control_mutex_;
        //Normal Mutex is for changing properties 
        std::thread* queue_thread_ = 0;

        std::mutex thread_finished_mutex_;
        std::condition_variable thread_finished_condition_;
        int running_thread_count_ = 0;
};

template <class T>
InEventPort<T>::InEventPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<void (T&) > callback_function, const std::string& middleware)
:EventPort(component, port_name, EventPort::Kind::RX, middleware){
    if(callback_function){
        callback_function_ = callback_function;
    }else{
        Log(Severity::WARNING).Msg("Got a null callback function").Context(this).Func(GET_FUNC);
    }
};

template <class T>
InEventPort<T>::~InEventPort(){
};

template <class T>
void InEventPort<T>::SetMaxQueueSize(int max_queue_size){
    std::lock_guard<std::mutex> lock(queue_mutex_);
    max_queue_size_ = max_queue_size;
};

template <class T>
bool InEventPort<T>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(EventPort::HandlePassivate()){
        //Gain the notify_mutex to flag the terminate flag
        {
            std::unique_lock<std::mutex> lock(notify_mutex_);
            terminate_ = true;
        }
        notify_lock_condition_.notify_all();
        return true;
    }
    return false;
};

template <class T>
bool InEventPort<T>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(EventPort::HandleConfigure()){
        if(!queue_thread_){
            std::unique_lock<std::mutex> lock2(rx_setup_mutex_);
            rx_setup_ = false;
            queue_thread_ = new std::thread(&InEventPort<T>::receive_loop, this);
            rx_setup_condition_.wait(lock2, [=]{return rx_setup_;});
            return true;
        }
    }
    return false;
};


template <class T>
bool InEventPort<T>::HandleTerminate(){
    InEventPort<T>::HandlePassivate();
    std::unique_lock<std::mutex> lock(control_mutex_);
    if(EventPort::HandleTerminate()){
        std::unique_lock<std::mutex> thread_lock(thread_finished_mutex_);
        //Wait until we have no running threads, or we time out.
        bool thread_finished = thread_finished_condition_.wait_for(thread_lock, std::chrono::milliseconds(terminate_timeout_ms_), [this]{return running_thread_count_ == 0;});

        if(!thread_finished){
            //If the thread didn't finish we can't terminate it, so we should detach and let the operation system decide what happens to the thread
            //The thread may remain running after termination, in which case we can't handle this operation
            //https://stackoverflow.com/questions/19744250/what-happens-to-a-detached-thread-when-main-exits
            queue_thread_->detach();
        }else{
            //Thread should instantly join, as we know its finished
            queue_thread_->join();
        }

        delete queue_thread_;
        queue_thread_ = 0;
        return true;
    }
    return false;
};

template <class T>
bool InEventPort<T>::rx(T* t, bool process_message){
    if(t){
        //Only process the message if we are running and we have a callback, and we aren't meant to ignore
        process_message &= is_running() && callback_function_;

        if(process_message){
            //Call into the function and log
            logger()->LogComponentEvent(*this, *t, ModelLogger::ComponentEvent::STARTED_FUNC);
            callback_function_(*t);
            logger()->LogComponentEvent(*this, *t, ModelLogger::ComponentEvent::FINISHED_FUNC);
        }

        EventProcessed(*t, process_message);
        delete t;
        return process_message;
    }
    return false;
};

template <class T>
void InEventPort<T>::receive_loop(){
    {
        std::unique_lock<std::mutex> queue_lock(thread_finished_mutex_);
        running_thread_count_ ++;
    }
    {
        //Notify that the thread is ready
        std::lock_guard<std::mutex> lock(rx_setup_mutex_);
        rx_setup_ = true;
        rx_setup_condition_.notify_all();
    }

    //Store a queue of messages
    std::queue<T*> queue;
    
    bool running = true;
    while(running){
        {
            std::unique_lock<std::mutex> notify_lock(notify_mutex_);
            notify_lock_condition_.wait(notify_lock, [this]{
                if(terminate_){
                    //Wake up if the termination flag has been set
                    return true;
                }else{
                    //Wake up if we have new messages to process
                    std::unique_lock<std::mutex> queue_lock(queue_mutex_);
                    return message_queue_.size() > 0;
                }
            });

            //Gain Mutex
            std::unique_lock<std::mutex> queue_lock(queue_mutex_);
            //Swap out the queue's, and release the mutex
            message_queue_.swap(queue);
            //Update the current processing count
            processing_count_ += queue.size();

            if(terminate_ && queue.empty()){
                running = false;
            }
        }

        while(!queue.empty()){
            auto m = queue.front();
            queue.pop();

            //If the component is Passivated, this will return false instantaneously
            rx(m);
            
            std::unique_lock<std::mutex> queue_lock(queue_mutex_);
            //Decrement our count of how many messages are currently being processed
            processing_count_ --;
        }
    }

    {
        std::unique_lock<std::mutex> queue_lock(thread_finished_mutex_);
        running_thread_count_ --;
        //Notify that our thread is dead
        thread_finished_condition_.notify_all();
    }
};

template <class T>
void InEventPort<T>::EnqueueMessage(T* t){
    if(t){
        //Log the recieving
        EventRecieved(*t);
        
        std::unique_lock<std::mutex> lock(queue_mutex_);
        //Sum the total number of messages we are processing
        auto queue_size = message_queue_.size() + processing_count_;

        //We should enqueue the message, if we are running, and we have room in our queue size (Or we don't care about queue size)
        bool enqueue_message = is_running() && (max_queue_size_ == -1 || max_queue_size_ > queue_size);

        if(enqueue_message){
            message_queue_.push(t);
            //Notify the thread that we have new messages
            notify_lock_condition_.notify_all();
        }else{
            lock.unlock();
            //Call the rx function, saying that we will ignore the message
            rx(t, false);
        }
    }
};



template <class T>
int InEventPort<T>::GetQueuedMessageCount(){
    //Gain mutex lock and append message to queue
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return message_queue_.size() + processing_count_;
};
                
#endif //INEVENTPORT_HPP