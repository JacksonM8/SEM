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
 
#include "protoreceiver.h"
#include <iostream>
#include "../monitor/monitor.h"
#include "../zmqutils.hpp"

zmq::ProtoReceiver::ProtoReceiver(){
    //Setup ZMQ context
    context_ = new zmq::context_t();
}

bool zmq::ProtoReceiver::Start(){
    std::unique_lock<std::mutex> lock(thread_mutex_);
    if(!reciever_thread_ && !reciever_thread_){
        terminate_proto_convert_thread_ = false;
        reciever_thread_ = new std::thread(&zmq::ProtoReceiver::RecieverThread, this);
        proto_convert_thread_ = new std::thread(&zmq::ProtoReceiver::ProtoConvertThread, this);
        return true;
    }
    return false;
}

void zmq::ProtoReceiver::SetBatchMode(bool on, int size){
    std::unique_lock<std::mutex> lock(queue_mutex_);
    batch_size_ = on ? size : 0;

    if(batch_size_ < 0){
        batch_size_ = 0;
    }
}

bool zmq::ProtoReceiver::Terminate(){
    std::unique_lock<std::mutex> lock(thread_mutex_);
    if(proto_convert_thread_ && reciever_thread_){
        {
            //Gain the lock so we can notify and set our terminate flag.
            std::unique_lock<std::mutex> lock(queue_mutex_);
            terminate_proto_convert_thread_ = true;
            queue_lock_condition_.notify_all();
        }
        proto_convert_thread_->join();
        delete proto_convert_thread_;
        proto_convert_thread_ = 0;

        if(context_){
            delete context_;
            context_ = 0;
        }
        //Terminate the receiver thread
        reciever_thread_->join();
        delete reciever_thread_;
        reciever_thread_ = 0;
        return true;
    }
    return false;
}

zmq::ProtoReceiver::~ProtoReceiver(){
    Terminate();
}

bool zmq::ProtoReceiver::AttachMonitor(zmq::Monitor* monitor, const int event_type){
    std::unique_lock<std::mutex> lock(zmq_mutex_);
    if(monitor && socket_){
        //Attach monitor; using a new address
        monitor->MonitorSocket(socket_, GetNewMonitorAddress(), event_type);
        return true;
    }
    return false;
}


void zmq::ProtoReceiver::RecieverThread(){
    //Gain mutex lock to ensure we are the only thing working at this point.
    {
        std::unique_lock<std::mutex> lock(zmq_mutex_);
        //Setup our Subscriber socket
        socket_ = new zmq::socket_t(*context_, ZMQ_SUB);

        //Connect to all nodes on our network
        for (const auto& a : addresses_){
            Connect_(a);
        }
        for (const auto& f : filters_){
            Filter_(f);
        }
    }
    

    while(true){
		try{
            zmq::message_t topic;

            std::pair<zmq::message_t, zmq::message_t> p;

            //Wait for Topic, Type and Data
            socket_->recv(&topic);
            socket_->recv(&(p.first));
            socket_->recv(&(p.second));
            
            std::unique_lock<std::mutex> lock(queue_mutex_);
            rx_message_queue_.push(std::move(p));
                
            //Notify the ProtoConvertThread
            if(rx_message_queue_.size() > batch_size_){
                queue_lock_condition_.notify_all();
            }
        }catch(zmq::error_t &ex){
            if(ex.num() != ETERM){
                std::cerr << "zmq::ProtoReceiver::RecieverThread: " << ex.what() << std::endl;
            }
			break;
        }
    }

    std::unique_lock<std::mutex> lock(zmq_mutex_);
    delete socket_;
    socket_ = 0;
}


