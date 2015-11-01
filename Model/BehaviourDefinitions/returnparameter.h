#ifndef RETURNPARAMETER_H
#define RETURNPARAMETER_H
#include "parameter.h"


class ReturnParameter : public Parameter
{
public:
    ReturnParameter();

    // Node interface
public:
    bool canAdoptChild(Node *node);
    bool canConnect(Node *node);
};

#endif // RETURNPARAMETER_H
