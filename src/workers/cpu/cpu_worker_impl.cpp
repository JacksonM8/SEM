#include "cpu_worker_impl.h"

#include <iostream>
#include <cmath>      // Math. functions needed for whets.cpp?

#include "intop.cpp"
#include "floatop.cpp"
#include "whets.cpp"
#include "dhry_1.cpp"

int Cpu_Worker_Impl::IntOp(double loops) {
    try{
        // This guard will ensure only one CPU Workload Execution is occurring at any given time
        //  i.e. for this worker in this component.
        std::unique_lock<std::mutex> guard(lock_, std::try_to_lock);
        if(!guard.owns_lock()){
            return -1;
        }

        // Workload Execution.
        if(loops > 0){
            integerop workload;
            workload.loop((unsigned long long)loops);
        }
        return 0;
    }
    catch(...){
        std::cerr << "Caught unkown exception WE_CPU_Impl::IntOp Loops: " << loops << std::endl;
    }
    return -1;
}

int Cpu_Worker_Impl::FloatOp(double loops){
    try{
        // This guard will ensure only one CPU Workload Execution is occurring at any given time
        //  i.e. for this worker in this component.
        std::unique_lock<std::mutex> guard(lock_, std::try_to_lock);
        if(!guard.owns_lock()){
            return -1;
        }

        // Workload Execution.
        if(loops > 0){
            floatop workload;
            workload.loop((unsigned long long)loops);
        }
        return 0;
    }
    catch(...){
        std::cerr << "Caught unkown exception WE_CPU_Impl::FloatOp Loops: " << loops << std::endl;
    }
    return -1;
}

int Cpu_Worker_Impl::Whetstone(double loops){
    try{
        // This guard will ensure only one CPU Workload Execution is occurring at any given time
        //  i.e. for this worker in this component.
        std::unique_lock<std::mutex> guard(lock_, std::try_to_lock);
        if(!guard.owns_lock()){
            return -1;
        }

        // Workload Execution.
        if(loops > 0){
            whetstone workload;
            workload.loop((int)loops);
        }
        return 0;
    }
    catch(...){
        std::cerr << "Caught unkown exception WE_CPU_Impl::Whetstone Loops: " << loops << std::endl;
    }
    return -1;
}

int Cpu_Worker_Impl::Dhrystone(double loops){
    try{
        // This guard will ensure only one CPU Workload Execution is occurring at any given time
        //  i.e. for this worker in this component.
        std::unique_lock<std::mutex> guard(lock_, std::try_to_lock);
        if(!guard.owns_lock()){
            return -1;
        }

        // Workload Execution.
        if(loops > 0){
            dhrystone workload;
            workload.loop((int)loops);
        }
        return 0;
    }
    catch(...){
        std::cerr << "Caught unkown exception WE_CPU_Impl::Dhrystone Loops: " << loops << std::endl;
    }
    return -1;
}

int Cpu_Worker_Impl::MWIP(double loops){
    try{
        // This guard will ensure only one CPU Workload Execution is occurring at any given time
        //  i.e. for this worker in this component.
        std::unique_lock<std::mutex> guard(lock_, std::try_to_lock);
        if(!guard.owns_lock()){
            return -1;
        }
        // Workload Execution.
        if(loops > 0){
            whetstone workload;
            workload.mwip((int)loops, true);
        }
        return 0;
    }
    catch(...){
        std::cerr << "Caught unkown exception WE_CPU_Impl::MWIP Loops: " << loops << std::endl;
    }
    return -1;
}

int Cpu_Worker_Impl::DMIP(double loops){
    try{
        // This guard will ensure only one CPU Workload Execution is occurring at any given time
        //  i.e. for this worker in this component.
        std::unique_lock<std::mutex> guard(lock_, std::try_to_lock);
        if(!guard.owns_lock()){
            return -1;
        }

        // Workload Execution.
        if(loops > 0){
            dhrystone workload;
            workload.dmip((int)loops, true);
        }
        return 0;
    }
    catch(...){
        std::cerr << "Caught unkown exception WE_CPU_Impl::MWIP Loops: " << loops << std::endl;
    }
    return -1;
}

int Cpu_Worker_Impl::MatrixMult(unsigned int lenA, unsigned int lenB, unsigned int lenC,
					                    const float* dataA, const float* dataB, float* dataC) {

    // Magic maths to determine dimensions of output matrix
    unsigned long long Ksquared = ((unsigned long long)lenA*(unsigned long long)lenB)/lenC;
	unsigned int k = (unsigned int)sqrt((double)Ksquared);
    std::cout << Ksquared << std::endl;
	unsigned int m = lenA/k;
	unsigned int n = lenB/k;
	if ((unsigned long)k*k != Ksquared || (unsigned long)m*k != lenA || (unsigned long)n*k != lenB) {
		std::cerr << "Error during matrix multiplication; sizes of matrices don't match, skipping calculation" << std::endl;
		return -1;
    }

    // Zero output array. This is fast (gets unrolled with decent optimisation options).
    std::fill(dataC, dataC + (n*m), 0.0f);

    // Do matrix mult
    for (unsigned int col=0; col<n; col++) {
		for (unsigned int row=0; row<m; row++) {
			
			float accum = 0;
			for (unsigned int t=0; t<k; t++) {
				dataC[col + row*n] += dataA[t + row*k]*dataB[col + t*n];
			}
		}
    }

    return 0;
}
