#include "requestbuilder.h"

/**
 * @brief RequestBuilder::RequestBuilder
 */
RequestBuilder::RequestBuilder() {}


/**
 * @brief RequestBuilder::build
 */
RequestBuilder RequestBuilder::build()
{
    return RequestBuilder();
}


/**
 * @brief RequestBuilder::buildRequests
 * @param requestKinds
 */
void RequestBuilder::buildRequests(const QList<MEDEA::ChartDataKind> &requestKinds)
{
    for (auto kind : requestKinds) {
        switch (kind) {
        case MEDEA::ChartDataKind::PORT_LIFECYCLE:
            portLifecycleRequest_ = std::unique_ptr<PortLifecycleRequest>(new PortLifecycleRequest());
            break;
        case MEDEA::ChartDataKind::WORKLOAD:
            workloadRequest_ = std::unique_ptr<WorkloadRequest>(new WorkloadRequest());
            break;
        case MEDEA::ChartDataKind::CPU_UTILISATION:
            cpuUtilisationRequest_ = std::unique_ptr<CPUUtilisationRequest>(new CPUUtilisationRequest());
            break;
        case MEDEA::ChartDataKind::MEMORY_UTILISATION:
            memoryUtilisationRequest_ = std::unique_ptr<MemoryUtilisationRequest>(new MemoryUtilisationRequest());
            break;
        case MEDEA::ChartDataKind::MARKER:
            markerRequest_ = std::unique_ptr<MarkerRequest>(new MarkerRequest());
            break;
        case MEDEA::ChartDataKind::PORT_EVENT:
            portEventRequest_ = std::unique_ptr<PortEventRequest>(new PortEventRequest());
            break;
        case MEDEA::ChartDataKind::NETWORK_UTILISATION:
            networkUtilisationRequest_ = std::unique_ptr<NetworkUtilisationRequest>(new NetworkUtilisationRequest());
            break;
        default:
            qWarning("RequestBuilder::buildRequests - Unknown chart data kind");
            break;
        }
    }
}


/**
 * @brief RequestBuilder::setExperimentRunID
 * @param experiment_run_id
 */
void RequestBuilder::setExperimentRunID(const quint32 experiment_run_id)
{
    if (portLifecycleRequest_) {
        portLifecycleRequest_->setExperimentRunID(experiment_run_id);
    }
    if (workloadRequest_) {
        workloadRequest_->setExperimentRunID(experiment_run_id);
    }
    if (cpuUtilisationRequest_) {
        cpuUtilisationRequest_->setExperimentRunID(experiment_run_id);
    }
    if (memoryUtilisationRequest_) {
        memoryUtilisationRequest_->setExperimentRunID(experiment_run_id);
    }
    if (markerRequest_) {
        markerRequest_->setExperimentRunID(experiment_run_id);
    }
    if (portEventRequest_) {
        portEventRequest_->setExperimentRunID(experiment_run_id);
    }
    if (networkUtilisationRequest_) {
        networkUtilisationRequest_->setExperimentRunID(experiment_run_id);
    }
}


/**
 * @brief RequestBuilder::setTimeInterval
 * @param time_interval
 */
void RequestBuilder::setTimeInterval(const QVector<qint64> &time_interval)
{
    if (portLifecycleRequest_) {
        portLifecycleRequest_->setTimeInterval(time_interval);
    }
    if (workloadRequest_) {
        workloadRequest_->setTimeInterval(time_interval);
    }
    if (cpuUtilisationRequest_) {
        cpuUtilisationRequest_->setTimeInterval(time_interval);
    }
    if (memoryUtilisationRequest_) {
        memoryUtilisationRequest_->setTimeInterval(time_interval);
    }
    if (markerRequest_) {
        markerRequest_->setTimeInterval(time_interval);
    }
    if (portEventRequest_) {
        portEventRequest_->setTimeInterval(time_interval);
    }
    if (networkUtilisationRequest_) {
        networkUtilisationRequest_->setTimeInterval(time_interval);
    }
}


/**
 * @brief RequestBuilder::setComponentNames
 * @param component_names
 */
void RequestBuilder::setComponentNames(const QVector<QString> &component_names)
{
    if (portLifecycleRequest_) {
        portLifecycleRequest_->setComponentNames(component_names);
    }
    if (workloadRequest_) {
        workloadRequest_->setComponentNames(component_names);
    }
    if (markerRequest_) {
        markerRequest_->setComponentNames(component_names);
    }
    if (portEventRequest_) {
        portEventRequest_->setComponentNames(component_names);
    }
}


/**
 * @brief RequestBuilder::setComponentInstanceIDS
 * @param component_instance_ids
 */
void RequestBuilder::setComponentInstanceIDS(const QVector<QString> &component_instance_ids)
{
    if (portLifecycleRequest_) {
        portLifecycleRequest_->setComponentInstanceIDS(component_instance_ids);
    }
    if (workloadRequest_) {
        workloadRequest_->setComponentInstanceIDS(component_instance_ids);
    }
    if (markerRequest_) {
        markerRequest_->setComponentInstanceIDS(component_instance_ids);
    }
    if (portEventRequest_) {
        portEventRequest_->setComponentInstanceIDS(component_instance_ids);
    }
}


/**
 * @brief RequestBuilder::setComponentInstancePaths
 * @param component_instance_paths
 */
void RequestBuilder::setComponentInstancePaths(const QVector<QString> &component_instance_paths)
{
    if (portLifecycleRequest_) {
        portLifecycleRequest_->setComponentInstancePaths(component_instance_paths);
    }
    if (workloadRequest_) {
        workloadRequest_->setComponentInstancePaths(component_instance_paths);
    }
    if (markerRequest_) {
        markerRequest_->setComponentInstancePaths(component_instance_paths);
    }
    if (portEventRequest_) {
        portEventRequest_->setComponentInstancePaths(component_instance_paths);
    }
}


