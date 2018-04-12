#include "datanode.h"
#include "vectorinstance.h"
#include <QDebug>
#include "../edge.h"
#include "../Keys/typekey.h"


DataNode::DataNode(EntityFactory* factory, NODE_KIND kind, QString kind_str) : Node(factory, kind, kind_str){

};

DataNode::DataNode(NODE_KIND kind):Node(kind)
{
    
    setNodeType(NODE_TYPE::DATA);
}

bool DataNode::hasInputData()
{
   return getInputData();
}

bool DataNode::hasOutputData()
{
    return getOutputData();
}

DataNode *DataNode::getInputData()
{
    for(auto edge : getEdges(0, EDGE_KIND::DATA)){
        if(edge->getDestination() == this){
            return (DataNode*) edge->getSource();
        }
    }
    return 0;
}

DataNode *DataNode::getOutputData()
{
    for(auto edge : getEdges(0, EDGE_KIND::DATA)){
        if(edge->getSource() == this){
            return (DataNode*) edge->getDestination();
        }
    }
    return 0;
}

void DataNode::setMultipleDataReceiver(bool receiver)
{
    is_multiple_data_receiver_ = receiver;
    if(receiver){
        setDataReceiver(true);
    }
}

void DataNode::setDataProducer(bool producer)
{
    is_producer_ = producer;
    setAcceptsEdgeKind(EDGE_KIND::DATA, isDataReceiver() || isDataProducer());
}

void DataNode::setDataReceiver(bool receiver)
{
    is_receiver_ = receiver;
    setAcceptsEdgeKind(EDGE_KIND::DATA, isDataReceiver() || isDataProducer());
}

bool DataNode::isDataProducer() const
{
    return is_producer_;
}

bool DataNode::isDataReceiver() const
{
    return is_receiver_;
}

bool DataNode::isMultipleDataReceiver() const
{
    return is_multiple_data_receiver_;
}

bool DataNode::comparableTypes(DataNode *node)
{
    QStringList numberTypes;
    numberTypes << "Float" << "Double" << "Integer" << "Boolean";

    if(node){
        if(node->getNodeKind() == NODE_KIND::VARIADIC_PARAMETER){
           return true;
        }

        //Types
        QString type1 = getDataValue("type").toString();
        QString type2 = node->getDataValue("type").toString();

        if(type1 == type2 && type1 != ""){
            //Allow direct matches.
            return true;
        }

        if(numberTypes.contains(type1) && numberTypes.contains(type2)){
            //Allow matches of numbers
            return true;
        }


        if(type2 == "Vector"){
            auto kind = getNodeKind();
            if(kind == NODE_KIND::VECTOR || kind == NODE_KIND::VECTOR_INSTANCE){
               return true;
            }
        }
        if(type1 == "Vector"){
            auto kind = node->getNodeKind();
            if(kind == NODE_KIND::VECTOR || kind == NODE_KIND::VECTOR_INSTANCE){
               return true;
            }
        }
        if(type2 == ""){
            return true;
        }
    }
    return false;

}

bool DataNode::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    
    switch(edgeKind){
    case EDGE_KIND::DATA:{
        qCritical() << this->toString() << " -> " << dst->toString();
        if(dst->isNodeOfType(NODE_TYPE::DATA) == false){
            //Cannot connect to a non DataNode type.
            return false;
        }

        DataNode* data_node = (DataNode*) dst;

        if(isDataProducer() == false){
            //Cannot connect from something which can't produce
            return false;
        }

        if(data_node->isDataReceiver() == false){
            //Cannot connect to something which can't recieve
            return false;
        }

        if(data_node->hasInputData() && data_node->isMultipleDataReceiver() != true){
            //Cannot have multiple input datas.
            return false;
        }

        if(isContainedInVector() || data_node->isContainedInVector()){
            //Cannot Data-Connect inside a vector.
            return false;
        }

        //if(data_node->isContainedInVariable()){
            //Cannot Data-Connect into a Variable
            //return false;
        //}


        if(!isPromiscuousDataLinker() && !data_node->isPromiscuousDataLinker()){
            auto source_containment_node = getContainmentNode();
            auto destination_containment_node = data_node->getContainmentNode();
            
            if(source_containment_node && destination_containment_node){
                auto source_contains_destination = source_containment_node->isAncestorOf(destination_containment_node);
                auto destination_contains_source = destination_containment_node->isAncestorOf(source_containment_node);

                //One of those needs to be true
                if(!source_contains_destination && !destination_contains_source){
                    return false;
                    //The Variable we are setting needs to be in scope.
                }
            }else if(!source_containment_node && !destination_containment_node){
                auto source_parent = getParentNode();

                if(!source_parent || !source_parent->isAncestorOf(data_node)){
                    return false;
                }
            }else{
                return false;
            }
        }

        

        if(TypeKey::CompareTypes(this, data_node) == false){
            //Must have compareable types
            return false;
        }
        
        break;
        }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);
}

bool DataNode::isContainedInVector(){
    RunContainmentChecks();
    return _contained_in_vector;
}

bool DataNode::isContainedInVariable(){
    RunContainmentChecks();
    return _contained_in_variable;
}


void DataNode::RunContainmentChecks(){
    if(getParentNode() && !_run_containment_checks){
        auto parent_nodes = getParentNodes(-1);
        //parent_nodes.push_front(this);
        //Check if we are inside a vector
        for(auto parent_node : parent_nodes){
            if(!_containment_node){
                if(parent_node->isNodeOfType(NODE_TYPE::BEHAVIOUR_CONTAINER)){
                    _containment_node = parent_node;
                }
            }

            switch(parent_node->getNodeKind()){
                case NODE_KIND::VARIABLE:{
                    _contained_in_variable = true;
                    break;
                }
                case NODE_KIND::VECTOR:
                case NODE_KIND::VECTOR_INSTANCE:{
                    _contained_in_vector = true;
                    break;
                }
                default:
                    break;
            }
        }
        _run_containment_checks = true;
    }
}

Node* DataNode::getContainmentNode(){
    RunContainmentChecks();
    return _containment_node;
}

void DataNode::setPromiscuousDataLinker(bool set){
    promiscuous_data_linker_ = set;
}

bool DataNode::isPromiscuousDataLinker() const{
    return promiscuous_data_linker_;
}