#ifndef OUTEVENTPORT_HPP
#define OUTEVENTPORT_HPP

#include "../component.h"
#include "../eventport.h"
#include <iostream>
//Interface for a standard templated OutEventPort
template <class T> class OutEventPort: public EventPort{
    public:
        OutEventPort(Component* component, std::string name){
            set_name(name);
            if(component){
                //Set our Component and attach this port
                component_ = component;
                component_->AddEventPort(this);
            }
        }
        virtual ~OutEventPort(){};
        virtual void tx(T*) = 0;
    private:
        Component* component_;
};

#endif //OUTEVENTPORT_HPP