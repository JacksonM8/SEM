#ifndef OPENCL_MANAGER_H
#define OPENCL_MANAGER_H


#include <unordered_map>
#include <map>
#include <mutex>

#include <core/modellogger.h>
#include <core/worker.h>

#include "OpenCLBuffer.hpp"
#include "opencldevice.h"

class OpenCLKernel;

namespace cl {
	class Platform;
	class Context;
	class CommandQueue;
	class Program;
	class Kernel;
}

class OpenCLManager {
	public:
		//OpenCLManager(OpenCLManager&& other) = default;
		//OpenCLManager& operator=(OpenCLManager&& other) = default;
		//OpenCLManager(const OpenCLManager& other) = delete;
		//OpenCLManager& operator=(const OpenCLManager& other) = delete;
		//OpenCLManager(OpenCLManager&& other) = delete;
		//OpenCLManager& operator=(OpenCLManager&& other) = delete;
		~OpenCLManager();// = default;

		/**
		* Returns the OpenCLManager responsible for managing a given OpenCL platform,
		* constructing and initializing the appropriate resources if required.
		*
		* @param platformID the index of the OpenCL platform as specified by the deployment information
		* @param workerReference a reference to the worker making the call (for logging)
		* @return The OpenCLManager for the provided platform, or NULL if one can't be created
		**/
		static OpenCLManager* GetReferenceByPlatformID(const Worker& worker, int platform_id);
		
		static OpenCLManager* GetReferenceByPlatformName(const Worker& worker, std::string platform_name);


		static const std::vector<cl::Platform> GetPlatforms(const Worker& worker);


		const cl::Context& GetContext() const;

		std::string GetPlatformName() const;

		const std::vector<std::unique_ptr<OpenCLDevice> > & GetDevices(const Worker& worker);

		const std::vector<std::shared_ptr<cl::CommandQueue> > GetQueues() const;

		const std::vector<OpenCLKernel> CreateKernels(const Worker& worker, const std::vector<std::string>& filenames);
		
		template <typename T>
		OpenCLBuffer<T> CreateBuffer(const Worker& worker, size_t buffer_size);
		template <typename T>
		OpenCLBuffer<T> CreateBuffer(const Worker& worker, const std::vector<T>& data, OpenCLDevice& device, bool blocking=true);

		template <typename T>
		void ReleaseBuffer(const Worker& worker, OpenCLBuffer<T>& buffer);
		
		bool IsValid() const;
        bool IsFPGA() const;

		class BufferAttorney {
			BufferAttorney() = delete;
		private:
			friend class OpenCLBufferBase;
			static int GetNewBufferID(const Worker& worker, OpenCLManager& manager, OpenCLBufferBase& buffer) {
				return manager.TrackBuffer(worker, buffer);
			}
			static void ReleaseBufferID(OpenCLManager& manager, OpenCLBufferBase& buffer) {
				manager.UntrackBuffer(buffer.GetID());
			}
		};

		static const int invalid_buffer_id_ = -1;

	private:
		OpenCLManager(const Worker& worker, cl::Platform &platform);
		//~OpenCLManager() {};

		int TrackBuffer(const Worker& worker, const OpenCLBufferBase& buffer);
		void UntrackBuffer(int buffer_id);
		void Initialise();

    	bool LoadAllBinaries(const Worker& worker);

		static void LogError(const Worker& worker,
							std::string function_name,
							std::string error_message,
							int cl_error_code);
		static void LogError(const Worker& worker,
							std::string function_name,
							std::string error_message);

		// A separate OpenCLManager is maintained for each OpenCL platform
		static std::unordered_map<unsigned int, std::unique_ptr<OpenCLManager> > reference_map_;
		static std::vector<cl::Platform> platform_list_;

		// Variables
		bool valid_ = false;
		cl::Platform& platform_;
		std::string platform_name_;
		std::unique_ptr<cl::Context> context_;
		std::vector<std::unique_ptr<OpenCLDevice> > device_list_;
		std::vector<std::shared_ptr<cl::CommandQueue> > queues_;
		//cl::Program* program_;
		//std::vector< std::vector<cl::Kernel>* >  kernel_vector_store_;

		std::mutex opencl_resource_mutex_;

		//std::map<int, std::unique_ptr<OpenCLBufferBase> > buffer_store_;
		std::map<int, size_t> buffer_store_;
		std::mutex opencl_buffer_mutex_;
		int buffer_id_count_ = -1;

        bool is_fpga_=false;
};


template <typename T>
OpenCLBuffer<T> OpenCLManager::CreateBuffer(const Worker& worker, size_t buffer_size){
	return OpenCLBuffer<T>(worker, *this, buffer_size);
}

template <typename T>
OpenCLBuffer<T> OpenCLManager::CreateBuffer(const Worker& worker, const std::vector<T>& data, OpenCLDevice& device, bool blocking) {
	return OpenCLBuffer<T>(worker, *this, data, device, blocking);
}


template <typename T>
void OpenCLManager::ReleaseBuffer(const Worker& worker, OpenCLBuffer<T>& buffer) {
	//UntrackBuffer(buffer.GetID());
}

#endif // OPENCL_MANAGER_H
