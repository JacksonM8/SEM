#ifndef RTI_INEVENTPORT_H
#define RTI_INEVENTPORT_H

#include "../../core/eventports/ineventport.hpp"

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "helper.hpp"
#include "datareaderlistener.hpp"



namespace rti{
     template <class T, class S> class InEventPort: public ::InEventPort<T>{
        public:
            InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function);
            void notify();

            void Startup(std::map<std::string, ::Attribute*> attributes);
            bool Teardown();
            bool Passivate();


        private:
            void receive_loop();
            
            std::thread* rec_thread_ = 0;
            std::mutex notify_mutex_;
            std::mutex control_mutex_;
            std::condition_variable notify_lock_condition_;

            std::string topic_name_;
            std::string qos_profile_path_;
            std::string qos_profile_name_;
            int domain_id_ = 0;
            std::string subscriber_name_;
            bool passivate_ = false;
    }; 
};

template <class T, class S>
void rti::InEventPort<T, S>::Startup(std::map<std::string, ::Attribute*> attributes){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(attributes.count("topic_name")){
        topic_name_ = attributes["topic_name"]->get_String();
    }

    if(attributes.count("subscriber_name")){
        subscriber_name_ = attributes["subscriber_name"]->get_String();
    }else{
        subscriber_name_ = "In_" + this->get_name();
    }

    if(attributes.count("domain_id")){
        domain_id_ = attributes["domain_id"]->get_Integer();
    }

    if(attributes.count("qos_profile_name")){
        qos_profile_name_ = attributes["qos_profile_name"]->get_String();
    }
    if(attributes.count("qos_profile_path")){
        qos_profile_path_ = attributes["qos_profile_path"]->get_String();
    }

    std::cout << "rti::InEventPort" << std::endl;
    std::cout << "**domain_id_: " << domain_id_ << std::endl;
    std::cout << "**subscriber_name: " << subscriber_name_ << std::endl;
    std::cout << "**topic_name_: "<< topic_name_ << std::endl;
    std::cout << "**qos_profile_path: " << qos_profile_path_ << std::endl;
    std::cout << "**qos_profile_name: " << qos_profile_name_ << std::endl << std::endl;


    if(topic_name_.length() && subscriber_name_.length() && domain_id_ >= 0){
        std::cout <<  "Constructing Thread!" << std::endl;
        if(!rec_thread_){
            std::cout <<  "Constructing Thread2!" << std::endl;
            rec_thread_ = new std::thread(&rti::InEventPort<T, S>::receive_loop, this);
            Activatable::WaitForStartup();
        }
    }else{
        std::cout << "rti::InEventPort<T, S>::startup: No Valid Topic_name + subscriber_names" << std::endl;
    }
};

template <class T, class S>
bool rti::InEventPort<T, S>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    {
        std::lock_guard<std::mutex> lock(notify_mutex_);
        passivate_ = true;
    }
    //Unlock the reciever
    notify();
    return ::InEventPort<T>::Passivate();
};


template <class T, class S>
bool rti::InEventPort<T, S>::Teardown(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::Teardown()){
        if(rec_thread_){
            //Join our zmq_thread
            rec_thread_->join();
            delete rec_thread_;
            rec_thread_ = 0;
        }
        return true;
    }
    return false;
};


template <class T, class S>
void rti::InEventPort<T, S>::notify(){
    //Called by the DataReaderListener to notify our InEventPort thread to get new data
    notify_lock_condition_.notify_all();
};


template <class T, class S>
rti::InEventPort<T, S>::InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function):
::InEventPort<T>(component, name, callback_function, "rti"){
};

template <class T, class S>
void rti::InEventPort<T, S>::receive_loop(){
    dds::sub::DataReader<S> reader_ = dds::sub::DataReader<S>(dds::core::null);

    bool success = false;
    try{
        //Construct a DDS Participant, Subscriber, Topic and Reader
        auto helper = DdsHelper::get_dds_helper();    
        auto participant = helper->get_participant(domain_id_);
        auto topic = get_topic<S>(participant, topic_name_);

        auto subscriber = helper->get_subscriber(participant, subscriber_name_);
        reader_ = get_data_reader<S>(subscriber, topic, qos_profile_path_, qos_profile_name_);
        
        //Construct a DDS Listener, designed to call back into the receive thread
        auto listener_ = new rti::DataReaderListener<T, S>(this);
        //Attach listener to only respond to data_available()
        reader_.listener(listener_, dds::core::status::StatusMask::data_available());
        success = true;
    }catch(...){
        std::cerr << "ERROR!" << std::endl;
    }

    
    //Notify Startup our thread is good to go
    Activatable::StartupFinished();
    
    if(!success){
        std::cout << "GOT ERROR!" << std::endl;
        //Return back on error
        return;
    }
    
    //Wait for the port to be activated before starting!
    Activatable::WaitForActivate();
    //Log the port becoming online
    EventPort::LogActivation();

    std::cout << "Waiting for notify!" << std::endl;
    while(true){
        {
            //Wait for next message
            std::unique_lock<std::mutex> lock(notify_mutex_);
            notify_lock_condition_.wait(lock);
            if(this->passivate_){
                break;
            }
        }
        ///Read all our samples
        try{
            std::cout << "WAiting for samples!" << std::endl;
            auto samples = reader_.take();
            for(auto sample : samples){
                //Translate and callback into the component for each valid message we receive
                if(sample->info().valid()){
                    auto m = rti::translate(&sample->data());
                    this->EnqueueMessage(m);
                }
            }
        }catch(...){//dds::core::PreconditionNotMetError e){
            std::cout << "ERROR" << std::endl;
        }
        
    }
    EventPort::LogPassivation();
};

#endif //RTI_INEVENTPORT_H
