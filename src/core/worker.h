#ifndef CORE_WORKER_H
#define CORE_WORKER_H

#include <mutex>
#include <string>
#include <stdarg.h>

#include "behaviourcontainer.h"

class Component;

class Worker: public BehaviourContainer{
    public:
        Worker(const BehaviourContainer& container, const std::string& class_name, const std::string& inst_name, bool is_worker = true);
        virtual ~Worker();

        const BehaviourContainer& get_container() const;

        const Component& get_component() const;
        
        std::string get_worker_name() const;
        
        std::string get_arg_string(const std::string str_format, va_list args);
        std::string get_arg_string_variadic(const std::string str_format, ...);

        bool is_custom_class() const;
        bool is_worker() const;

    protected:
        int get_new_work_id();
        void Log(const std::string& function_name, const Logger::WorkloadEvent& event, int work_id = -1, std::string args = "", int message_log_level = -1);
        void LogException(const std::string& function_name, const std::exception& ex, int work_id = -1);

    private:
        const BehaviourContainer& container_;
        const std::string worker_name_;
        const bool is_worker_class_;
        
        std::mutex mutex_;
        int work_id_ = 0;

};

#endif //CORE_WORKER_H
