#include "dds_topicdataqospolicy.h"

DDS_TopicDataQosPolicy::DDS_TopicDataQosPolicy():Node(NK_QOS_DDS_POLICY_TOPICDATA)
{
    setNodeType(NT_QOS_DDS_POLICY);
}
bool DDS_TopicDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_TopicDataQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
