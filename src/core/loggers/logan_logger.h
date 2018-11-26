#ifndef CORE_LOGGERS_LOGAN_LOGGER_H
#define CORE_LOGGERS_LOGAN_LOGGER_H

#include <mutex>
#include <string>

#include <core/logger.h>
#include <zmq/protowriter/cachedprotowriter.h>
#include <chrono>

namespace Logan{
    class Logger : public ::Logger{
        public:
            Logger(const std::string& experiment_name, const std::string& host_name, const std::string& container_name, const std::string& container_id, const std::string& address, const std::string& port, Logger::Mode mode);
            ~Logger();
            
            void LogMessage(const Activatable& entity, const std::string& message);
            void LogException(const Activatable& entity, const std::string& message);
            void LogLifecycleEvent(const Activatable& entity, const ::Logger::LifeCycleEvent& event);
            void LogWorkerEvent(const Worker& worker, const std::string& function_name, const ::Logger::WorkloadEvent& event, int work_id, std::string args, int message_log_level);
            void LogPortUtilizationEvent(const Port& port, const ::BaseMessage& message, const ::Logger::UtilizationEvent& event, const std::string& message_str);
            
            static std::chrono::milliseconds GetCurrentTime();

            const std::string& GetExperimentName() const;
            const std::string& GetHostName() const;
            const std::string& GetContainerId() const;
            const std::string& GetContainerName() const;
        private:
            void LogMessage(const Activatable& entity, bool is_exception, const std::string& message);

            void PushMessage(std::unique_ptr<google::protobuf::MessageLite> message);
            mutable std::mutex mutex_;
            std::unique_ptr<zmq::ProtoWriter> writer_;
            const std::string experiment_name_;
            const std::string host_name_;
            const std::string container_id_;
            const std::string container_name_;
        public:
            int send_count = 0;
            int SENT_COUNT = 0;
            int SENT_COUNT_A = 0;
    };
};

#endif //CORE_LOGGERS_LOGAN_LOGGER_H