#ifndef SEM_NODEMANAGERCONTROLIMPL_H
#define SEM_NODEMANAGERCONTROLIMPL_H
#include "epmregistry.h"
#include "node_manager_control_service.grpc.pb.h"

namespace sem::node_manager {

/**
 * This class is NOT thread safe.
 *  The underlying epm_registry_ SHOULD be thread safe.
 */

namespace ServiceNamespace = sem::network::services::node_manager;
class NodeManagerControlImpl final
    : public ServiceNamespace::Control::Service {
public:
    NodeManagerControlImpl(epm_registry::EpmRegistry& epm_registry);
    ~NodeManagerControlImpl() final;
    auto NewEpm(grpc::ServerContext* context,
                const ServiceNamespace::NewEpmRequest* request,
                ServiceNamespace::NewEpmResponse* response)
        -> grpc::Status final;
    auto StopEpm(grpc::ServerContext* context,
                 const ServiceNamespace::StopEpmRequest* request,
                 ServiceNamespace::StopEpmResponse* response)
        -> grpc::Status final;

private:
    epm_registry::EpmRegistry& epm_registry_;
};
} // namespace sem::node_manager

#endif // SEM_NODEMANAGERCONTROLIMPL_H
