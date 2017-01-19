#ifndef RTI_OUTEVENTPORT_H
#define RTI_OUTEVENTPORT_H

#include "../core/eventports/outeventport.hpp"

#include <string>

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
    if(writer_ != dds::core::null){
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
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(attributes.count("publisher_name")){
        publisher_name_ = attributes["publisher_name"]->s;
        configured_ = true;
    }
    if(attributes.count("topic_name")){
        topic_name_ = attributes["topic_name"]->s;
        configured_ = true && configured_;
    }
    if(attributes.count("domain_id")){
        domain_id_ = attributes["domain_id"]->s;
        configured_ = true && configured_;                
    }
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
    auto publisher = helper->get_publisher(participant, publisher_name_);
    auto topic = helper->get_topic<S>(participant, topic_name_);
    writer_ = helper->get_data_writer<S>(publisher, topic);

    return ::OutEventPort<T>::activate();
};

template <class T, class S>
bool rti::OutEventPort<T, S>::passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(writer_){
        delete this->writer_;
    }

    return ::OutEventPort<T>passivate();
};

#endif //RTI_OUTEVENTPORT_H