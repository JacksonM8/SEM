#ifndef OUTEVENTPORT_HPP
#define OUTEVENTPORT_HPP

#include "../modellogger.h"
#include "../component.h"
#include "../eventport.h"
#include <iostream>
//Interface for a standard templated OutEventPort
template <class T> class OutEventPort: public EventPort{
    public:
        OutEventPort(Component* component, std::string name):
        EventPort(component, name, EventPort::Type::TX){}
        virtual ~OutEventPort(){};
        virtual void tx(T*){
            if(is_active()){
                logger()->LogMessageEvent(this);
            }
        }
};

#endif //OUTEVENTPORT_HPP