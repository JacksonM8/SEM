#ifndef KEY_H
#define KEY_H
#include "graphml.h"


#include <QMultiMap>
#include <QVariant>

//Forward declare
class Data;
class Node;

class EntityFactoryRegistryBroker;
class Key : public GraphML
{
    Q_OBJECT
public:
    
    //Used for conversion to and from for export
    static QString getGraphMLTypeName(const QVariant::Type type);
    static QVariant::Type getTypeFromGraphML(const QString typeString);

    friend class EntityFactory;
protected:
    Key(EntityFactoryBroker& factory, const QString& keyName, QVariant::Type type);
    ~Key();
    void setProtected(bool protect);
    void setVisual(bool visible);
public:
    bool isVisual() const;
    bool isProtected() const;
    QString getName() const;
    QVariant::Type getType() const;


    virtual QVariant validateDataChange(Data* data, QVariant dataValue);
    virtual bool setData(Data* data, QVariant data_value);
    QString toGraphML(int indent_depth = 0, bool functional_export = false);
    QString toString() const;
protected:
    bool forceDataValue(Data* data, QVariant value);
signals:
    void validation_failed(int ID, QString error);
    void validateError(QString, QString, int);
private:
    QString key_name_;
    QVariant::Type key_type_;
    bool is_protected_ = false;
    bool is_visual_ = false;
};

#endif // KEY_H
