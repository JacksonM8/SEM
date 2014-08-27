#include "inputeventport.h"
#include <QDebug>
#include <typeinfo>
#include "outputeventport.h"

InputEventPort::InputEventPort(QString name):EventPort(name)
{
    qDebug() << "Constructed InputEventPort: "<< this->getName();
}

InputEventPort::~InputEventPort()
{

}

bool InputEventPort::isAdoptLegal(GraphMLContainer *child)
{
    return true;
}

bool InputEventPort::isEdgeLegal(GraphMLContainer *attachableObject)
{
    OutputEventPort* outputEventPort = dynamic_cast<OutputEventPort*> (attachableObject);

    if(outputEventPort == 0){
        return false;
    }

    if(outputEventPort->getDataValue("type") != this->getDataValue("type")){
        qCritical() << "Cannot connect 2 Different IDL Types!";
        return false;
    }


    if(this->isConnected(attachableObject)){
        qCritical() << "Cannot connect 2 already connected ports!";
        return false;
    }

    return true;
}


QString InputEventPort::toString()
{
    return QString("InputEventPort[%1]: "+this->getName()).arg(this->getID());

}