/**
 * @brief RequestBuilder::setPortIDs
 * @param port_ids
 */
void RequestBuilder::setPortIDs(const QVector<QString> &port_ids)
{
    if (portLifecycleRequest_) {
        portLifecycleRequest_->setPortIDs(port_ids);
    }
    if (portEventRequest_) {
        portEventRequest_->setPortIDs(port_ids);
    }
}


/**
 * @brief RequestBuilder::setPortPaths
 * @param port_paths
 */
void RequestBuilder::setPortPaths(const QVector<QString> &port_paths)
{
    if (portLifecycleRequest_) {
        portLifecycleRequest_->setPortPaths(port_paths);
    }
    if (portEventRequest_) {
        portEventRequest_->setPortPaths(port_paths);
    }
}


/**
 * @brief RequestBuilder::setWorkerInstanceIDs
 * @param worker_instance_ids
 */
void RequestBuilder::setWorkerInstanceIDs(const QVector<QString> &worker_instance_ids)
{
    if (workloadRequest_) {
        workloadRequest_->setWorkerInstanceIDs(worker_instance_ids);
    }
    if (markerRequest_) {
        markerRequest_->setWorkerInstanceIDs(worker_instance_ids);
    }
}


/**
 * @brief RequestBuilder::setWorkerInstancePaths
 * @param worker_paths
 */
void RequestBuilder::setWorkerInstancePaths(const QVector<QString> &worker_instance_paths)
{
    if (workloadRequest_) {
        workloadRequest_->setWorkerInstancePaths(worker_instance_paths);
    }
    if (markerRequest_) {
        markerRequest_->setWorkerInstancePaths(worker_instance_paths);
    }
}


/**
 * @brief RequestBuilder::setNodeIDs
 * @param node_ids
 */
void RequestBuilder::setNodeIDs(const QVector<QString> &node_ids)
{
    if (cpuUtilisationRequest_) {
        cpuUtilisationRequest_->setNodeIDs(node_ids);
    }
    if (memoryUtilisationRequest_) {
        memoryUtilisationRequest_->setNodeIDs(node_ids);
    }
    if (networkUtilisationRequest_) {
        networkUtilisationRequest_->setNodeIDs(node_ids);
    }
}


/**
 * @brief RequestBuilder::setNodeHostnames
 * @param node_hostnames
 */
void RequestBuilder::setNodeHostnames(const QVector<QString> &node_hostnames)
{
    if (cpuUtilisationRequest_) {
        cpuUtilisationRequest_->setNodeHostnames(node_hostnames);
    }
    if (memoryUtilisationRequest_) {
        memoryUtilisationRequest_->setNodeHostnames(node_hostnames);
    }
    if (networkUtilisationRequest_) {
        networkUtilisationRequest_->setNodeHostnames(node_hostnames);
    }
}


/**
 * @brief RequestBuilder::getPortLifecycleRequest
 * @throws std::invalid_argument
 * @return
 */
const PortLifecycleRequest& RequestBuilder::getPortLifecycleRequest() const
{
    if (!portLifecycleRequest_) {
        throw std::invalid_argument("No PortLifecycleRequest");
    }
    return *portLifecycleRequest_;
}


/**
 * @brief RequestBuilder::getWorkloadRequest
 * @throws std::invalid_argument
 * @return
 */
const WorkloadRequest& RequestBuilder::getWorkloadRequest() const
{
    if (!workloadRequest_) {
        throw std::invalid_argument("No WorkloadRequest");
    }
    return *workloadRequest_;
}


/**
 * @brief RequestBuilder::getCPUUtilisationRequest
 * @throws std::invalid_argument
 * @return
 */
const CPUUtilisationRequest& RequestBuilder::getCPUUtilisationRequest() const
{
    if (!cpuUtilisationRequest_) {
        throw std::invalid_argument("No CPUUtilisationRequest");
    }
    return *cpuUtilisationRequest_;
}


/**
 * @brief RequestBuilder::getMemoryUtilisationRequest
 * @throws std::invalid_argument
 * @return
 */
const MemoryUtilisationRequest& RequestBuilder::getMemoryUtilisationRequest() const
{
    if (!memoryUtilisationRequest_) {
        throw std::invalid_argument("No MemoryUtilisationRequest");
    }
    return *memoryUtilisationRequest_;
}


/**
 * @brief RequestBuilder::getMarkerRequest
 * @throws std::invalid_argument
 * @return
 */
const MarkerRequest& RequestBuilder::getMarkerRequest() const
{
    if (!markerRequest_) {
        throw std::invalid_argument("No MarkerRequest");
    }
    return *markerRequest_;
}


/**
 * @brief RequestBuilder::getPortEventRequest
 * @throws std::invalid_argument
 * @return
 */
const PortEventRequest& RequestBuilder::getPortEventRequest() const
{
    if (!portEventRequest_) {
        throw std::invalid_argument("No PortEventRequest");
    }
    return *portEventRequest_;
}


/**
 * @brief RequestBuilder::getNetworkUtilisationRequest
 * @throws std::invalid_argument
 * @return
 */
const NetworkUtilisationRequest& RequestBuilder::getNetworkUtilisationRequest() const
{
    if (!networkUtilisationRequest_) {
        throw std::invalid_argument("No NetworkUtilisationRequest");
    }
    return *networkUtilisationRequest_;
}
