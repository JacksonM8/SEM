#ifndef OSPLTXMESSAGE_H
#define OSPLTXMESSAGE_H

//Include the concrete port interfaces
#include "../interfaces.h"

//Includes the ::Message and ospl::Message
#include "messageconvert.h"

namespace ospl{
    //Forward declare the Middleware specific EventPort
    template <class T, class S> class Ospl_OutEventPort;

    class TxMessage: public txMessageInt{
        public:
            TxMessage(txMessageInt* component, int domain_id, std::string publisher_name, std::string writer_name, std::string topic_name);
            void txMessage(::Message* message);
            void tx_(::Message* message){};
        private:
            //This is the concrete event port
            Ospl_OutEventPort<::Message, ospl::Message> * event_port_;

            //This is the Component this port should call into
            txMessageInt* component_;        
    };
};

#endif //OSPLTXMESSAGE_H