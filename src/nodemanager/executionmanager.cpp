#include "executionmanager.h"
#include "executionparser/modelparser.h"
#include "environmentmanager/deploymentgenerator.h"
#include "environmentmanager/deploymentrule.h"
#include "environmentmanager/deploymentrules/zmq/zmqrule.h"
#include "environmentmanager/deploymentrules/dds/ddsrule.h"
#include "environmentmanager/deploymentrules/amqp/amqprule.h"
#include "environmentmanager/deploymentrules/tao/taorule.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <unordered_map>

#include <proto/controlmessage/controlmessage.pb.h>

#include <re_common/zmq/protowriter/protowriter.h>
#include <re_common/util/execution.hpp>

#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype>

ExecutionManager::ExecutionManager(const std::string& master_ip_addr,
                                    const std::string& graphml_path,
                                    double execution_duration,
                                    Execution* execution,
                                    const std::string& experiment_id,
                                    const std::string& environment_manager_endpoint){
    if(execution){
        //Setup writer
        proto_writer_ = new zmq::ProtoWriter();

        execution_ = execution;
        execution_->AddTerminateCallback(std::bind(&ExecutionManager::TerminateExecution, this));
    }

    registrar_ = std::unique_ptr<zmq::Registrar>(new zmq::Registrar(*this));
    
    master_ip_addr_ = master_ip_addr;
    experiment_id_ = experiment_id;
    environment_manager_endpoint_ = environment_manager_endpoint;

    auto start = std::chrono::steady_clock::now();
    //Setup the parser
    protobuf_model_parser_ = std::unique_ptr<ProtobufModelParser>(new ProtobufModelParser(graphml_path, experiment_id_));
    deployment_message_ = protobuf_model_parser_->ControlMessage();

    auto master_ip_address = deployment_message_->add_attributes();
    auto master_ip_address_info = master_ip_address->mutable_info();
    master_ip_address_info->set_name("master_ip_address");
    master_ip_address->set_kind(NodeManager::Attribute::STRING);
    master_ip_address->add_s(master_ip_addr_);

    if(!environment_manager_endpoint.empty()){
        requester_ = std::unique_ptr<EnvironmentRequester>(new EnvironmentRequester(environment_manager_endpoint, experiment_id_,
                                                            EnvironmentRequester::DeploymentType::RE_MASTER));
    }

    parse_succeed_ = PopulateDeployment();
    if(parse_succeed_){
        ConstructControlMessages();
    }

    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    if(parse_succeed_){
        std::cout << "* Deployment Parsed In: " << ms.count() << " us" << std::endl;
        proto_writer_->BindPublisherSocket(master_publisher_endpoint_);

        if(parse_succeed_ && execution){
            std::cout << "--------[Slave Registration]--------" << std::endl;
            execution_thread_ = new std::thread(&ExecutionManager::ExecutionLoop, this, execution_duration);
        }
    }else{
        std::cout << "* Deployment Parsing Failed!" << std::endl;
    }
}

std::string ExecutionManager::GetMasterRegistrationEndpoint(){
    return master_registration_endpoint_;
}

bool ExecutionManager::PopulateDeployment(){
    if(local_mode_){
        Environment* environment = new Environment("");

        environment->AddDeployment(deployment_message_->experiment_id(), "", Environment::DeploymentType::EXECUTION_MASTER);

        DeploymentGenerator generator(*environment);
        //TODO: Add other middlewares.
        generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Zmq::DeploymentRule(*environment)));
        generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Dds::DeploymentRule(*environment)));
        generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Amqp::DeploymentRule(*environment)));
        generator.AddDeploymentRule(std::unique_ptr<DeploymentRule>(new Tao::DeploymentRule(*environment)));

        generator.PopulateDeployment(*deployment_message_);
    }
    else{
        requester_->Init(environment_manager_endpoint_);
        requester_->Start();

        std::this_thread::sleep_for(std::chrono::seconds(1));
        NodeManager::ControlMessage response;
        try{
            response = requester_->AddDeployment(*deployment_message_);
            //std::cout << response.DebugString() << std::endl;
        }catch(const std::runtime_error& ex){
            //If anything goes wrong, we've failed to populate our deployment. Return false
            std::cerr << "Failed to populate deployment." << std::endl;
            return false;
        }

        *deployment_message_ = response;

        for(auto& attribute : deployment_message_->attributes()){
            if(attribute.info().name() == "master_publisher_endpoint"){
                master_publisher_endpoint_ = attribute.s(0);
            }
            else if(attribute.info().name() == "master_registration_endpoint"){
                master_registration_endpoint_= attribute.s(0);
            }
        }
    }
    return true;
}

