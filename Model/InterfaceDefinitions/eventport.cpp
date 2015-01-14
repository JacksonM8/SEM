#include "eventport.h"
#include "aggregate.h"
#include <QDebug>

EventPort::EventPort():Node(Node::NT_DEFINITION)
{
    aggregate = 0;
}

EventPort::~EventPort()
{

}

void EventPort::setAggregate(Aggregate *aggregate)
{
    if(getAggregate() != aggregate){
        this->aggregate = aggregate;
        aggregate->addEventPort(this);
    }
}

Aggregate *EventPort::getAggregate()
{
    return aggregate;
}

void EventPort::unsetAggregate()
{
    if(aggregate){
        aggregate->removeEventPort(this);
        aggregate = 0;
    }
}

bool EventPort::canConnect(Node* attachableObject)
{
    Aggregate* aggregate = dynamic_cast<Aggregate*>(attachableObject);

    if(aggregate && getAggregate()){
        qWarning() << "Can only connect an EventPort to one aggregate.";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool EventPort::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}
