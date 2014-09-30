#include "graphmlcontainer.h"
#include <QDebug>
GraphMLContainer::GraphMLContainer(GraphML::KIND kind, QString name):GraphML(kind, name)
{
    parent = 0;
}

GraphMLContainer::~GraphMLContainer()
{
}

void GraphMLContainer::setParent(GraphMLContainer *parent)
{
    if(this->parent != 0){
        //detach old parent
        this->parent->disown(this);
    }
    this->parent = parent;

}

GraphMLContainer *GraphMLContainer::getParent()
{
    return this->parent;
}


Edge *GraphMLContainer::getEdge(GraphMLContainer *element)
{
    for (int i = 0; i < edges.size(); i++){
        if(this->edges[i]->contains(element)){
            return edges[i];
        }
    }
    return 0;
}

bool GraphMLContainer::isConnected(GraphMLContainer *element)
{
    if (getEdge(element) != 0){
        return true;
    }
    return false;
}

QVector<Edge *> GraphMLContainer::getEdges(int depth)
{
    QVector<Edge *> returnable;

    //Append this GraphMLContainers Edges which originate from here.
    for(int i=0; i < edges.size(); i++){
        if(edges[i]->getSource() == this || edges[i]->getDestination() == this){
            if(!returnable.contains(edges[i])){
                returnable.append(this->edges[i]);

            }
        }
    }

    //If depth is not 0, keep Recursing.
    if(depth != 0){
        for(int i=0; i < descendants.size(); i++){
            QVector<Edge *> toAppend = descendants.at(i)->getEdges(depth -1);
            for(int j=0; j < toAppend.size(); j++){
                if(!returnable.contains(toAppend[j])){
                    returnable.append(toAppend[j]);

                }
            }
        }
    }

    return returnable;
}

void GraphMLContainer::removeEdges()
{
    //Delete all Edges
    while(!edges.isEmpty()){
        Edge* current = edges.first();
        edges.removeFirst();
        delete current;
    }
}

void GraphMLContainer::removeChildren()
{
    //Delete all Children
    while(!descendants.isEmpty()){
        GraphMLContainer* current = descendants.first();
        descendants.removeFirst();
        delete current;
    }
}


void GraphMLContainer::adopt(GraphMLContainer *child)
{
    child->setParent(this);
    this->descendants.append(child);
}

void GraphMLContainer::disown(GraphMLContainer *child)
{
       if(isAncestorOf(child)){
        for (int i = 0; i < descendants.size(); i++){
            if(descendants[i] == child){
                //delete descendants[i];
                descendants.removeAt(i);
                return;
            }
        }
        qCritical() <<  "Couldn't find child!";
    }
}

bool GraphMLContainer::isAncestorOf(GraphMLContainer *element)
{
    QVector<GraphMLContainer *> ancestors = this->getChildren(-1);
    return ancestors.contains(element);
}

bool GraphMLContainer::isDescendantOf(GraphMLContainer *element)
{
    if(parent != 0){
        //Check if the parent is the element we are looking for;
        //Check if the element is a cousin.
        if(parent == element || parent->descendants.contains(element)){
            return true;
        }else{
            //Check up the tree.
            return parent->isDescendantOf(element);
        }
    }
    return false;

}

QVector<GraphMLContainer *> GraphMLContainer::getChildren(int depth)
{
    QVector<GraphMLContainer *> returnable;

    if(depth != 0){
        for(int i=0; i < this->descendants.size(); i++){
            returnable += this->descendants.at(i);
            returnable +=  this->descendants.at(i)->getChildren(depth - 1 );
        }
    }else{
        returnable += this->descendants;
    }
    return returnable;

}


QVector<GraphMLKey *> GraphMLContainer::getKeys(int depth)
{
    QVector<GraphMLContainer *> children = getChildren(depth);

    QVector<GraphMLKey *> allKeys;

    QVector<GraphMLData *> data = getData();
    for(int i=0; i < data.size(); i++){
        if(!allKeys.contains(data[i]->getKey())){
            allKeys.append(data[i]->getKey());
        }
    }

    for(int i=0; i < children.size(); i++){
        GraphMLContainer* child = children[i];

        QVector<GraphMLData *> data = child->getData();
        for(int j=0; j < data.size(); j++){
            if(!allKeys.contains(data[j]->getKey())){
                allKeys.append(data[j]->getKey());
            }
        }

    }

    return allKeys;

}


void GraphMLContainer::addEdge(Edge *edge)
{
    edges.append(edge);
}

void GraphMLContainer::removeEdge(Edge *edge)
{
    if(this->edges.contains(edge)){
        for (int i = 0; i < this->edges.size(); i++){
            if(this->edges[i] == edge){
                edges.removeAt(i);
            }
        }
    }else{
        qDebug() <<  "Couldn't find Edge!";
    }


}
