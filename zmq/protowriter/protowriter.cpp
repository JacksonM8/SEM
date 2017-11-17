/* re_common
 * Copyright (C) 2016-2017 The University of Adelaide
 *
 * This file is part of "re_common"
 *
 * "re_common" is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * "re_common" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
 
#include "protowriter.h"
#include <iostream>
#include <zmq.hpp>
#include <google/protobuf/message_lite.h>
#include <thread>
#include "../monitor/monitor.h"

zmq::ProtoWriter::ProtoWriter(){
    std::unique_lock<std::mutex> lock(mutex_);
    context_ = new zmq::context_t();
    socket_ = new zmq::socket_t(*context_, ZMQ_PUB);
    //Increase the HighWaterMark to 10,000 to make sure we don't lose messages
    socket_->setsockopt(ZMQ_SNDHWM, 10000);
}

zmq::ProtoWriter::~ProtoWriter(){
    Terminate();
}

bool zmq::ProtoWriter::AttachMonitor(zmq::Monitor* monitor, const int event_type){
    std::unique_lock<std::mutex> lock(mutex_);
    if(monitor && socket_){
        //Attach monitor; using a new address
        return monitor->MonitorSocket(socket_, GetNewMonitorAddress(), event_type);
    }
    return false;
}

bool zmq::ProtoWriter::BindPublisherSocket(const std::string& endpoint){
    //Gain the lock
    std::unique_lock<std::mutex> lock(mutex_);
    if(socket_){
        try{
            socket_->bind(endpoint.c_str());    
        }catch(zmq::error_t &ex){
            std::cerr << "zmq::ProtoWriter::BindPublisherSocket(" << endpoint << "): " << ex.what() << std::endl;
            return false;
        }
        return true;
    }
    return false;
}

bool zmq::ProtoWriter::PushMessage(const std::string& topic, google::protobuf::MessageLite* message){
    bool success = false;
    if(message){
        std::string message_str;
        if(message->SerializeToString(&message_str)){
           success = PushString(topic, message->GetTypeName(), message_str);
        }
        delete message;
    }
    return success;
}

bool zmq::ProtoWriter::PushString(const std::string& topic, const std::string& type, const std::string& message){
    std::unique_lock<std::mutex> lock(mutex_);
    if(socket_){
        //Construct a zmq message for both the type and message data
        zmq::message_t topic_data(topic.c_str(), topic.size());
        zmq::message_t type_data(type.c_str(), type.size());
        zmq::message_t message_data(message.c_str(), message.size());
        
        //Send Type then Data
        try{
            socket_->send(topic_data, ZMQ_SNDMORE);
            socket_->send(type_data, ZMQ_SNDMORE);
            socket_->send(message_data);
            return true;   
        }catch(zmq::error_t &ex){
            std::cerr << "zmq::ProtoWriter::PushString(): " << ex.what() << std::endl;
        }
    }
    return false;
}

bool zmq::ProtoWriter::Terminate(){
    std::unique_lock<std::mutex> lock(mutex_);

    if(context_ && socket_){
        delete socket_;
        socket_ = 0;
        delete context_;
        context_ = 0;
        return true;
    }
    return false;
}
//Gain the lock
    