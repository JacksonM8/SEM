#ifndef OPENCL_RE_UTILITIES_H
#define OPENCL_RE_UTILITIES_H

#include <string>
#include <iostream>
#include <core/modellogger.h>

//#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#include "cl2.hpp"

#define OPENCL_DEBUG_TO_STDERR

/*void LogOpenCLError(std::string message, cl_int errorCode) {
	std::cerr << "OpenCL error (" << errorCode << "): " << message << std::endl;
}*/

/*void LogOpenCLError(std::string message) {
	std::cerr << "OpenCL error: " << message << std::endl;
}*/

extern std::string clErrorNames[];

extern std::string GetSourcePath(std::string filename);

extern cl::Program::Sources ReadOpenCLSourceCode(const std::vector<std::string>& filenames,
	Worker* worker_reference=NULL);
	
extern cl::Program::Binaries ReadOpenCLBinaries(const std::vector<std::string>& filenames,
	Worker* worker_reference=NULL);

extern void LogOpenCLError(const Worker* worker_reference,
	std::string function_signature,
	std::string error_message,
	cl_int cl_error_code);

extern void LogOpenCLError(const Worker* worker_reference,
	std::string function_signature,
	std::string error_message);


#endif
