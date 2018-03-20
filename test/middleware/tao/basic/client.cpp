#include <iostream>
#include <future>
#include <unordered_map>
#include <set>

#include <middleware/tao/helper.h>

#include "global.h"
#include "message_clientS.h"


bool interupt = false;

void signal_handler(int sig)
{
    interupt = true;
}

int main(int argc, char** argv){
    signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

    auto& helper = tao::TaoHelper::get_tao_helper();
    auto orb = helper.get_orb("iiop://192.168.111.90:50000");
    
    if(!orb){
        std::cerr << "No Valid Orb" << std::endl;
        return -1;
    }

    
    std::string server_addr("corbaloc:iiop:" + sender_orb_endpoint);

    std::string sender_1_name("Sender1");
    std::string sender_2_name("Sender2");

    std::string sender_1_addr(server_addr + "/" + sender_1_name);
    std::string sender_2_addr(server_addr + "/" + sender_2_name);

    helper.register_initial_reference(orb, sender_1_name, sender_1_addr);
    helper.register_initial_reference(orb, sender_2_name, sender_2_addr);

    std::set<std::string> unregistered_references;
    std::set<std::string> registered_references;

    unregistered_references.insert(sender_1_name);
    unregistered_references.insert(sender_2_name);

    std::unordered_map<std::string, CORBA::Object_ptr> object_ptrs;
    std::unordered_map<std::string, Test::Hello_ptr> senders;


    Test::Message2 message;
    message.inst_name2 = "=D";
    message.time2 = argc;

   
    

    while(!interupt){
        for (auto itt = unregistered_references.begin(); itt != unregistered_references.end();) {
            auto reference_str = *itt;
            try{
                auto ptr = helper.resolve_initial_references(orb, reference_str);
                if(ptr){
                    object_ptrs[reference_str] = ptr;
                    senders[reference_str] = Test::Hello::_narrow(ptr);
                    registered_references.insert(reference_str);
	                itt = unregistered_references.erase(itt);
                    std::cout << "Registered: " << reference_str << std::endl;
                }
            }catch(...){
                ++itt;
            }
        }


        for (auto itt = registered_references.begin(); itt != registered_references.end();) {
            auto reference_str = *itt;
            bool deregister = false;
            try{
                std::cout << "SENDING A HECK" << std::endl;
                message.time2++;

                auto fill_size = sizeof(message.long_bois) / sizeof(std::remove_extent<decltype(message.long_bois)>::type);

                //message.long_bois.length(message.time2 > sizeof(message.long_bois.maximum()) ? sizeof(message.long_bois.maximum()) : message.time2);
                std::cout << fill_size << std::endl;

                for(int i = 0; i < fill_size; i++){
                    message.long_bois[i] = i;
                }

                if(message.time2 % 3){
                    message.test.enum_short(10);
                }else if(message.time2 % 2){
                    message.test.enum_string("test string");
                }else{
                    message.test.enum_long(20);
                }

                //std::cout << "Sending: " << registered_reference << std::endl;
                auto sender = senders[reference_str];
                if(sender){
                    sender->send22(message);
                }
                ++itt;

            }catch(const CORBA::TRANSIENT& e){
                deregister = true;
            }catch(const CORBA::COMM_FAILURE& e){
                deregister = true;
            }catch(const CORBA::OBJECT_NOT_EXIST& e){
                deregister = true;
            }catch(const CORBA::MARSHAL& e) {
                std::cerr << "GOT INVALID DATA" << std::endl;
            }catch(const CORBA::Exception& e) {
                std::cout << e._rep_id() << std::endl;
                std::cout << e._name() << std::endl;
            }

            if(deregister){
                std::cout << "Ref Ded: " << reference_str << std::endl;
                object_ptrs[reference_str]->_remove_ref();
                CORBA::release(object_ptrs[reference_str]);
                //CrypImpl->_remove_ref();
                itt = registered_references.erase(itt);
                unregistered_references.insert(reference_str);
            }

            //Need to handle
            //SERVER DYING
            //IDL:omg.org/CORBA/TRANSIENT:1.0 | //TRANSIENT
            //IDL:omg.org/CORBA/COMM_FAILURE:1.0 | COMM_FAILUTRE
            
            //PORT INUSE
            //IDL:omg.org/CORBA/BAD_PARAM:1.0 | //BAD_PARAM
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return 0;
}