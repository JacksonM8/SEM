#include "aggregate.h"
#include "member.h"
#include "aggregatemember.h"
#include "../BehaviourDefinitions/aggregateinstance.h"
#include "eventport.h"
#include <QDebug>
Aggregate::Aggregate(): Node(Node::NT_DEFINITION)
{

}

Aggregate::~Aggregate()
{

}

void Aggregate::addEventPort(EventPort *node)
{
    if(!attachedEventPorts.contains(node)){
        attachedEventPorts.append(node);
        node->setAggregate(this);
    }
}

void Aggregate::removeEventPort(EventPort *node)
{
    int index = attachedEventPorts.indexOf(node);
    if(index != -1){
        node->unsetAggregate();
        attachedEventPorts.remove(index);
    }
}

QVector<EventPort *> Aggregate::getEventPorts()
{
    return attachedEventPorts;
}

QString Aggregate::toString()
{
    return QString("Aggregate[%1]: "+this->getName()).arg(this->getID());
}

bool Aggregate::canConnect(Node* attachableObject)
{
    EventPort* eventport = dynamic_cast<EventPort*>(attachableObject);
    AggregateMember* aggregateMember = dynamic_cast<AggregateMember*>(attachableObject);
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(attachableObject);

    if (!aggregateMember && !eventport && !aggregateInstance){
        qWarning() << "Aggregate can only connect to an AggregateMember, AggregateInstance or EventPort.";
        return false;
    }

    if(eventport && eventport->getAggregate()){
        qWarning() << "Aggregate can only connect to an unn-attached Eventport";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool Aggregate::canAdoptChild(Node *child)
{
    AggregateMember* aggregateMember = dynamic_cast<AggregateMember*>(child);
    Member* member = dynamic_cast<Member*>(child);

    if(!member && !aggregateMember){
        qWarning() << "Aggregate can only adopt Member/Aggregates";
        return false;
    }

    //Check for loops

    return Node::canAdoptChild(child);
}
