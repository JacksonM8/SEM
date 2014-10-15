#include "graphmldata.h"

GraphMLData::GraphMLData(GraphMLKey *key, QString value):GraphML(GraphML::DATA)
{
    this->key = key;
    if(value==""){
        this->setValue(this->key->getDefaultValue());
    }else{
        this->setValue(value);
    }

    //Set to default.
    this->setProtected(key->getProtected());
}

GraphMLData::~GraphMLData()
{

}
/*
void GraphMLData::setValue(QString value)
{
    this->value = value;
}
*/

QString GraphMLData::getValue() const
{
    return this->value;
}

GraphMLKey *GraphMLData::getKey()
{
    return this->key;
}

QString GraphMLData::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString returnable = tabSpace + QString("<data key=\"%1\">%2</data>\n").arg(this->getKey()->getID(), this->getValue());
    return returnable;
}

QString GraphMLData::toString()
{
    return QString("Data[%1]: "+this->getName()).arg(this->getID());
}

QStringList GraphMLData::toStringList()
{
     QStringList dataDump;
     dataDump << key->getName();
     dataDump << key->getTypeString();
     dataDump << key->getForKindString();
     dataDump << this->getValue();
     return dataDump;

}

void GraphMLData::setValue(QString value)
{
    if(value != this->value){
        this->value = value;
        emit dataChanged(this);
    }
}

void GraphMLData::setProtected(bool setProtected)
{
    isProtected = setProtected;
}

bool GraphMLData::getProtected()
{
    return isProtected;
}
