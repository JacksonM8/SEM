#ifndef CORE_MODELLOGGER_H
#define CORE_MODELLOGGER_H

#include <mutex>
#include <string>

#include "component.h"
#include "eventports/eventport.h"

#ifdef MSVC
    #define GET_FUNC __FUNCTION__
#else
    #define GET_FUNC __PRETTY_FUNCTION__
#endif

class Worker;

namespace zmq{
    class ProtoWriter;
};
namespace google{namespace protobuf{class MessageLite;}};

class ModelLogger{
    public:
    enum class Mode {OFF, LIVE, CACHED};
        

        enum class LifeCycleEvent{
            STARTED = 0,
            ACTIVATED = 1,
            PASSIVATED = 2,
            TERMINATED = 3,
        };

        enum class WorkloadEvent{
            STARTED = 0,
            FINISHED = 1,
            MESSAGE = 2,
        };

        enum class ComponentEvent{
            SENT = 0,
            RECEIVED = 1,
            STARTED_FUNC = 2,
            FINISHED_FUNC = 3,
            IGNORED = 4,
        };
        //Static getter functions
        static bool setup_model_logger(std::string host_name, std::string endpoint, Mode mode);
        static ModelLogger* get_model_logger();
        static bool shutdown_logger();
        
    protected:
        ModelLogger();
        
        bool setup_logger(std::string endpoint, Mode mode);
        bool is_setup();
        void set_hostname(std::string host_name);
        
        zmq::ProtoWriter* writer_;
        ~ModelLogger();
    public:
        void LogWorkerEvent(const Worker& worker, std::string function_name, ModelLogger::WorkloadEvent event, int work_id = -1, std::string args = "");
        void LogMessageEvent(const EventPort& eventport);
        void LogUserMessageEvent(const Component& component, std::string message);
        void LogUserFlagEvent(const Component& component, std::string message);

        
        void LogLifecycleEvent(const Component& component, ModelLogger::LifeCycleEvent event);
        void LogLifecycleEvent(const EventPort& eventport, ModelLogger::LifeCycleEvent event);
        void LogComponentEvent(const EventPort& eventport, const ::BaseMessage& message, ModelLogger::ComponentEvent event);
        void LogFailedComponentConstruction(std::string component_type, std::string component_name, std::string component_id);
        void LogFailedPortConstruction(std::string component_type, std::string component_name, std::string component_id);

        const std::string get_hostname();

    private:
        void PushMessage(google::protobuf::MessageLite* message);

        bool active_ = true;

        std::string host_name_;
        
        static ModelLogger* singleton_;
        static std::mutex global_mutex_;
};

enum class Severity{
    FATAL = 0,
    ERROR = 1,
    WARNING = 2,
    INFO = 3,
    DEBUG = 4
};



class Log{
    public:
        Log& Msg(const std::string& message);
        Log& Context(Activatable* context);
        Log& Func(const std::string& function_name);
        Log& Class(const std::string& class_name);

        ~Log();
        Log(const Log& log) = delete;
        Log(const Severity& severity);
    private:
        
        std::string message_;
        std::string class_name_;
        std::string function_name_;
        Activatable* context_ = 0;
        Severity severity_;
};


#endif //CORE_MODELLOGGER_H