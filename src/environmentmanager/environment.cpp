#include "environment.h"
#include <iostream>
#include <cassert>
#include <queue>

Environment::Environment(int port_range_min, int port_range_max){
    PORT_RANGE_MIN = port_range_min;
    PORT_RANGE_MAX = port_range_max;

    MANAGER_PORT_RANGE_MIN = port_range_min + 10000;
    MANAGER_PORT_RANGE_MAX = port_range_max + 10000;

    //Bail out if ranges are illegal
    assert(PORT_RANGE_MIN < PORT_RANGE_MAX);
    assert(MANAGER_PORT_RANGE_MIN < MANAGER_PORT_RANGE_MAX);

    //Populate range sets
    auto hint_iterator = available_ports_.begin();
    for(int i = PORT_RANGE_MIN; i <= PORT_RANGE_MAX; i++){
        available_ports_.insert(hint_iterator, i);
        hint_iterator++;
    }

    auto hint_iterator_manager = available_node_manager_ports_.begin();
    for(int i = MANAGER_PORT_RANGE_MIN; i < MANAGER_PORT_RANGE_MAX; i++){
        available_node_manager_ports_.insert(hint_iterator_manager, i);
        hint_iterator_manager++;
    }
    clock_ = 0;
}

std::string Environment::AddExperiment(const std::string& model_name){
    if(experiment_map_.count(model_name)){
        throw std::invalid_argument("");
    }
    experiment_map_[model_name] = new Environment::Experiment(model_name);
    auto experiment = experiment_map_[model_name];

    experiment->manager_port_ = GetManagerPort();

    std::cout << "Added experiment: " << model_name << std::endl;
    std::cout << "Registered experiments: " << experiment_map_.size() << std::endl;

    return experiment->manager_port_;
}

void Environment::RemoveExperiment(const std::string& model_name, uint64_t time_called){
    //go through experiment and free all ports used.
    try{
        auto experiment = experiment_map_[model_name];
        for(const auto& port : experiment->port_map_){
            auto event_port = port.second;
            auto name = experiment->node_map_[event_port.node_id]->info().name();
            auto port_number = event_port.port_number;
            FreePort(name, port_number);
        }
        for(const auto& node : experiment->node_map_){
            auto node_struct = node.second;
            auto name = node_struct->info().name();

            if(experiment->management_port_map_.count(node.first)){
                auto management_port = experiment->management_port_map_[node.first];
                FreePort(name, management_port);

            }
            if(experiment->modellogger_port_map_.count(node.first)){
                auto modellogger_port = experiment->modellogger_port_map_[node.first];
                FreePort(name, modellogger_port);
            }
        }
        FreeManagerPort(experiment->manager_port_);
        std::string master_node_name = experiment->node_map_[experiment->node_id_map_[experiment->master_ip_address_]]->info().name();
        FreePort(master_node_name, experiment->master_port_);
        delete experiment;
        experiment_map_.erase(model_name);
        std::cout << "Removed experiment: " << model_name << std::endl;
    }
    catch(...){
        std::cout << "Could not delete deployment :" << model_name << std::endl;
    }

    std::cout << "Registered experiments: " << experiment_map_.size() << std::endl;
}

void Environment::StoreControlMessage(const NodeManager::ControlMessage& control_message){
    std::string experiment_name = control_message.experiment_id();
    if(experiment_name.empty()){
        return;
    }

    if(!experiment_map_.count(experiment_name)){
        return;
    }

    experiment_map_.at(experiment_name)->deployment_message_ = control_message;

}

void Environment::DeclusterExperiment(NodeManager::ControlMessage& message){
    for(int i = 0; i < message.nodes_size(); i++){
        auto node = message.mutable_nodes(i);
        DeclusterNode(*node);
    }
}

void Environment::DeclusterNode(NodeManager::Node& node){
    //TODO: change this to have 2nd mode for distribution (reflecting components running on nodes from other experiments)
    if(node.type() == NodeManager::Node::HARDWARE_CLUSTER || node.type() == NodeManager::Node::DOCKER_CLUSTER){
        std::queue<NodeManager::Component> component_queue;
        for(int i = 0; i < node.components_size(); i++){
            component_queue.push(NodeManager::Component(node.components(i)));
        }
        node.clear_components();

        int child_node_count = node.nodes_size();
        int counter = 0;

        while(!component_queue.empty()){
            auto component = component_queue.front();
            component_queue.pop();

            auto new_component = node.mutable_nodes(counter % child_node_count)->add_components();
            *new_component = component;
            counter++;
        }
    }
    for(int i = 0; i < node.nodes_size(); i++){
        DeclusterNode(*(node.mutable_nodes(i)));
    }
}

