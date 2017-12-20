#ifndef EVENTPORT_H
#define EVENTPORT_H

#include "../activatable.h"

//Forward declare
class Component;

enum class ThreadState{WAITING, STARTED, ERROR, TERMINATE};

//Interface for a standard EventPort
class EventPort : public Activatable
{
    public:
        enum class Kind{
            NONE = 0,
            TX = 1,
            RX = 2,
            PE = 3
        };
        
        EventPort(std::weak_ptr<Component> component, const std::string& port_name, const EventPort::Kind& port_kind, const std::string& port_middleware);
        
        EventPort::Kind get_kind() const;
        std::string get_middleware() const;
        std::weak_ptr<Component> get_component() const;
    protected:
        virtual bool HandleActivate();
        virtual bool HandleConfigure();
        virtual bool HandlePassivate();
        virtual bool HandleTerminate();
        void LogActivation();
        void LogPassivation();

        void SetKind(const EventPort::Kind& port_kind);
    private:
        std::weak_ptr<Component> component_;
        std::string port_middleware_;
        EventPort::Kind port_kind_ = EventPort::Kind::NONE;
};

#endif //EVENTPORT_H