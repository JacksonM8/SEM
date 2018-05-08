#ifndef DATA_H
#define DATA_H
#include "key.h"
#include "entity.h"
#include <QSet>

class EntityFactory; 
class Data : public GraphML
{
    Q_OBJECT
    friend class Entity;
    friend class EntityFactory;
    friend class Key;
    friend class Node;
    friend class Edge;
protected:
    
    Data(EntityFactory& factory, Key* key, QVariant value = QVariant(), bool protect = false);
    ~Data();
    static Data* clone(Data* data);
public:
    static bool CompareData(const Data* a, const Data* b);
    static bool SortByKey(const Data* a, const Data* b);
    Entity* getParent();

    void setProtected(bool protect);
    bool isProtected() const;



    bool setValue(QVariant value);
    
    bool linkData(Data* data, bool setup_bind);
    bool linkData(Data* data);
    bool unlinkData(Data* data);


    void registerParent(Entity* parent);
    
    bool isParentData(Data* data);
    //Data* getParentData();
    
    bool revalidateData();
    void clearValue();
    bool compare(const Data* data) const;


    Key* getKey() const;
    QString getKeyName() const;
    QVariant getValue() const;

    QString toGraphML(int indentDepth = 0, bool functional_export = false);
    QString toString() const;

    void addValidValue(QVariant value);
    void addValidValues(QList<QVariant> values);

    void removeValidValue(QVariant value);
    void clearValidValues();
    QList<QVariant> getValidValues();
    bool gotValidValues();
protected:

    void store_value();
    void restore_value();

    bool forceValue(QVariant value);

    void setParent(Entity* parent);
private:
    void addParentData(Data* data);
    void removeParentData(Data* data);
private:
    bool _setData(QVariant value);
    bool addChildData(Data* data);
    bool removeChildData(Data* data);
signals:
    void dataChanged(QVariant data);
private:
    bool _setValue(QVariant value, bool validate = true);
    void updateChildren(bool changed = true);
    
    Entity* parent = 0;
    Key* key = 0;

    QSet<Data*> parent_datas;
    QSet<Data*> child_datas;
    
    QString key_name;

    bool is_protected = false;
    bool is_data_linked = false;

    QList<QVariant> valid_values_;
    
    
    QVariant value;
    QVariant old_value;
};

#endif // DATA_H
