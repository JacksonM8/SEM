#include "edge.h"
#include "graphml.h"
#include "data.h"
#include "node.h"
#include <QDebug>

Edge::Edge(Node *s, Node *d, EDGE_CLASS edgeClass):Entity(EK_EDGE)
{
    //Set the instance Variables
    source = s;
    destination = d;

    this->edgeClass = edgeClass;

    //Attach the Edge to its source/Destination
    source->addEdge(this);
    destination->addEdge(this);


    type = getType();
}

Edge::~Edge()
{
    if(destination){
        destination->removeEdge(this);
    }

    if(source){
        source->removeEdge(this);
    }
}

Node *Edge::getSource()
{
    return source;
}

int Edge::getSourceID()
{
    if(source){
        return source->getID();
    }
    return -1;
}

int Edge::getDestinationID()
{
    if(destination){
        return destination->getID();
    }
    return -1;
}

Node *Edge::getDestination()
{
    return destination;
}

QString Edge::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    tabSpace.fill('\t', indentationLevel);

    QString returnable = tabSpace + QString("<edge id=\"%1\" source=\"%2\" target =\"%3\"").arg(QString::number(getID()), QString::number(getSource()->getID()), QString::number(getDestination()->getID()));

    if(hasData()){
        returnable += ">\n";
        foreach(Data* data, getData()){
            returnable += data->toGraphML(indentationLevel + 1);
        }
        returnable += tabSpace + "</edge>\n";
    }else{
        returnable += "/>\n";
    }

    return returnable;
}



bool Edge::isInstanceToInstanceLink()
{
    return (source->isInstance() && source->getDefinition() == destination && type == ET_MATCHINGKINDS);

}

bool Edge::isInstanceLink()
{
    return (source->isInstance() && source->getDefinition() == destination);
}

bool Edge::isImplLink()
{
    return (source->isImpl() && source->getDefinition() == destination);
}

bool Edge::isAggregateLink()
{
    return type == ET_AGGREGATE;
}

bool Edge::isDeploymentLink()
{
    return type == ET_DEPLOYMENT;
}

bool Edge::isAssemblyLink()
{
    return type == ET_ASSEMBLY;
}

bool Edge::isComponentLink()
{
    return type == ET_COMPONENT;
}

bool Edge::isTerminationLink()
{
    return type == ET_TERMINATION;
}

bool Edge::isNormalLink()
{
    return !(isInstanceLink() || isImplLink() || isDelegateLink() ||  isAggregateLink());
}

bool Edge::isDataLink()
{
    return type == ET_DATALINK;
}

bool Edge::isAssemblyLevelLink()
{
    return isDeploymentLink() || isAssemblyLink() || isComponentLink() || isDelegateLink();
}

bool Edge::isDelegateLink()
{
    return type == ET_DELEGATE;
}




bool Edge::contains(Node *item)
{
    return item == this->source || item == this->destination;
}

QString Edge::toString()
{
    return QString("Edge[%1]: [" + this->getSource()->toString() +"] <-> [" + this->getDestination()->toString() + "]").arg(this->getID());
}

bool Edge::isAggregateEdge()
{
    return edgeClass == EC_AGGREGATE;
}

bool Edge::isDefinitionEdge()
{
    return edgeClass == EC_DEFINITION;
}

Edge::EDGE_CLASS Edge::getEdgeClass()
{
    return edgeClass;
}

Edge::EDGE_TYPE Edge::getType()
{
    QString srcKind = source->getNodeKind();
    QString dstKind = destination->getNodeKind();

    if(dstKind.startsWith("Hardware")){
        if(srcKind == "ComponentInstance" || srcKind == "ComponentAssembly" || srcKind == "ManagementComponent" || srcKind == "BlackBoxInstance"){
            return ET_DEPLOYMENT;
        }
    }
    if(dstKind == "Aggregate"){
        if(srcKind.endsWith("EventPort") || srcKind == "Vector" || srcKind.endsWith("EventPortDelegate")){
            return ET_AGGREGATE;
        }
    }
    if(dstKind.endsWith("Delegate")){
        if(srcKind.endsWith("Delegate")){
            return ET_ASSEMBLY;
        }
    }
    if(dstKind == "InEventPortInstance"){
        if(srcKind == "OutEventPortInstance"){
            return ET_COMPONENT;
        }
    }
    if(dstKind == "Termination"){
        if(srcKind == "BranchState"){
            return ET_TERMINATION;
        }
    }
    if(dstKind.endsWith("EventPortInstance")){
        if(srcKind.endsWith("EventPortDelegate")){
            return ET_DELEGATE;
        }
    }
    if(dstKind.endsWith("EventPortDelegate")){
        if(srcKind.endsWith("EventPortInstance")){
            return ET_DELEGATE;
        }
    }
    if(srcKind.endsWith("Parameter") || dstKind.endsWith("Parameter")){
        return ET_DATALINK;
    }
    if(srcKind == dstKind){
        return ET_MATCHINGKINDS;
    }
    return ET_NORMAL;
}
