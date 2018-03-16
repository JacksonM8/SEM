#include <iostream>
#include <future>
#include <thread>
#include <signal.h>

#include <middleware/tao/helper.h>
#include <nodemanager/execution.hpp>

#include "global.h"
#include "server.h"


Execution* exe = 0;

std::mutex global_mutex_;

void signal_handler(int sig)
{
    std::lock_guard<std::mutex> lock(global_mutex_);
    exe->Interrupt();
}

void print_message(const Test::Message& message){
    std::lock_guard<std::mutex> lock(global_mutex_);
    std::cout << "\ttime: " << message.time << std::endl;
    std::cout << "\tinst_name: " << message.inst_name << std::endl;
    std::cout << "\tBlocking" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "\tDone" << std::endl;
}

void Hello::send(const Test::Message& message){
    std::cout << "GOT send: " << std::endl;
    print_message(message);
}

void Hello::send22(const Test::Message& message){
    std::cout << "GOT send22: " << std::endl;
    print_message(message);
    if(message.time % 2){
        auto exception = Test::Hello::TestException("Sending One Heck Exception");
        throw exception;
    }
}

int main(int argc, char ** argv){
    signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
    exe = new Execution();

    int i = 5;
    while(i--){
        std::cerr << std::endl << "===== COUNT: " << i << "=====" << std::endl;
        auto& helper = tao::TaoHelper::get_tao_helper();
        {
            auto orb = helper.get_orb("iiop://" + sender_orb_endpoint);
            if(!orb){
                std::cerr << "No Valid Orb" << std::endl;
                continue;
            }
            
            std::string Sender1("Sender1");
            
            // Create the child POA for the test logger factory servants.

            auto sender1_poa = helper.get_poa(orb, Sender1);
            auto sender1_impl = new Hello();
                
            helper.register_servant(orb, sender1_poa, sender1_impl, Sender1);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            helper.deregister_servant(orb, sender1_poa, sender1_impl, Sender1);
        }
        
    }
    delete exe;
    return 0;
}