bool zmq::ProtoReceiver::RegisterNewProto(const google::protobuf::MessageLite &message_type, std::function<void(const google::protobuf::MessageLite&)> fn){
    std::unique_lock<std::mutex> lock(proto_mutex_);
    
    const auto& type_name = message_type.GetTypeName();
    
    //Register a constructor
    if(!proto_lookup_.count(type_name)){
        proto_lookup_[type_name] = [&message_type](const zmq::message_t& message){
            auto message_pb = message_type.New();

            if(!message_pb->ParseFromArray(message.data(), message.size())){
                delete message_pb;
                message_pb = 0;
            };
            return message_pb;
            };
    }
    //Add the callback
    callback_lookup_.insert(std::make_pair(type_name, fn));
    return true;
}

int zmq::ProtoReceiver::GetRxCount(){
    std::unique_lock<std::mutex> lock(proto_mutex_);
    return rx_count_;
}

bool zmq::ProtoReceiver::ProcessMessage(const zmq::message_t& type, const zmq::message_t& data){
    const auto& type_str = Zmq2String(type);
    std::unique_lock<std::mutex> lock(proto_mutex_);
    bool success = false;
    if(proto_lookup_.count(type_str)){
        //Construct a new protobuff message
        auto message_object = proto_lookup_[type_str](data);
        
        //Parse into the message_object
        if(message_object){
            rx_count_ ++;

            //Run our callbacks
            for(const auto& c : callback_lookup_){
                if(c.first == type_str){
                    c.second(*message_object);
                }
            }
            success = true;
        }else{
            std::cerr << "zmq::ProtoReceiver::Cannot Parse: Proto Type: " << type_str << std::endl;
        }
        delete message_object;
    }else{
        std::cerr << "zmq::ProtoReceiver::Proto Type: " << type_str << " not registered" << std::endl;
    }
    return success;
}


void zmq::ProtoReceiver::ProtoConvertThread(){
    //Update loop.
    while(true){
        std::queue<std::pair<zmq::message_t, zmq::message_t> > replace_queue; 
        {
            //Obtain lock for the queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
            //Wait until we have messages or are told to terminate
            queue_lock_condition_.wait(lock, [this]{
                return terminate_proto_convert_thread_ || !rx_message_queue_.empty();
            });

            //Break out if we need to terminat
            if(terminate_proto_convert_thread_){
                return;
            }else{
                //Now that we have access, swap out queues and release the mutex
                rx_message_queue_.swap(replace_queue);
            }
        }

        while(!replace_queue.empty()){
            const auto& type = replace_queue.front().first;
            const auto& msg = replace_queue.front().second;

            ProcessMessage(type, msg);
            replace_queue.pop();
        }
    }
}

bool zmq::ProtoReceiver::Connect_(const std::string& address){
    //If we have a reciever_thread_ active we can directly interact
    try{
        if(socket_){
            socket_->connect(address.c_str());
            return true;
        }
    }
    catch(zmq::error_t ex){
        std::cerr << "zmq::ProtoReceiver: Connect to: " << address << " Failed! " << ex.what() << std::endl;
    }    
    return false;
}

bool zmq::ProtoReceiver::Filter_(const std::string& topic_filter){
    //If we have a reciever_thread_ active we can directly interact
    try{
        if(socket_){
            //Subscribe to specific topic
            socket_->setsockopt(ZMQ_SUBSCRIBE, topic_filter.c_str(), topic_filter.size());
            return true;
        }
    }
    catch(zmq::error_t ex){
        std::cerr << "zmq::ProtoReceiver: Filter: " << topic_filter << " Failed! " << ex.what() << std::endl;
    }    
    return false;
}

bool zmq::ProtoReceiver::Filter(const std::string& filter){
    std::unique_lock<std::mutex> lock(zmq_mutex_);

    if(!reciever_thread_){
        //Append to addresses.
        filters_.push_back(filter);        
        return true;
    }else{
        return Filter_(filter);
    }
}

bool zmq::ProtoReceiver::Connect(const std::string& address){
    std::unique_lock<std::mutex> lock(zmq_mutex_);

    if(!reciever_thread_){
        //Append to addresses.
        addresses_.push_back(address);        
        return true;
    }else{
        return Connect_(address);
    }
}