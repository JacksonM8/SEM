#ifndef RTI_TXMESSAGE_H
#define RTI_TXMESSAGE_H

#define EXPORTED __attribute__((visibility("default")))

//Include the concrete port interfaces
#include "../interfaces.h"

//Includes the ::Message and ospl::Message
#include "messageconvert.h"

namespace rti{
    //Forward declare the Middleware specific EventPort
    template <class T, class S> class OutEventPort;

    class TxMessage: public txMessageInt{
        public:
            EXPORTED TxMessage(txMessageInt* component, int domain_id, std::string publisher_name, std::string topic_name);
            void EXPORTED txMessage(::Message* message);
            void EXPORTED tx_(::Message* message){};
        private:
            //This is the concrete event port
            rti::OutEventPort<::Message, cdit::Message> * event_port_;

            //This is the Component this port should call into
            txMessageInt* component_;        
    };
};

#endif //RTI_TXMESSAGE_H