void Environment::AddNodeToExperiment(const std::string& model_name, const NodeManager::Node& node){
    Experiment* experiment;
    if(experiment_map_.count(model_name)){
        experiment = experiment_map_[model_name];
    }
    else{
        throw std::invalid_argument("No experiment called: " + model_name);
    }

    AddNodeToEnvironment(node);

    experiment->node_map_[node.info().id()] = new NodeManager::Node(node);
    experiment->deployment_map_[experiment->node_address_map_[node.info().id()]] = 0;
    for(int i = 0; i < node.attributes_size(); i++){
        auto attribute = node.attributes(i);
        if(attribute.info().name() == "ip_address"){
            experiment->node_address_map_[node.info().id()] = attribute.s(0);
            experiment->node_id_map_[attribute.s(0)] = node.info().id();
            break;
        }
    }

    for(int i = 0; i < node.components_size(); i++){
        auto component = node.components(i);
        for(int j = 0; j < component.ports_size(); j++){
            auto port = component.ports(j);
            
            EventPort event_port;
            event_port.id = port.info().id();
            event_port.guid = port.port_guid();
            event_port.port_number = GetPort(node.info().name());
            event_port.node_id = node.info().id();

            //TODO: Fix this when namespace/scoping is into medea, also need to fix in protobufmodelparser
            event_port.type = port.namespace_name() + ":" + port.info().type();

            for(int a = 0; a < port.attributes_size(); a++){
                auto attribute = port.attributes(a);
                if(attribute.info().name() == "topic"){
                    event_port.topic = attribute.s(0);
                    experiment->topic_set_.insert(event_port.topic);
                    break;
                }
            }

            for(int k = 0; k < port.connected_ports_size(); k++){
                auto id = port.connected_ports(k);
                experiment->connection_map_[id].push_back(event_port.id);
            }

            if(port.visibility() == NodeManager::EventPort::PUBLIC){
                public_event_port_map_[port.port_guid()] = EventPort(event_port);
            }

            experiment->port_map_[event_port.id] = event_port;
        }
        experiment->deployment_map_[experiment->node_address_map_[node.info().id()]]++;
    }
}

void Environment::ConfigureNode(const std::string& model_name, NodeManager::Node& node){

    std::string node_name = node.info().name();

    if(node.components_size() > 0){
        //set modellogger port
        auto logger_port = GetPort(node_name);

        auto logger_attribute = node.add_attributes();
        auto logger_attribute_info = logger_attribute->mutable_info();
        logger_attribute->set_kind(NodeManager::Attribute::STRING);
        logger_attribute_info->set_name("modellogger_port");
        logger_attribute->add_s(logger_port);


        auto hardwarelogger_port = GetPort(node_name);

        auto hardwarelogger_attribute = node.add_attributes();
        auto hardwarelogger_attribute_info = hardwarelogger_attribute->mutable_info();
        hardwarelogger_attribute->set_kind(NodeManager::Attribute::STRING);
        hardwarelogger_attribute_info->set_name("hardwarelogger_port");
        hardwarelogger_attribute->add_s(hardwarelogger_port);

        //set master/slave port
        auto management_port = GetPort(node_name);

        auto management_endpoint_attribute = node.add_attributes();
        auto management_endpoint_attribute_info = management_endpoint_attribute->mutable_info();
        management_endpoint_attribute->set_kind(NodeManager::Attribute::STRING);
        management_endpoint_attribute_info->set_name("management_port");
        management_endpoint_attribute->add_s(management_port);

        experiment_map_[model_name]->modellogger_port_map_[node.info().id()] = logger_port;
        experiment_map_[model_name]->hardwarelogger_port_map_[node.info().id()] = hardwarelogger_port;
        experiment_map_[model_name]->management_port_map_[node.info().id()] = management_port;
    }

    experiment_map_[model_name]->node_map_[node.info().id()] = new NodeManager::Node(node);
}

