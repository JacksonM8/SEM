#include "inputparameter.h"

InputParameter::InputParameter(): Parameter(NK_INPUTPARAMETER)
{
    setDataReciever(true);
    setDataProducer(false);
}

bool InputParameter::canAdoptChild(Node *node)
{
    return false;
}

bool InputParameter::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return Parameter::canAcceptEdge(edgeKind, dst);
}
