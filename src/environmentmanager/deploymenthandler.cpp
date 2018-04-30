#include "deploymenthandler.h"
#include <iostream>

#include "deploymentgenerator.h"
#include "deploymentrules/zmq/zmqrule.h"
#include "deploymentrules/dds/ddsrule.h"

DeploymentHandler::DeploymentHandler(Environment* env, zmq::context_t* context, const std::string& ip_addr, 
                                    std::promise<std::string>* port_promise, const std::string& experiment_id){

    environment_ = env;
    context_ = context;
    ip_addr_ = ip_addr;
    experiment_id_ = experiment_id;
    port_promise_ = port_promise;
    handler_thread_ = new std::thread(&DeploymentHandler::Init, this);
}

void DeploymentHandler::Init(){
    handler_socket_ = new zmq::socket_t(*context_, ZMQ_REP);

    time_added_ = environment_->GetClock();

    std::string assigned_port = environment_->AddExperiment(experiment_id_);
    try{
        handler_socket_->bind(TCPify(ip_addr_, assigned_port));

        port_promise_->set_value(assigned_port);

    }catch(zmq::error_t ex){
        //Set our promise as exception and exit if we can't find a free port.
        port_promise_->set_exception(std::current_exception());
        return;
    }

    //Start heartbeat to track liveness of deployment
    //Use heartbeat to receive any updates re. components addition/removal
    HeartbeatLoop();
}

void DeploymentHandler::HeartbeatLoop(){
    //Initialise our poll item list
    std::vector<zmq::pollitem_t> sockets = {{*handler_socket_, 0, ZMQ_POLLIN, 0}};

    //Wait for first heartbeat, allow more time for this one in case of server congestion
    int initial_events = zmq::poll(sockets, INITIAL_TIMEOUT);

    if(initial_events >= 1){
        auto request = ZMQReceiveRequest(handler_socket_);
        HandleRequest(request);
    }
    //Break out early if we never get our first heartbeat
    else{
        delete handler_socket_;
        RemoveExperiment(time_added_);
        return;
    }

    int interval = INITIAL_INTERVAL;
    int liveness = HEARTBEAT_LIVENESS;

    while(!removed_flag_){
        zmq::message_t hb_message;

        //Poll zmq socket for heartbeat message, time out after HEARTBEAT_TIMEOUT milliseconds
        int events = zmq::poll(sockets, HEARTBEAT_INTERVAL);
        if(removed_flag_){
            break;
        }

        if(events >= 1){
            //Reset
            liveness = HEARTBEAT_LIVENESS;
            interval = INITIAL_INTERVAL;
            auto request = ZMQReceiveRequest(handler_socket_);
            HandleRequest(request);
            if(removed_flag_){
                break;
            }
            environment_->ExperimentLive(experiment_id_, time_added_);
        }
        else if(--liveness == 0){
            environment_->ExperimentTimeout(experiment_id_, time_added_);
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if(interval < MAX_INTERVAL){
                interval *= 2;
            }
            else{
                RemoveExperiment(time_added_);
                break;
            }
            liveness = HEARTBEAT_LIVENESS;
        }
    }
    //Broken out of heartbeat loop at this point, remove deployment
    delete handler_socket_;
}

void DeploymentHandler::RemoveExperiment(uint64_t call_time){
    environment_->RemoveExperiment(experiment_id_, call_time);
    removed_flag_ = true;
}

std::string DeploymentHandler::TCPify(const std::string& ip_address, const std::string& port) const{
    return std::string("tcp://" + ip_address + ":" + port);
}

std::string DeploymentHandler::TCPify(const std::string& ip_address, int port) const{
    return std::string("tcp://" + ip_address + ":" + std::to_string(port));
}

void DeploymentHandler::ZMQSendReply(zmq::socket_t* socket, const std::string& message){
    std::string lamport_string = std::to_string(environment_->Tick());
    zmq::message_t time_msg(lamport_string.begin(), lamport_string.end());
    zmq::message_t msg(message.begin(), message.end());

    try{
        socket->send(time_msg, ZMQ_SNDMORE);
        socket->send(msg);
    }
    catch(std::exception e){
        std::cerr << e.what() << " in DeploymentHandler::ZMQSendReply" << std::endl;
    }
}

std::pair<uint64_t, std::string> DeploymentHandler::ZMQReceiveRequest(zmq::socket_t* socket){
    zmq::message_t lamport_time_msg;
    zmq::message_t request_contents_msg;
    try{
        socket->recv(&lamport_time_msg);
        socket->recv(&request_contents_msg);
    }
    catch(zmq::error_t error){
        //TODO: Throw this further up
        std::cerr << error.what() << " in DeploymentHandler::ZMQReceiveRequest" << std::endl;
    }
    std::string contents(static_cast<const char*>(request_contents_msg.data()), request_contents_msg.size());

    //Update and get current lamport time
    std::string incoming_time(static_cast<const char*>(lamport_time_msg.data()), lamport_time_msg.size());

    uint64_t lamport_time = environment_->SetClock(std::stoull(incoming_time));
    return std::make_pair(lamport_time, contents);
}

void DeploymentHandler::HandleRequest(std::pair<uint64_t, std::string> request){
    auto message_time = request.first;
    NodeManager::EnvironmentMessage message;
    message.ParseFromString(request.second);

    try{
        switch(message.type()){

            case NodeManager::EnvironmentMessage::GET_DEPLOYMENT_INFO:{
                //Create generator and populate message
                DeploymentGenerator generator(*environment_);
                generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Zmq::DeploymentRule(*environment_)));
                generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Dds::DeploymentRule(*environment_)));
                generator.PopulateDeployment(*(message.mutable_control_message()));
                message.set_type(NodeManager::EnvironmentMessage::SUCCESS);
                break;
            }

            case NodeManager::EnvironmentMessage::REMOVE_DEPLOYMENT:{
                RemoveExperiment(message_time);
                message.set_type(NodeManager::EnvironmentMessage::SUCCESS);
                break;
            }

            case NodeManager::EnvironmentMessage::HEARTBEAT:{
                if(environment_->ExperimentIsDirty(experiment_id_)){
                    HandleDirtyExperiment(message);
                }else{
                    message.set_type(NodeManager::EnvironmentMessage::HEARTBEAT_ACK);
                }
                break;
            }

            default:{
                message.set_type(NodeManager::EnvironmentMessage::ERROR_RESPONSE);
                break;
            }
        }
    }
    catch(std::exception& ex){
        //TODO: Add ex.what() as error message.
        message.set_type(NodeManager::EnvironmentMessage::ERROR_RESPONSE);
    }

    ZMQSendReply(handler_socket_, message.SerializeAsString());
}

void DeploymentHandler::HandleDirtyExperiment(NodeManager::EnvironmentMessage& message){
    
    message.set_type(NodeManager::EnvironmentMessage::UPDATE_DEPLOYMENT);

    auto update_message = message.mutable_control_message();

    environment_->GetExperimentUpdate(experiment_id_, *update_message);

}