void ExecutionManager::PushMessage(const std::string& topic, google::protobuf::MessageLite* message){
    proto_writer_->PushMessage(topic, message);
}

std::vector<std::string> ExecutionManager::GetSlaveAddresses(){
    std::vector<std::string> slave_addresses;

    for(const auto& ss : slave_states_){
        slave_addresses.push_back(ss.first);
    }
    return slave_addresses;
}

bool ExecutionManager::IsValid(){
    return parse_succeed_;
}

bool ExecutionManager::HandleSlaveResponseMessage(const NodeManager::SlaveStartupResponse& response){
    auto slave_state = SlaveState::ERROR_;

    const auto& slave_ip = response.slave_ip();
    auto slave_hostname = GetSlaveHostName(slave_ip);

    if(response.IsInitialized()){
        slave_state = response.success() ? SlaveState::ONLINE : SlaveState::ERROR_;
    }

    if(slave_state == SlaveState::ERROR_){
        std::cerr << "* Slave: " << slave_hostname << " @ " << slave_ip << " Error!" << std::endl;
        for(const auto& error_str : response.error_codes()){
            std::cerr << "* " << error_str << std::endl;
        }
    }else{
        std::cout << "* Slave: " << slave_hostname << " @ " << slave_ip << " Online" << std::endl;
    }

    if(slave_states_.count(slave_ip)){
        slave_states_[slave_ip] = slave_state;

        if(GetSlaveStateCount(SlaveState::OFFLINE) == 0){
            bool should_execute = GetSlaveStateCount(SlaveState::ERROR_) == 0;
            TriggerExecution(should_execute);
        }
        return true;
    }
    return false;
}

std::string ExecutionManager::GetSlaveHostName(const std::string& slave_ip){
    if(deployment_map_.count(slave_ip)){
        return deployment_map_[slave_ip].info().name();
    }
    return "Unknown Hostname";
}

const NodeManager::SlaveStartup ExecutionManager::GetSlaveStartupMessage(const std::string& slave_ip){
    NodeManager::SlaveStartup startup;
    startup.set_allocated_configure(new NodeManager::ControlMessage(*deployment_message_));

    auto configure = startup.mutable_configure();
    configure->clear_nodes();
    auto node = configure->add_nodes();

    if(deployment_map_.count(slave_ip)){
        *node = deployment_map_[slave_ip];
    }

    auto slave_name = GetSlaveHostName(slave_ip);

    std::string logger_port;
    std::string master_publisher_port;

    for(int i = 0; i < node->attributes_size(); i++){
        auto attribute = node->attributes(i);

        if(attribute.info().name() == "modellogger_port"){
            logger_port = attribute.s(0);
        }
    }

    startup.mutable_logger()->set_mode(NodeManager::Logger::CACHED);
    startup.mutable_logger()->set_publisher_address("tcp://" + slave_ip + ":" + logger_port);

    startup.set_master_publisher_address(master_publisher_endpoint_);
    startup.set_slave_host_name(slave_name);
    return startup;
}

int ExecutionManager::GetSlaveStateCount(const SlaveState& state){
    int count = 0;
    for(const auto& ss : slave_states_){
        if(ss.second == state){
            count ++;
        }
    }
    return count;
}

