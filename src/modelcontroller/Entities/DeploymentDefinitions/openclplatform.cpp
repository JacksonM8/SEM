#include "openclplatform.h"
#include "../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::OPENCL_PLATFORM;
const static QString kind_string = "OpenCLPlatform";

void OpenCLPlatform::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new OpenCLPlatform(factory, is_temp_node);
    });
}

OpenCLPlatform::OpenCLPlatform(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setLabelFunctional(false);
    setNodeType(NODE_TYPE::HARDWARE);
    setAcceptsNodeKind(NODE_KIND::OPENCL_DEVICE);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::TARGET);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "", true);
    factory.AttachData(this, "vendor", QVariant::String, "", true);
    factory.AttachData(this, "version", QVariant::String, "", true);
}