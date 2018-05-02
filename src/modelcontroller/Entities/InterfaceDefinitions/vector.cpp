#include "vector.h"
#include <QDebug>
#include "../data.h"
#include "../Keys/typekey.h"


Vector::Vector(EntityFactory* factory) : DataNode(factory, NODE_KIND::VECTOR, "Vector"){
	auto node_kind = NODE_KIND::VECTOR;
	QString kind_string = "Vector";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Vector();});

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "inner_type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "outer_type", QVariant::String, true, "Vector");
    RegisterDefaultData(factory, node_kind, "icon", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "icon_prefix", QVariant::String, true);

};

Vector::Vector(): DataNode(NODE_KIND::VECTOR)
{
    //Can be both an input/output for data.
    setDataProducer(true);
    setDataReceiver(true);

    addInstanceKind(NODE_KIND::VECTOR_INSTANCE);   

    setAcceptsNodeKind(NODE_KIND::MEMBER);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
}


bool Vector::canAdoptChild(Node *child)
{
    if(hasChildren()){
        return false;
    }
    return Node::canAdoptChild(child);
}

bool Vector::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::AGGREGATE:{
        if(dst->getNodeKind() != NODE_KIND::AGGREGATE){
            return false;
        }
        if(hasChildren()){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return DataNode::canAcceptEdge(edge_kind, dst);
}

void Vector::updateVectorIcon(Node* node){
    auto first_child = node->getFirstChild();

    auto child_kind = first_child ? first_child->getNodeKind() : NODE_KIND::NONE;
    
    auto icon = node->getDataValue("kind").toString();

    switch(child_kind){
        case NODE_KIND::NONE:
            break;
        case NODE_KIND::AGGREGATE_INSTANCE:
            icon += "_AggregateInstance";
            break;
        default:
            icon += "_Member";
            break;
    }
    node->setDataValue("icon_prefix", "EntityIcons");
    node->setDataValue("icon", icon);
}

void Vector::childAdded(Node* child){
    Vector::updateVectorIcon(this);

    DataNode::childAdded(child);
    TypeKey::BindInnerAndOuterTypes(child, this, true);
}

void Vector::childRemoved(Node* child){
    Vector::updateVectorIcon(this);
    DataNode::childRemoved(child);
    TypeKey::BindInnerAndOuterTypes(child, this, false);
}