NodeManager::Attribute_Kind GetAttributeType(const std::string& type){
    if(type == "Integer"){
        return NodeManager::Attribute::INTEGER;
    }else if(type == "Boolean"){
        return NodeManager::Attribute::BOOLEAN;
    }else if(type == "Character"){
        return NodeManager::Attribute::CHARACTER;
    }else if(type == "String"){
        return NodeManager::Attribute::STRING;
    }else if(type == "Double"){
        return NodeManager::Attribute::DOUBLE;
    }else if(type == "Float"){
        return NodeManager::Attribute::FLOAT;
    }
    std::cerr << "Unhandle Graphml Attribute Type: '" << type << "'" << std::endl;
    return NodeManager::Attribute::STRING;
}

bool ExecutionManager::ConstructControlMessages(){
    std::unique_lock<std::mutex>(mutex_);
    std::cout << "------------[Deployment]------------" << std::endl; 

    for(int i = 0; i<deployment_message_->nodes_size(); i++){
        ConfigureNode(deployment_message_->nodes(i));
    }

    return true;
}

void ExecutionManager::ConfigureNode(const NodeManager::Node& node){

    std::string ip_address;

    for(int i = 0; i<node.nodes_size(); i++){
        ConfigureNode(node.nodes(i));
    }

    for(int i = 0; i < node.attributes_size(); i++){
        auto attribute = node.attributes(i);
        if(attribute.info().name() == "ip_address"){
            ip_address = attribute.s(0);
        }
    }

    if(node.components_size() > 0){
        std::cout << "* Slave: '" << node.info().name() << "' Deploys:" << std::endl;

        slave_states_[ip_address] = SlaveState::OFFLINE;
        
        for(int i = 0; i < node.components_size(); i++){
            std::cout << "** " << node.components(i).info().name() << " [" << node.components(i).info().type() << "]" << std::endl;
        }
    }
    //TODO: Add fail cases
    deployment_map_[ip_address] = node;
}

void ExecutionManager::TriggerExecution(bool execute){
    //Obtain lock
    std::unique_lock<std::mutex> lock(execution_mutex_);
    terminate_flag_ = !execute;
    //Notify
    execution_lock_condition_.notify_all();
}

void ExecutionManager::TerminateExecution(){
    //Interupt
    TriggerExecution(false);
    if(execution_thread_){
        //Notify
        execution_thread_->join();
    }
}

void ExecutionManager::ExecutionLoop(double duration_sec) noexcept{
    auto execution_duration = std::chrono::duration<double>(duration_sec);
    
    bool execute = true;
    {
        //Wait to be executed or terminated
        std::unique_lock<std::mutex> lock(execution_mutex_);
        execution_lock_condition_.wait(lock);
        if(terminate_flag_){
            execute = false;
        }
    }

    std::cout << "-------------[Execution]------------" << std::endl;

    if(execute){
        std::cout << "* Activating Deployment" << std::endl;
        
        //Send Activate function
        auto activate = new NodeManager::ControlMessage();
        activate->set_type(NodeManager::ControlMessage::ACTIVATE);
        PushMessage("*", activate);

        {
            std::unique_lock<std::mutex> lock(execution_mutex_);
            if(duration_sec == -1){
                //Wait indefinately
                execution_lock_condition_.wait(lock, [this]{return this->terminate_flag_;});
            }else{
                execution_lock_condition_.wait_for(lock, execution_duration, [this]{return this->terminate_flag_;});
            }
        }

        std::cout << "* Passivating Deployment" << std::endl;
        //Send Terminate Function
        auto passivate = new NodeManager::ControlMessage();
        passivate->set_type(NodeManager::ControlMessage::PASSIVATE);
        PushMessage("*", passivate);
    }
    
    std::cout << "* Terminating Deployment" << std::endl;
    //Send Terminate Function
    auto terminate = new NodeManager::ControlMessage();
    terminate->set_type(NodeManager::ControlMessage::TERMINATE);
    PushMessage("*", terminate);

    if(!local_mode_){
        requester_->RemoveDeployment();
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
    execution_->Interrupt();
}
