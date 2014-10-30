#ifndef ASSEMBLYDEFINITIONS_H
#define ASSEMBLYDEFINITIONS_H

#include "../node.h"

class AssemblyDefinitions: public Node
{
    Q_OBJECT
public:
    AssemblyDefinitions();
    ~AssemblyDefinitions();


    // GraphML interface
public:
    QString toString();

    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};
#endif // AssemblyDefinitions_H
