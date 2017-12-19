#ifndef WORKERS_MEMORY_MEMORYWORKER
#define WORKERS_MEMORY_MEMORYWORKER

#include <core/worker.h>

class Memory_Worker_Impl;

class Memory_Worker : public Worker{
    public:
        Memory_Worker(const Component& component, const std::string& inst_name);
        ~Memory_Worker();
        
        void Allocate(double kilobytes);
        void Deallocate(double kilobytes);
        long GetAllocatedCount() const;

    private:
        Memory_Worker_Impl* impl_ = 0;
};

#endif  //WORKERS_MEMORY_MEMORYWORKER
