#include "messageS.h"
#include "../../../base/basic.hpp"
#include <core/ports/libportexport.h>
#define TAO_SERVER_FUNC_NAME send
#include <middleware/tao/requestreply/replierport.hpp>
#include <middleware/tao/requestreply/requesterport.hpp>

const std::string ns_addr("192.168.111.82");
const std::string ns_port("4355");



const std::string ip_addr("192.168.111.82");
const std::string replier_addr(ip_addr + ":5002");
const std::string requester_addr(ip_addr + ":5010");

const std::string replier_orb_addr("iiop://" + replier_addr);
const std::string requester_orb_addr("iiop://" + requester_addr);

const std::string replier_connect_addr("corbaloc:iiop:" + replier_addr);
const std::string ns_connect_address("corbaloc:iiop:" + ns_addr + ":" + ns_port);


bool setup_port(Port& port, const std::string& orb_address, const std::string& name_server_endpoint, const std::vector<std::string>& server_name){
    auto orb_attr = port.GetAttribute("orb_endpoint").lock();
	auto ns_attr = port.GetAttribute("naming_service_endpoint").lock();
	auto sn_attr = port.GetAttribute("server_name").lock();
	auto sk_attr = port.GetAttribute("server_kind").lock();
	if(orb_attr && ns_attr && sn_attr && sk_attr){
		orb_attr->set_String(orb_address);
		ns_attr->set_String(name_server_endpoint);
		sn_attr->set_StringList(server_name);
        sk_attr->set_String("");
        return true;
    }
	return false;
}

Base::Basic2 Callback(Base::Basic2& message){
    //std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    //message.int_val *= 10;
    return message;
};

int main(int, char**){
    //Define the base types
    using base_request_type = Base::Basic2;
    using base_reply_type = Base::Basic2;
    
    //Define the proto types
    using mw_reply_type = ::Basic2;
    using mw_request_type = ::Basic2;

    using mw_reply_server_type = ::POA_SequenceTest;
    using mw_reply_client_type = ::SequenceTest;

    const auto test_name = std::string("Hello");
    const auto req_name = "rq_" + test_name;
    const auto rep_name = "rp_" + test_name;

    auto component = std::make_shared<Component>("c_" + test_name);
    component->RegisterCallback<base_reply_type, base_request_type>(rep_name, Callback);
    auto requester_port = ConstructRequesterPort<tao::RequesterPort<base_reply_type, mw_reply_type, base_request_type, mw_request_type, mw_reply_client_type>>(req_name, component);

    
    setup_port(*requester_port, requester_orb_addr, ns_connect_address, {"TEST", "POOPYFACETOMATONOSE", rep_name});

    std::cerr << (requester_port->Configure() ? "Configured" : "FAILED") << std::endl;
    std::cerr << (requester_port->Activate() ? "Activated" : "FAILED") << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    for(int i = 0 ; i < 1000; i++){
        Base::Basic2 b;
        //b.int_val = i;
        b.str_val = std::to_string(i);

        for(int j = 0 ; j < i; j++){
            b.str_vals.emplace_back("J: " + std::to_string(j));
        }
        std::cerr << "TX: " << i << std::endl;
        auto c = requester_port->SendRequest(b, std::chrono::milliseconds(250));
        if(c.first){
            std::cerr << "GOT RX: " << i << std::endl;
            //std::cerr << c.second.int_val << std::endl;
            std::cerr << c.second.str_val << std::endl;
        }else{
            std::cerr << "CAN'T SEND" << std::endl;
            i--;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cerr << (requester_port->Passivate() ? "Passivated" : "FAILED") << std::endl;
    std::cerr << (requester_port->Terminate() ? "Terminated" : "FAILED") << std::endl;
    delete requester_port;
    return 0;
}


