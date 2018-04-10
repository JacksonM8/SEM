#include "deploymentattribute.h"
#include <QDebug>
const NODE_KIND node_kind = NODE_KIND::DEPLOYMENT_ATTRIBUTE;
const QString kind_string = "DeploymentAttribute";

MEDEA::DeploymentAttribute::DeploymentAttribute(EntityFactory* factory) : DataNode(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DeploymentAttribute();});

    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, false, "String");
    RegisterDefaultData(factory, node_kind, "value", QVariant::String, false);
};

MEDEA::DeploymentAttribute::DeploymentAttribute() : DataNode(node_kind)
{
    setDataProducer(true);
    setDataReceiver(true);
}

bool MEDEA::DeploymentAttribute::canAdoptChild(Node*)
{
    return false;
}

bool MEDEA::DeploymentAttribute::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    qCritical() << "MEDEA::DeploymentAttribute::canAcceptEdge";
    if(!acceptsEdgeKind(edge_kind)){
        qCritical() << "MEDEA::DeploymentAttribute::canAcceptEdge" << 1;
        return false;
    }

    QSet<NODE_KIND> valid_data_kinds = {NODE_KIND::ATTRIBUTE_INSTANCE, NODE_KIND::DEPLOYMENT_ATTRIBUTE};

    switch(edge_kind){
    case EDGE_KIND::DATA:{
        if(!valid_data_kinds.contains(dst->getNodeKind())){
            qCritical() << "MEDEA::DeploymentAttribute::canAcceptEdge" << 2;
            return false;
        }
        break;
    }
    default:
        break;
    }

    return DataNode::canAcceptEdge(edge_kind, dst);
}