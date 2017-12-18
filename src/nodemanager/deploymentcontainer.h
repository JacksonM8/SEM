#ifndef CORE_NODEMANAGER_DEPLOYMENTCONTAINER_H
#define CORE_NODEMANAGER_DEPLOYMENTCONTAINER_H

#include <unordered_map>
#include <functional>

#include <core/component.h>
#include <core/libportexport.h>
#include <core/libcomponentexport.h>

#include "dllloader.h"

namespace NodeManager{
    class Component;
    class EventPort;
    class Worker;
    class Node;
};

typedef std::function<EventPortCConstructor> EventPortConstructor;
typedef std::function<ComponentCConstructor> ComponentConstructor;

class DeploymentContainer : public Activatable{
    public:
        DeploymentContainer();
        ~DeploymentContainer();
        bool Configure(const NodeManager::Node& message);
        std::weak_ptr<Component> AddComponent(std::unique_ptr<Component> component, const std::string& name);
        std::weak_ptr<Component> GetComponent(const std::string& name);
        std::shared_ptr<Component> RemoveComponent(const std::string& name);

        void SetLibraryPath(const std::string library_path);
    protected:
        bool HandleActivate();
        bool HandlePassivate();
        bool HandleTerminate();
        bool HandleConfigure();
    private:
        //Get/Constructors
        std::shared_ptr<Worker> GetConfiguredWorker(std::shared_ptr<Component> component, const NodeManager::Worker& worker_pb);
        std::shared_ptr<Component> GetConfiguredComponent(const NodeManager::Component& component_pb);
        std::shared_ptr<EventPort> GetConfiguredEventPort(std::shared_ptr<Component> component, const NodeManager::EventPort& eventport_pb);
        
        //Constructor functions
        std::shared_ptr<EventPort> ConstructPeriodicEvent(std::weak_ptr<Component> component, const std::string& port_name);
        std::shared_ptr<EventPort> ConstructOutEventPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string& namespace_name);
        std::shared_ptr<EventPort> ConstructInEventPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string& namespace_name);
        std::shared_ptr<Component> ConstructComponent(const std::string& component_type, const std::string& component_name, const std::string& component_id);
        
        std::string get_port_library_name(const std::string& middleware, const std::string& namespace_name, const std::string& datatype);
        std::string get_component_library_name(const std::string& component_type);

        std::string library_path_;

        //Middleware -> construct functions
        std::unordered_map<std::string, EventPortConstructor> out_eventport_constructors_;
        std::unordered_map<std::string, EventPortConstructor> in_eventport_constructors_;
        std::unordered_map<std::string, ComponentConstructor> component_constructors_;
        std::unordered_map<std::string, std::shared_ptr<Component> > components_;

        DllLoader dll_loader;
};
#endif //CORE_NODEMANAGER_DEPLOYMENTCONTAINER_H