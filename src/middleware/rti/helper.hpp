#ifndef RTI_DDS_HELPER_H
#define RTI_DDS_HELPER_H

#include <iostream>
#include <string>
#include <mutex>

//Include RTI DDS Headers
#include <rti/rti.hpp>

#include "ddshelper.h"

namespace rti
{
    template<class M> dds::topic::Topic<M> get_topic(dds::domain::DomainParticipant participant, std::string topic_name);
    template<class M> dds::sub::DataReader<M> get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string qos_uri = "", std::string qos_profile = "");
    template<class M> dds::pub::DataWriter<M> get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string qos_uri = "", std::string qos_profile = "");
};

template<class M> dds::topic::Topic<M> rti::get_topic(dds::domain::DomainParticipant participant, std::string topic_name){
    //Acquire the Lock from the DDS Helper
    std::lock_guard<std::mutex> guard(DdsHelper::get_dds_helper()->mutex);

    dds::topic::Topic<M> topic = nullptr;
    try{
        //Try use the Middleware's find function to find the topic
        topic = dds::topic::find<dds::topic::Topic<M> >(participant, topic_name);
    }catch(dds::core::InvalidDowncastError	e){
        std::cout << "rti::get_topic: Error: " << e.what() << std::endl;
    }

    if(topic == nullptr){
        //If we can't find the topic, we should construct it
        topic = dds::topic::Topic<M>(participant, topic_name);
        topic.retain();
        std::cout << "rti::get_topic: Constructed Topic: " << topic_name << std::endl;
    }
    
    return topic;
};

template<class M> dds::pub::DataWriter<M> rti::get_data_writer(dds::pub::Publisher publisher, dds::topic::Topic<M> topic, std::string qos_uri, std::string qos_profile){
    //Acquire the Lock from the DDS Helper
    std::lock_guard<std::mutex> guard(DdsHelper::get_dds_helper()->mutex);

    dds::pub::DataWriter<M> writer = nullptr;

    //If we have the publisher and the topic, construct the writer.
    if(publisher != nullptr && topic != nullptr){
        writer = dds::pub::DataWriter<M>(publisher, topic);
        writer.retain();
        std::cout << "rti::get_data_writer: Constructed DataWriter" << std::endl;
    }
    return writer;
};

template<class M> dds::sub::DataReader<M> rti::get_data_reader(dds::sub::Subscriber subscriber, dds::topic::Topic<M> topic, std::string qos_uri, std::string qos_profile){
    //Acquire the Lock from the DDS Helper
    std::lock_guard<std::mutex> guard(DdsHelper::get_dds_helper()->mutex);
    
    dds::sub::DataReader<M> reader = nullptr;
    
    //If we have the subscriber and the topic, construct the writer.
    if(subscriber != nullptr && topic != nullptr){
        reader = dds::sub::DataReader<M>(subscriber, topic);
        reader.retain();
        std::cout << "rti::get_data_reader: Constructed DataReader" << std::endl;
    }
    return reader;
};

#endif //RTI_DDS_HELPER_H