#include "key.h"

#include "entity.h"
#include "data.h"
#include "node.h"
#include "../nodekinds.h"

#include <QDebug>
#include <QStringBuilder>

QString Key::getGraphMLTypeName(const QVariant::Type type)
{
    if(type == QVariant::Bool){
        return "boolean";
    }else if(type == QVariant::Int){
        return "int";
    }else if(type == QVariant::LongLong){
        return "longlong";
    }else if(type == QVariant::Double){
        return "double";
    }
    return "string";
}

QVariant::Type Key::getTypeFromGraphML(const QString typeString)
{
    QString typeStringLow = typeString.toLower();

    if(typeString == "boolean"){
        return QVariant::Bool;
    }else if(typeString == "int"){
        return QVariant::Int;
    }else if(typeString == "long" || typeString == "longlong"){
        return QVariant::LongLong;
    }else if(typeString == "float" || typeString == "double"){
        return QVariant::Double;
    }else if(typeString == "string"){
        return QVariant::String;
    }

    return QVariant::nameToType(typeStringLow.toStdString().c_str());
}

Key::Key(QString key_name, QVariant::Type type):GraphML(GRAPHML_KIND::KEY)
{
    key_name_ = key_name;
    key_type_ = type;
}

Key::~Key()
{
}

void Key::setProtected(bool protect)
{
    is_protected_ = protect;
}

bool Key::isProtected() const
{
    return is_protected_;
}

QString Key::getName() const
{
    return key_name_;
}

QVariant::Type Key::getType() const
{
    return key_type_;
}

bool Key::forceDataValue(Data* data, QVariant value){
    bool result = false;
    if(data){
        result = data->forceValue(value);
    }
    return result;
}

void Key::setVisual(bool is_visual){
    is_visual_ = is_visual;
}

bool Key::isVisual() const{
    return is_visual_;
}

QVariant Key::validateDataChange(Data *data, QVariant new_value)
{
    if(!data || data->getKey() != this){
        return new_value;
    }

    Entity* entity = data->getParent();
    QVariant value = data->getValue();
    
    int ID = -1;
    if(entity){
        ID = entity->getID();
    }

    //Check if the value can be converted to this key type
    if(!new_value.canConvert(key_type_)){
        emit validation_failed(ID, "Value cannot be converted to Key's type.");
        return value;
    }else{
        new_value.convert(key_type_);
    }

    NODE_KIND node_kind = NODE_KIND::NONE;

    if(entity && entity->isNode()){
        node_kind = ((Node*)entity)->getNodeKind();
    }

    //If we have valid values, has to be one of these
    if(gotValidValues(node_kind)){
        auto valid_values = getValidValues(node_kind);
        if(!valid_values.contains(new_value)){
            emit validation_failed(ID, "Value not in list of valid values.");
            return value;
        }
    }

    return new_value;
}

bool Key::setData(Data* data, QVariant data_value){
    bool data_changed = false;
    if(data){
        auto valid_value = validateDataChange(data, data_value);
        data_changed = data->_setData(valid_value);
    }
    return data_changed;
}

QString Key::toGraphML(int indent_depth, bool functional_export)
{
    QString xml;
    bool should_export = !functional_export || !is_visual_;
    if(should_export){
        QTextStream stream(&xml); 
        QString tab = QString("\t").repeated(indent_depth);
        stream << tab << "<key attr.name=\"" << getName() << "\"";
        stream << " attr.type=\"" << getGraphMLTypeName(key_type_) << "\"";
        stream << " id=\"" << getID()<< "\"/>\n";
    }

    return xml;
}

QString Key::toString()
{
    QString str;
    QTextStream stream(&str); 
    //Todo get actual edge type
    stream << "[" << getID() << "] Key " << getName() << " Type: " << getGraphMLTypeName(key_type_);
    return str;
}


void Key::addValidValue(QVariant value, NODE_KIND kind){
    if(!valid_values_.contains(kind, value)){
        valid_values_.insert(kind, value);
        //qCritical() << "Key: " << getName() << " Added Valid Value: " << value;
    }
}

void Key::addValidValues(QList<QVariant> values, NODE_KIND kind){
    foreach(auto value, values){
        addValidValue(value, kind);
    }
}

bool Key::gotValidValues(NODE_KIND kind){
    return valid_values_.contains(kind);
}

QList<QVariant> Key::getValidValues(NODE_KIND kind){
    return valid_values_.values(kind);
}