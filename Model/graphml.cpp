#include "graphml.h"
#include "graphmldata.h"
#include <qdebug>
int GraphML::_Uid = 0;

GraphML::GraphML(GraphML::KIND kind, QString name):QObject(0)
{
    this->Uid = ++_Uid;
    this->kind = kind;
    this->setName(name);
    setGenerated(false);
}

GraphML::~GraphML()
{
    removeData();
}


void GraphML::removeData(){
    while(attachedData.size() > 0){
        GraphMLData* data = attachedData.takeFirst();
        delete data;
    }
}

bool GraphML::wasGenerated()
{
    return generated;
}

void GraphML::setGenerated(bool isGenerated)
{
    generated = isGenerated;
}


void GraphML::setName(QString name)
{
    this->name = name;
}

GraphML::KIND GraphML::getKind() const
{
    return this->kind;
}

QString GraphML::getName() const
{
    return this->name;
}

QString GraphML::getID()
{
    return QString::number(Uid);
}


void GraphML::updateDataValue(QString keyName, QString value)
{
    GraphMLData* data = getData(keyName);
    if(data != 0){
        data->setValue(value);
    }else{
        qCritical() << "GraphML Object does not contain Data: " << keyName << "!";
    }
}

QString GraphML::getDataValue(QString keyName)
{
    GraphMLData* data = getData(keyName);

    if(data != 0){
        return data->getValue();
    }else{
        //qCritical() <<"GraphML::getDataValue() << GraphML Object does not contain Data: " << keyName;
        return "";
    }
}

GraphMLData *GraphML::getData(QString keyName)
{
    for(int i=0; i < attachedData.size(); i++){
        if(attachedData[i]->getKey() != 0){
            if(attachedData[i]->getKey()->getName() == keyName){
                return this->attachedData[i];
            }
        }
    }
    return 0;
}

GraphMLData *GraphML::getData(GraphMLKey *key)
{
    for(int i=0; i < attachedData.size(); i++){
        if(attachedData[i]->getKey() == key){
            return this->attachedData[i];
        }
    }
    return 0;

}

bool GraphML::containsData(GraphMLData *data)
{
    return attachedData.contains(data);
}


QList<GraphMLData *> GraphML::getData()
{
    return this->attachedData;
}



void GraphML::attachData(GraphMLData *data)
{
    if(data != 0){
        GraphML::KIND keyKind = data->getKey()->getForKind();

        if(keyKind == this->getKind() || (keyKind == GraphML::ALL && this->getKind() < GraphML::ALL)){

            foreach(GraphMLData* cData, attachedData){
                if(cData->getKey() == data->getKey()){
                    qCritical() << "Got Duplicate Data";
                    cData->setValue(data->getValue());
                    return;
                }
            }
            if(data){
                attachedData.append(data);
                emit dataAdded(data);
            }
        }else{
            qCritical() << "Cannot attach <data> to this object. Wrong Kind!";
            qCritical() << data->getKey()->getForKind() << " != " << this->getKind();
        }
    }
}

void GraphML::removeData(GraphMLData *data)
{
    int index = attachedData.indexOf(data);
    attachedData.removeAll(data);
    emit dataRemoved(data);
}

void GraphML::attachData(QList<GraphMLData *> data)
{
    while(!data.isEmpty()){
        attachData(data.takeFirst());
    }
}





