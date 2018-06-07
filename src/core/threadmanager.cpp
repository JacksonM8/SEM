#include "threadmanager.h"

ThreadManager::~ThreadManager(){
    if(thread_){
        //Shutdown our thread
        thread_->join();
    }
};

void ThreadManager::SetThread(std::unique_ptr<std::thread> thread){
    thread_ = std::move(thread);
};

void ThreadManager::Thread_Configured(){
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    if(state_ == State::NONE){
        //Transition the state
        state_ = State::CONFIGURED;
        state_condition_.notify_all();
    }
};

void ThreadManager::Thread_Terminated(){
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    //Transition the state
    state_ = State::TERMINATED;
    state_condition_.notify_all();
};

void ThreadManager::Thread_Activated(){
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    if(state_ == State::CONFIGURED){
        //Transition the state
        state_ = State::ACTIVE;
        state_condition_.notify_all();
    }
};

bool ThreadManager::Thread_WaitForActivate(){
    std::unique_lock<std::mutex> transition_lock(transition_mutex_);
    transition_condition_.wait(transition_lock, [this]{return transition_ != Transition::NONE;});
    auto activate_trans = transition_ == Transition::ACTIVATE;
    if(activate_trans){
        //Thread_Activated();
        return true;
    }
    return false;
};

bool ThreadManager::Configure(){
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    if(state_ == State::NONE){
        //Wait for state to be moved from NONE
        state_condition_.wait(state_lock, [this]{return state_ != State::NONE;});
    }
    return state_ == State::CONFIGURED;
};

bool ThreadManager::Activate(){
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    if(state_ == State::CONFIGURED){
        {
            //Notify That Transition has happened
            std::unique_lock<std::mutex> terminate_lock(transition_mutex_);
            transition_ = Transition::ACTIVATE;
            transition_condition_.notify_all();
        }
        //Wait for state to be moved from NONE
        state_condition_.wait(state_lock, [this]{return state_ != State::CONFIGURED;});
    }
    return state_ == State::ACTIVE;
};

bool ThreadManager::Terminate(){
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    {
        //Notify That Transition has happened
        std::unique_lock<std::mutex> terminate_lock(transition_mutex_);
        transition_ = Transition::TERMINATE;
        transition_condition_.notify_all();
    }
    //Wait for state to be moved from NONE
    state_condition_.wait(state_lock, [this]{return state_ == State::TERMINATED;});
    return state_ == State::TERMINATED;
};

ThreadManager::State ThreadManager::GetState(){
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    return state_;
};