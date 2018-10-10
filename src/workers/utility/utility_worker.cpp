#include "utility_worker.h"
#include "utility_worker_impl.h"
#include <sstream>
#include <iostream>
#include <stdarg.h>

const int UTILITY_WORKER_LOG_LEVEL = 8;

Utility_Worker::Utility_Worker(const BehaviourContainer& container, const std::string& inst_name) : Worker(container, GET_FUNC, inst_name){
    impl_ = std::unique_ptr<Utility_Worker_Impl>(new Utility_Worker_Impl());
    impl_->SetRandomSeed(static_cast<unsigned int>(GetTimeOfDay()));
}

double Utility_Worker::GetTimeOfDay(){
    return impl_->TimeOfDay();
}

std::string Utility_Worker::GetTimeOfDayString(){
    return impl_->TimeOfDayString();
}

double Utility_Worker::EvaluateComplexity(const std::string complexity, ...){
    va_list args;
    va_start(args, complexity);
    auto out = impl_->EvaluateComplexity(complexity.c_str(), args);
    va_end(args);
    return out;
}

void Utility_Worker::Log(const std::string str_format, int log_level, ...){
    va_list args;
    va_start(args, log_level);
    const auto& message = get_arg_string(str_format, args);
    va_end(args);

    Worker::Log(GET_FUNC, Logger::WorkloadEvent::MESSAGE, get_new_work_id(), message, log_level);
}



void Utility_Worker::USleep(int microseconds){
    auto id = get_new_work_id();
    Worker::Log(GET_FUNC, Logger::WorkloadEvent::STARTED, id, "Sleeping for: " + std::to_string(microseconds) + " us");
    impl_->USleep(microseconds);
    Worker::Log(GET_FUNC, Logger::WorkloadEvent::FINISHED, id);
}

void Utility_Worker::Sleep(int seconds){
    auto id = get_new_work_id();
    Worker::Log(GET_FUNC, Logger::WorkloadEvent::STARTED, id, "Sleeping for: " + std::to_string(seconds) + " s");
    impl_->Sleep(seconds);
    Worker::Log(GET_FUNC, Logger::WorkloadEvent::FINISHED, id);
}


 
void Utility_Worker::SetRandomSeed(unsigned int seed){
    Worker::Log(GET_FUNC, Logger::WorkloadEvent::MESSAGE, get_new_work_id(), "Set random seed: " + std::to_string(seed));
    impl_->SetRandomSeed(seed);
}

int Utility_Worker::RandomUniformInt(int lower_bound, int upper_bound){
    return impl_->RandomUniformInt(lower_bound, upper_bound);
}

double Utility_Worker::RandomUniformReal(double lower_bound, double upper_bound){
    return impl_->RandomUniformReal(lower_bound, upper_bound);
}

double Utility_Worker::RandomNormalReal(double mean, double stddev){
    return impl_->RandomNormalReal(mean, stddev);
}

double Utility_Worker::RandomExponentialReal(double lambda){
    return impl_->RandomExponentialReal(lambda);
}

std::string Utility_Worker::GenerateUUID(){
    return impl_->GenerateUUID();
}
