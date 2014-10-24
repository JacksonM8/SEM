#ifndef OUTEVENTPORTINSTANCE_H
#define OUTEVENTPORTINSTANCE_H
#include "eventport.h"
#include "../InterfaceDefinitions/outeventport.h"

class OutEventPortInstance : public Node
{
        Q_OBJECT
public:
    OutEventPortInstance(QString name="");
    ~OutEventPortInstance();
    // GraphML interface
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    QString toString();
};

#endif // OUTEVENTPORTINSTANCE_H
