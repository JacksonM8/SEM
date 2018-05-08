#ifndef NAMESPACE_KEY_H
#define NAMESPACE_KEY_H
#include "../key.h"

class NamespaceKey : public Key
{
    Q_OBJECT
public:
    NamespaceKey(EntityFactory& factory);
    QVariant validateDataChange(Data* data, QVariant dataValue);

    static QString CombineNamespaces(QString namespace_1, QString namespace_2);
};


#endif //NAMESPACE_KEY_H
