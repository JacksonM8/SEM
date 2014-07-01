#include "graph.h"
#include <QDebug>
#include "node.h"

Graph::Graph(QString name):GraphMLContainer(GraphML::GRAPH, name)
{
    qDebug() << "Constructed Graph["<< this->getName() <<"]";
}

bool Graph::isAdoptLegal(GraphMLContainer *child)
{
    //Check for self connection.
    if(child == this){
        qWarning() << "Cannot adopt self.";
        return false;
    }

    //Graph Objects can only contain Node Type objects.
    if(child->getKind() != GraphML::NODE){
        qWarning() << "Cannot adopt non-Node type GraphML Objects.";
        return false;
    }

    //Check if the GraphML object is already owned by this object, or a child of this object.
    if(this->isDescendantOf(child)){
        qWarning() << "Cannot adopt a Node object which is already a descendant";
        return false;
    }

     //Check if the GraphML object is a parent of this
    if(this->isAncestorOf(child)){
        qWarning() << "Cannot adopt a Node object which is already an ancestor.";
        return false;
    }

    return true;
}

bool Graph::isEdgeLegal(GraphMLContainer *attachableObject)
{
    //Check for self connection.
    if(attachableObject == this){
        qWarning() << "Graph cannot connected to self!";
        return false;
    }

    //Only Items that aren't connected to any view?!
    return true;
}

QString Graph::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString returnable = "";
    if(this->descendants.size() > 0){
        
        //If this is the parent Graph, we need to specify the edge type.
        QString edgeType="edgedefault=\"directed\"";
        
        returnable = tabSpace + QString("<graph %2 id =\"%1\">\n").arg(this->getName(),edgeType);

        for(int i=0; i < this->descendants.size();i++){
            returnable += this->descendants[i]->toGraphML(indentationLevel+1);
        }
        returnable += tabSpace + "</graph>\n";
    }
    return returnable;
}

QString Graph::toString()
{
    return QString("Graph[%1]: "+this->getName()).arg(this->getID());
}
