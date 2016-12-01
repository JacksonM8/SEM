#include <iostream>
#include <chrono>
#include <thread>

#include "interfaces.h"
#include "senderimpl.h"
#include "recieverimpl.h"

//#include "test/testtxmessage.h"
//#include "test/testrxmessage.h"
//#include "zmq/zmqrxmessage.h"
//#include "zmq/zmqtxmessage.h"
#include "dds/ddstxmessage.h"
#include "dds/ddsrxmessage.h"
#include "dds/message.hpp"

#include <dds/dds.hpp>

#include "zmq.hpp"




int main(int argc, char** argv){
    dds::domain::DomainParticipant participant(0);
    dds::topic::Topic<test_dds::Message> * topic = new dds::topic::Topic<test_dds::Message>(participant, "Test_topic");    
    dds::pub::Publisher* publisher = new dds::pub::Publisher(participant);
    dds::sub::Subscriber* subscriber = new dds::sub::Subscriber(participant); 




    SenderImpl* sender_impl = new SenderImpl();
    RecieverImpl* reciever_impl = new RecieverImpl();
    
    //Construct Ports
    rxMessageInt* rxMessage = new dds_rxMessage(reciever_impl, subscriber, topic);
    txMessageInt* txMessage = new dds_txMessage(sender_impl, publisher, topic);

    //zmq::context_t * context = new zmq::context_t(1);
    //txMessageInt* txMessage = new zmq_txMessage(sender_impl, context, std::string("tcp://*:6000"));

    //rxMessageInt* rxMessage = new zmq_rxMessage(reciever_impl, context, std::string("tcp://192.168.111.187:6000"));
    
    //Attach Ports
    sender_impl->txMessage_ = txMessage;
    reciever_impl->rxMessage_ = rxMessage;

    sender_impl->set_instName("SenderImpl");
    sender_impl->set_message("Hello, World!");


    int i = 60;
    while(i-- > 0){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        sender_impl->periodic_event();
    }

    return -1;
}