std::vector<std::string> Environment::GetPublisherAddress(const std::string& model_name, const NodeManager::EventPort& port){
    std::vector<std::string> publisher_addresses;

    Experiment* experiment;

    if(experiment_map_.count(model_name)){
        experiment = experiment_map_[model_name];
    }else{
        throw std::invalid_argument("Model named :" + model_name + " not found.");
    }

    std::unique_lock<std::mutex> lock(experiment->mutex_);
    

    if(port.kind() == NodeManager::EventPort::IN_PORT){

        //Get list of connected ports
        auto publisher_port_ids = experiment->connection_map_[port.info().id()];

        //Get those ports addresses
        for(auto id : publisher_port_ids){

            //XXX: Ugly hack to differentiate between local connected ports and public connected ports.
            //ID of public connected ports is set to be port guid, which is not parseable as int
            bool local_port = true;
            try{
                std::stoi(id);
            }
            catch(const std::invalid_argument& ex){
                local_port = false;
            }

            if(local_port){
                auto node_id = experiment->port_map_[id].node_id;
                auto port_assigned_port = experiment->port_map_[id].port_number;
                publisher_addresses.push_back("tcp://" + experiment->node_address_map_[node_id] + ":" + port_assigned_port);
            }
            else{
                if(public_event_port_map_.count(id)){
                    publisher_addresses.push_back(public_event_port_map_.at(id).endpoint);
                }
                //We don't have this public port's address yet. The experiment it orinates from is most likely not started yet.
                //We need to keep track of the fact that this experiment is waiting for this port to become live so we can notify it of the environment change.
                else{
                    pending_port_map_[id].insert(model_name);
                }
            }
        }
    }

    else if(port.kind() == NodeManager::EventPort::OUT_PORT){
        auto node_id = experiment->port_map_[port.info().id()].node_id;
        auto port_assigned_port = experiment->port_map_[port.info().id()].port_number;

        std::string addr_string = "tcp://" + experiment->node_address_map_[node_id] + ":" + port_assigned_port;

        publisher_addresses.push_back(addr_string);

        if(port.visibility() == NodeManager::EventPort::PUBLIC){
            public_event_port_map_[port.port_guid()].endpoint = addr_string;

            if(pending_port_map_.count(port.port_guid())){
                auto set = pending_port_map_.at(port.port_guid());

                for(auto experiment_id : set){
                    experiment_map_[experiment_id]->dirty_flag = true;
                    experiment_map_[experiment_id]->updated_port_ids_.insert(port.port_guid());
                }
                pending_port_map_.erase(port.port_guid());
            }
        }
    }
    return publisher_addresses;
}

std::string Environment::GetTopic(const std::string& model_name, const std::string& port_id){
    auto port = experiment_map_[model_name]->port_map_[port_id];
}

//Return list of experiments using the topic name provided
std::vector<std::string> Environment::CheckTopic(const std::string& model_name, const std::string& topic){
    std::vector<std::string> out;
    for(auto experiment_pair : experiment_map_){
        auto experiment = experiment_pair.second;
        for(auto check_topic : experiment->topic_set_){
            if(topic == check_topic){
                out.push_back(experiment->model_name_);
            }
        }
    }
    return out;
}

void Environment::AddNodeToEnvironment(const NodeManager::Node& node){
    //Bail out early
    if(node_map_.count(node.info().name())){
        return;
    }
    auto new_node = new Node(node.info().name(), available_ports_);
    for(int i = 0; i < node.attributes_size(); i++){
        auto attribute = node.attributes(i);
        if(attribute.info().name() == "ip_address"){
            new_node->ip = attribute.s(0);
            break;
        }
    }
    node_map_[new_node->name] = new_node;
}

//Get port from node specified.
std::string Environment::GetPort(const std::string& node_name){
    //Get first available port, store then erase it
    Node* node;
    if(node_map_.count(node_name)){
        node = node_map_[node_name];
    }
    else{
        throw std::invalid_argument("No node found with name: " + node_name);
    }
    return node->GetPort();
}

//Free port specified from node specified
void Environment::FreePort(const std::string& node_name, const std::string& port_number){

    Node* node;
    if(node_map_.count(node_name)){
        node = node_map_[node_name];
    }
    else{
        throw std::invalid_argument("No node found with name: " + node_name);
    }
    node->FreePort(port_number);
}

std::string Environment::GetManagerPort(){
    std::unique_lock<std::mutex> lock(port_mutex_);

    if(available_node_manager_ports_.empty()){
        return "";
    }
    auto it = available_node_manager_ports_.begin();
    auto port = *it;
    available_node_manager_ports_.erase(it);
    auto port_str =  std::to_string(port);
    return port_str;
}

void Environment::FreeManagerPort(const std::string& port){
    std::unique_lock<std::mutex> lock(port_mutex_);
    int port_number = std::stoi(port);
    available_node_manager_ports_.insert(port_number);
}

bool Environment::NodeDeployedTo(const std::string& model_name, const std::string& ip_address) const{
    if(experiment_map_.count(model_name)){
        auto experiment = experiment_map_.at(model_name);
        try{
            bool out = experiment->deployment_map_.at(ip_address) > 0;
            return out;
        }catch(...){
            return false;
        }
    }
    return false;
}

bool Environment::ModelNameExists(const std::string& model_name) const{
    return experiment_map_.count(model_name);
}

