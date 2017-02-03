#include "rx.h"

#include <zmq/ineventport.hpp>

::InEventPort<::Message>* zmq::Message::construct_rx(Component* component, std::string name, std::function<void (::Message*)> callback_function){
    return new zmq::InEventPort<::Message, proto::Message>(component, name, callback_function);
}


