#include "worker.h"
#include <stdio.h>
#include <iostream>
#include <stdarg.h>

Worker::Worker(Component* component, std::string worker_name, std::string inst_name){
    set_name(inst_name);
    component_ = component;
    worker_name_ = worker_name;
};

Worker::~Worker(){

};

std::string Worker::get_worker_name(){
    return worker_name_;
};

int Worker::get_new_work_id(){
    std::unique_lock<std::mutex>(mutex_);
    return work_id_ ++;
};

Component* Worker::get_component(){
    return component_;
};

std::string Worker::get_arg_string(const std::string str_format, va_list args){
    //We need to make a copy of the arg list before we unwind it.
    va_list arg_copy;
    va_copy(arg_copy, args);
    //Include space for NULL character at end of string
    size_t size = vsnprintf(NULL, 0, str_format.c_str(), arg_copy) + 1;
    //Free memory of our copy
    va_end(arg_copy);
    
    //Dynamically allocate memory for buffer to write into
    char * buffer = new char[size];
    //Write into our buffer using our args list
    vsprintf (buffer, str_format.c_str(), args);
    //Construct a string to return
    std::string str(buffer);
    //Free memory we allocated on the heap
    delete[] buffer;
    return str;
};

std::string Worker::get_arg_string_variadic(const std::string str_format, ...){
    va_list args;
    va_start(args, str_format);
    auto str = get_arg_string(str_format, args);
    va_end(args);
    return str;
};