std::string Environment::GetMasterPublisherPort(const std::string& model_name, const std::string& master_ip_address){
    if(experiment_map_.count(model_name)){
        auto experiment = experiment_map_.at(model_name);
        experiment->master_ip_address_ = master_ip_address;
        std::string node_name = experiment->node_map_[experiment->node_id_map_[master_ip_address]]->info().name();
        experiment->master_port_ = GetPort(node_name);
        return experiment->master_port_;
    }
    return "";
}

std::string Environment::GetNodeManagementPort(const std::string& model_name, const std::string& ip_address){
    if(experiment_map_.count(model_name)){
        auto experiment = experiment_map_.at(model_name);
        std::string node_id = experiment->node_id_map_.at(ip_address);
        return experiment->management_port_map_.at(node_id);
    }
    return "";
}
std::string Environment::GetNodeModelLoggerPort(const std::string& model_name, const std::string& ip_address){
    if(experiment_map_.count(model_name)){
        auto experiment = experiment_map_.at(model_name);
        std::string node_id = experiment->node_id_map_.at(ip_address);
        return experiment->modellogger_port_map_.at(node_id);
    }
    return "";
}
std::string Environment::GetNodeHardwareLoggerPort(const std::string& model_name, const std::string& ip_address){
    if(experiment_map_.count(model_name)){
        auto experiment = experiment_map_.at(model_name);
        std::string node_id = experiment->node_id_map_.at(ip_address);
        return experiment->hardwarelogger_port_map_.at(node_id);
    }
    return "";
}

bool Environment::ExperimentIsDirty(const std::string& model_name){
    try{
        return experiment_map_.at(model_name)->dirty_flag;
    }catch(const std::out_of_range& ex){
        throw std::invalid_argument("Model named " + model_name + " not found in Environment::ExperimentIsDirty.");
    }
}

void Environment::GetExperimentUpdate(const std::string& model_name, NodeManager::ControlMessage& control_message){
    try{
        auto experiment = experiment_map_.at(model_name);
        std::unique_lock<std::mutex> lock(experiment->mutex_);
        experiment->dirty_flag = false;

        auto updated_ports = experiment->updated_port_ids_;

        while(!experiment->updated_port_ids_.empty()){
            auto port_it = experiment->updated_port_ids_.begin();
            auto port_id = *port_it;
            experiment->updated_port_ids_.erase(port_it);

            //XXX: Jeez, this is pretty terrible...
            for(int i = 0; i < control_message.nodes_size(); i++){
                auto node = control_message.nodes(i);
                for(int j = 0; j < node.components_size(); j++){
                    auto component = node.components(j);
                    for(int k = 0; k < component.ports_size(); k++){
                        auto port = component.mutable_ports(k);
                        for(int l = 0; l < port->connected_ports_size(); l++){
                            auto connected_port = port->connected_ports(l);
                            if(port_id == connected_port){
                                try{
                                    std::string endpoint = public_event_port_map_.at(port_id).endpoint;

                                    for(int m = 0; m < port->attributes_size(); m++){
                                        auto attribute = port->mutable_attributes(i);
                                        if(attribute->info().name() == "publisher_address"){
                                            attribute->add_s(endpoint);
                                        }
                                    }
                                }
                                catch(const std::out_of_range& ex){
                                    throw std::runtime_error("Updated public ports + master public ports map mismatch.");
                                }
                            }
                        }
                    }
                }
            }
        }

    }catch(const std::out_of_range& ex){
        throw std::invalid_argument("Model named " + model_name + " not found in Environment::GetExperimentUpdate.");
    }
}

void Environment::ExperimentLive(const std::string& model_name, uint64_t time_called){
    try{
        auto experiment = experiment_map_.at(model_name);
        if(time_called >= experiment->time_added){
            experiment->state = ExperimentState::ACTIVE;
        }
    }
    catch(std::out_of_range& ex){
        std::cerr << "Tried to live non existant experiment: " << model_name << std::endl;
    }
}

void Environment::ExperimentTimeout(const std::string& model_name, uint64_t time_called){
    try{
        auto experiment = experiment_map_.at(model_name);
        if(time_called >= experiment->time_added){
            experiment->state = ExperimentState::TIMEOUT;
        }
    }
    catch(std::out_of_range& ex){
        std::cerr << "Tried to timeout non existant deployment: " << model_name << std::endl;
    }
}

uint64_t Environment::GetClock(){
    std::unique_lock<std::mutex> lock(clock_mutex_);
    return clock_;
}

uint64_t Environment::SetClock(uint64_t incoming){
    std::unique_lock<std::mutex> lock(clock_mutex_);
    clock_ = std::max(incoming, clock_) + 1;
    return clock_;
}

uint64_t Environment::Tick(){
    std::unique_lock<std::mutex> lock(clock_mutex_);
    clock_++;
    return clock_;
}
