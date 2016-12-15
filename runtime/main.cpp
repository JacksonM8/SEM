#include <iostream>
#include <chrono>
#include <thread>
#include <iostream>


#include "interfaces.h"
#include "senderimpl.h"
#include "receiverimpl.h"

//RTI DDS
#include "rti/rtitxmessage.h"
#include "rti/rtirxmessage.h"

//OPENSPLICE
#include "opensplice/osplrxmessage.h"
#include "opensplice/ospltxmessage.h"

//ZMQ
#include "zmq/zmqrxmessage.h"
#include "zmq/zmqtxmessage.h"

//QPID
#include "qpid/qpidrxmessage.h"
#include "qpid/qpidtxmessage.h"

int main(int argc, char** argv){
    
    SenderImpl* sender_impl = new SenderImpl();
    SenderImpl* sender_impl2 = new SenderImpl();
    SenderImpl* sender_impl3 = new SenderImpl();
    SenderImpl* sender_impl4 = new SenderImpl();

    ReceiverImpl* receiver_impl = new ReceiverImpl();
    ReceiverImpl* receiver_impl2 = new ReceiverImpl();
    ReceiverImpl* receiver_impl3 = new ReceiverImpl();
    ReceiverImpl* receiver_impl4 = new ReceiverImpl();


    sender_impl->set_instName("RTI_SENDER");
    sender_impl2->set_instName("OSPL_SENDER");
    sender_impl3->set_instName("ZMQ_SENDER");
    sender_impl4->set_instName("QPID_SENDER");
    
    sender_impl->set_message("YO");
    sender_impl2->set_message("YO");
    sender_impl3->set_message("YO");
    sender_impl4->set_message("YO");
    
    receiver_impl->set_instName("RTI_receiveR");
    receiver_impl2->set_instName("OSPL_receiveR");
    receiver_impl3->set_instName("zmq_receiveR");
    receiver_impl4->set_instName("qpid_receiveR");
    
    std::string topic_name("Topic1");
    std::string topic_name2("Topic2");

    std::string pub_name("pub");
    std::string sub_name("sub");
    std::string writer_name("writer");
    std::string reader_name("reader");

    txMessageInt* rti_tx  = 0;
    rxMessageInt* rti_rx  = 0;
    txMessageInt* ospl_tx = 0;
    rxMessageInt* ospl_rx = 0;
    txMessageInt* qpid_tx = 0;
    rxMessageInt* qpid_rx = 0;
    txMessageInt* zmq_tx = 0;
    rxMessageInt* zmq_rx = 0;

    //RTI DDS
    rti_tx = new rti::TxMessage(sender_impl, 0, pub_name, topic_name);
    rti_rx = new rti::RxMessage(receiver_impl, 0, sub_name, topic_name);

    //OpenSplice DDS
    ospl_tx = new ospl::TxMessage(sender_impl2, 0, pub_name, topic_name);
    ospl_rx = new ospl::RxMessage(receiver_impl2, 0, sub_name, topic_name);

    //ZMQ
    zmq_tx = new zmq::TxMessage(sender_impl3, std::string("tcp://*:6000"));
    zmq_rx = new zmq::RxMessage(receiver_impl3, std::string("tcp://localhost:6000"));

    //QPID
    qpid_tx = new qpid::TxMessage(sender_impl4, "localhost:5672", "a");
    qpid_rx = new qpid::RxMessage(receiver_impl4, "localhost:5672",  "a");
    
    
    //Attach Ports
    sender_impl->_set_txMessage(rti_tx);
    sender_impl2->_set_txMessage(ospl_tx);
    sender_impl3->_set_txMessage(zmq_tx);
    sender_impl4->_set_txMessage(qpid_tx);

    receiver_impl->_set_rxMessage(rti_rx);
    receiver_impl2->_set_rxMessage(ospl_rx);
    receiver_impl3->_set_rxMessage(zmq_rx);
    receiver_impl4->_set_rxMessage(qpid_rx);


    int i = 600;
    while(i-- > 0){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        sender_impl->periodic_event();
        sender_impl2->periodic_event();
        sender_impl3->periodic_event();
        sender_impl4->periodic_event();
    }

    return -1;
}
