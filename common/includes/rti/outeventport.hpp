#ifndef RTI_OUTEVENTPORT_H
#define RTI_OUTEVENTPORT_H

#include "../core/eventports/outeventport.hpp"

#include <string>
#include <mutex>


#include "helper.hpp"

namespace rti{
     template <class T, class S> class OutEventPort: public ::OutEventPort<T>{
        public:
            OutEventPort(Component* component, std::string name);
            void tx(T* message);

            void startup(std::map<std::string, ::Attribute*> attributes);
            void teardown();

            bool activate();
            bool passivate();
        private:
            std::mutex control_mutex_;

            bool configured_ = false;

            std::string topic_name_;
            int domain_id_;
            std::string publisher_name_;


            dds::pub::DataWriter<S> writer_ = dds::pub::DataWriter<S>(dds::core::null);
    }; 
};

template <class T, class S>
void rti::OutEventPort<T, S>::tx(T* message){
    if(this->is_active() && writer_ != dds::core::null){
        auto m = translate(message);
        //De-reference the message and send
        writer_.write(*m);
        delete m;
    }else{
        //No writer
    }
};

template <class T, class S>
rti::OutEventPort<T, S>::OutEventPort(Component* component, std::string name):
::OutEventPort<T>(component, name){};

template <class T, class S>
void rti::OutEventPort<T, S>::startup(std::map<std::string, ::Attribute*> attributes){
    {std::lock_guard<std::mutex> lock(control_mutex_);

    if(attributes.count("publisher_name")){
        publisher_name_ = attributes["publisher_name"]->get_string();
        configured_ = true;
    }
    if(attributes.count("topic_name")){
        topic_name_ = attributes["topic_name"]->get_string();
        configured_ = true && configured_;
    }
    if(attributes.count("domain_id")){
        domain_id_ = attributes["domain_id"]->i;
        configured_ = true && configured_;                
    }

    
    std::cout << "rti::OutEventPort" << std::endl;
    std::cout << "**domain_id_: "<< domain_id_ << std::endl;
    std::cout << "**publisher_name_: "<< publisher_name_ << std::endl;
    std::cout << "**topic_name_: "<< topic_name_ << std::endl << std::endl;


    if(configured_){
       // auto helper = DdsHelperS::get_dds_helper();   
       // auto participant = helper->get_participant(domain_id_);
       // auto topic = get_topic<S>(participant, topic_name_);
    }
}
    //activate();
};

template <class T, class S>
void rti::OutEventPort<T, S>::teardown(){
    passivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    configured_ = false;
};

template <class T, class S>
bool rti::OutEventPort<T, S>::activate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    //Construct a DDS Participant, Publisher, Topic and Writer
    auto helper = DdsHelper::get_dds_helper();   
    auto participant = helper->get_participant(domain_id_);
    auto topic = get_topic<S>(participant, topic_name_);
    
    //std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    auto publisher = helper->get_publisher(participant, publisher_name_);
    writer_ = get_data_writer<S>(publisher, topic);
    return ::OutEventPort<T>::activate();
};

template <class T, class S>
bool rti::OutEventPort<T, S>::passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(writer_ != dds::core::null){
        writer_ = dds::core::null;
    }

    return ::OutEventPort<T>::passivate();
};

#endif //RTI_OUTEVENTPORT_H