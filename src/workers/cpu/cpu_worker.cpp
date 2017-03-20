#include "cpu_worker.h"
#include "cpu_worker_impl.h"
#include <iostream>
#include <core/component.h>
#include <core/modellogger.h>

Cpu_Worker::Cpu_Worker(Component* component, std::string inst_name) : Worker(component, __func__, inst_name){
    impl_ = new Cpu_Worker_Impl();
}

Cpu_Worker::~Cpu_Worker(){
    if(impl_){
        delete impl_;
        impl_ = 0;
    }
}

int Cpu_Worker::IntOp(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    //Log Before
    Log(fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
    //Run work
    int result = impl_->IntOp(loop);
    //Log After
    Log(fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    return result;
}

int Cpu_Worker::FloatOp(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    //Log Before
    Log(fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
    //Run work
    int result = impl_->FloatOp(loop);
    //Log After
    Log(fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    return result;
}

int Cpu_Worker::Whetstone(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    //Log Before
    Log(fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
    //Run work
    int result = impl_->Whetstone(loop);
    //Log After
    Log(fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    return result;
}

int Cpu_Worker::Dhrystone(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    //Log Before
    Log(fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
    //Run work
    int result = impl_->Dhrystone(loop);
    //Log After
    Log(fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    return result;
}

int Cpu_Worker::MWIP(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    Log(fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
    //Log Before
    int result = impl_->MWIP(loop);
    Log(fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    //Log After
    return result;
}

int Cpu_Worker::DMIP(double loop){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("loop = %lf", loop);

    //Log Before
    Log(fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
    //Run work
    int result = impl_->DMIP(loop);
    //Log After
    Log(fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    return result;
}

int Cpu_Worker::MatrixMult(const std::vector<float> &matrixA, const std::vector<float> &matrixB,
                        std::vector<float> &matrixC){
    auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("matrixA size = %lf; matrixB size = %lf; matrixC size = %lf", 
                                        matrixA.size(), matrixB.size(), matrixC.size());

    //Log Before
    Log(fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);

    int result = impl_->MatrixMult(matrixA.size(), matrixB.size(), matrixC.size(), 
                                   matrixA.data(), matrixB.data(), matrixC.data());
    //Log After
    Log(fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
    return result;
}