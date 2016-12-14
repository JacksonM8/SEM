#ifndef OSPL_OUTEVENTPORT_H
#define OSPL_OUTEVENTPORT_H

#include "../globalinterfaces.h"

#include <iostream>
#include <string>

#include "osplhelper.h"

namespace ospl{
     template <class T, class S> class Ospl_OutEventPort: public ::OutEventPort<T>{
        public:
            Ospl_OutEventPort(::OutEventPort<T>* port, int domain_id, std::string publisher_name, std::string writer_name, std::string topic_name);
            void notify();
        private:
            void tx_(T* message);

            dds::pub::DataWriter<S> writer_;
            ::OutEventPort<T>* port_;
    }; 
};

template <class T, class S>
void ospl::OutEventPort<T, S>::tx_(T* message){
    //Call the translate function
    auto m = translate(message);
    //De-reference the message and send
    writer_.write(*m);
    delete m;
};

template <class T, class S>
ospl::OutEventPort<T, S>::Ospl_OutEventPort(::OutEventPort<T>* port, int domain_id, std::string publisher_name, std::string writer_name, std::string topic_name){
    this->port_ = port;
    
    auto helper = OsplHelper::get_ospl_helper();   
    auto participant = helper->get_participant(domain_id);
    auto publisher = helper->get_publisher(participant, publisher_name);
    auto topic = helper->get_topic<S>(participant, topic_name);
    writer_ = helper->get_data_writer<S>(subscriber, topic, writer_name);
};

#endif //OSPL_OUTEVENTPORT_H
