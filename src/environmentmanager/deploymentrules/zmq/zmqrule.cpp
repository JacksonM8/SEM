#include "zmqrule.h"
#include <iostream>
Zmq::DeploymentRule::DeploymentRule(Environment& environment) : ::DeploymentRule(::DeploymentRule::MiddlewareType::ZMQ, environment){
}

void Zmq::DeploymentRule::ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::EventPort& event_port){

    //set publisher_address
    auto publisher_address_attr = event_port.add_attributes();
    auto publisher_address_attr_info = publisher_address_attr->mutable_info();
    publisher_address_attr_info->set_name("publisher_address");
    publisher_address_attr->set_kind(NodeManager::Attribute::STRINGLIST);

    //find deployed hardware node
    if(event_port.kind() == NodeManager::EventPort::OUT_PORT){
        auto publisher_address = environment_.GetPublisherAddress(message.model_name(), event_port);
        publisher_address_attr->add_s(publisher_address.at(0));
    }

    //If we're an in port, add all publisher endpoints
    else if(event_port.kind() == NodeManager::EventPort::IN_PORT){
        auto connected_addresses = environment_.GetPublisherAddress(message.model_name(), event_port);
        for(auto id : connected_addresses){
            publisher_address_attr->add_s(id);
        }
    }
}
void Zmq::DeploymentRule::TerminateEventPort(const NodeManager::ControlMessage& message, NodeManager::EventPort& event_port){

}

//Returns id of node port is deployed to
std::string Zmq::DeploymentRule::GetDeploymentLocation(const NodeManager::ControlMessage& message,
                                                                const std::string& port_id){
    //find node eventport is deployed to, can include clusters

    std::string out("");

    for(int i = 0; i < message.nodes_size(); i++){
        out = CheckNode(message.nodes(i), port_id);
        if(!out.empty()){
            break;
        }
    }
    return out;
}

std::string Zmq::DeploymentRule::CheckNode(const NodeManager::Node& node, const std::string& port_id){
    
    std::string out("");

    //check node's children for deployed port_id
    for(int i = 0; i < node.nodes_size(); i++){
        out = CheckNode(node.nodes(i), port_id);
        if(!out.empty()){
            break;
        }
    }

    //port id not contained in any child nodes, look in ourself
    if(out.empty()){
        for(int i = 0; i < node.components_size(); i++){
            auto component = node.components(i);
            for(int j = 0; j < component.ports_size(); j++){
                auto port = component.ports(j);
                if(port.info().id() == port_id){
                    out = node.info().id();
                }
            }
        }
    }

    return out;
}
