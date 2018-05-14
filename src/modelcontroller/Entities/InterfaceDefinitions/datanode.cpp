#include "datanode.h"
#include "vectorinstance.h"
#include <QDebug>
#include "../edge.h"
#include "../Keys/typekey.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"


DataNode::DataNode(EntityFactoryBroker& broker, NODE_KIND node_kind, bool is_temp, bool use_complex_types) : Node(broker, node_kind, is_temp){
    setNodeType(NODE_TYPE::DATA);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    if(use_complex_types){
        broker.AttachData(this, "inner_type", QVariant::String, "", true);
        broker.AttachData(this, "outer_type", QVariant::String, "", true);
    }
    broker.AttachData(this, "type", QVariant::String, "", true);
    broker.AttachData(this, "value", QVariant::String, "", false);
};

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
    for(auto edge : getEdgesOfKind(EDGE_KIND::DATA, 0)){
        if(edge->getDestination() == this){
            return (DataNode*) edge->getSource();
        }
    }
    return 0;
}

DataNode *DataNode::getOutputData()
{
    for(auto edge : getEdgesOfKind(EDGE_KIND::DATA, 0)){
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

void DataNode::setMultipleDataProducer(bool producer)
{
    is_multiple_data_producer_ = producer;
    if(producer){
        setDataProducer(true);
    }
}

void DataNode::setDataProducer(bool producer)
{
    is_producer_ = producer;
    setAcceptsEdgeKind(EDGE_KIND::DATA, EDGE_DIRECTION::SOURCE, is_producer_);
}

void DataNode::setDataReceiver(bool receiver)
{
    is_receiver_ = receiver;
    setAcceptsEdgeKind(EDGE_KIND::DATA, EDGE_DIRECTION::TARGET, is_receiver_);
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

bool DataNode::isMultipleDataProducer() const
{
    return is_multiple_data_producer_;
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

bool DataNode::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }
    
    switch(edge_kind){
    case EDGE_KIND::DATA:{
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

        if(isContainedInVariable() && getNodeKind() == NODE_KIND::VECTOR){
            //Cannot data-link from a vector itself
            return false;
        }

        if(data_node->isContainedInVariable()){
            //Cannot data link into a variable
            return false;
        }


        if(!isPromiscuousDataLinker() && !data_node->isPromiscuousDataLinker()){
            auto source_containment_node = getContainmentNode();
            auto destination_containment_node = data_node->getContainmentNode();
            
            if(source_containment_node && destination_containment_node){
                auto source_contains_destination = source_containment_node->isAncestorOf(destination_containment_node);

                if(source_contains_destination){
                    //Check to see if we are trying to link into parameters, if the variables scoped parent is the same as the parameter,
                    //We shouldn't allow
                    if(data_node->isNodeOfType(NODE_TYPE::PARAMETER)){
                        if(data_node->getParentNode() == source_containment_node){
                            return false;
                        }
                    }
                }else{
                    //Source needs to contain the destination for scoping to work
                    return false;
                }

                auto src_child_of_containment_node = getChildOfContainmentNode();
                auto dst_child_of_containment_node = data_node->getChildOfContainmentNode();
                
                //Cannot inside the same workflow item
                if(src_child_of_containment_node == dst_child_of_containment_node){
                    return false;
                }
            }else{
                auto source_parent = getParentNode();

                if(!source_parent || !source_parent->isAncestorOf(data_node)){
                    return false;
                }
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
    return Node::canAcceptEdge(edge_kind, dst);
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

        Node* previous_node = this;
        for(auto parent_node : parent_nodes){
            if(!_containment_node){
                if(parent_node->isNodeOfType(NODE_TYPE::BEHAVIOUR_CONTAINER)){
                    _containment_node = parent_node;
                    _child_of_containment_node = previous_node;
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
            previous_node = parent_node;
        }
        _run_containment_checks = true;
    }
}

Node* DataNode::getContainmentNode(){
    RunContainmentChecks();
    return _containment_node;
}

Node* DataNode::getChildOfContainmentNode(){
    RunContainmentChecks();
    return _child_of_containment_node;
}

void DataNode::setPromiscuousDataLinker(bool set){
    promiscuous_data_linker_ = set;
}

bool DataNode::isPromiscuousDataLinker() const{
    return promiscuous_data_linker_;
}


void DataNode::BindDataRelationship(Node* source, Node* destination, bool setup){
    if(source && destination && source->isNodeOfType(NODE_TYPE::DATA) && destination->isNodeOfType(NODE_TYPE::DATA)){
        auto source_parent = source->getParentNode();
        auto destination_parent = destination->getParentNode();

        if(destination_parent){
            if(destination_parent->getNodeKind() == NODE_KIND::FUNCTION_CALL){
                auto worker_name = destination_parent->getDataValue("worker").toString();
                auto parameter_label = destination->getDataValue("label").toString();

                if(worker_name == "OpenCL_Worker" || worker_name == "Vector_Operations"){
                    for(auto param : destination_parent->getChildren(0)){
                        if(param->isNodeOfType(NODE_TYPE::PARAMETER)){
                            //Check if we are using generic params
                            auto is_generic_param = param->getDataValue("is_generic_param").toBool();
                            if(is_generic_param){
                                LinkData(source, "inner_type", param, "inner_type", setup);
                                if(!setup){
                                    param->setDataValue("inner_type", "");
                                }
                            }
                        }
                    }
                }
            }
            if(destination_parent->getNodeKind() == NODE_KIND::SETTER){
                for(auto param : destination_parent->getChildren(0)){
                    if(param->isNodeOfType(NODE_TYPE::PARAMETER)){
                        LinkData(source, "inner_type", param, "inner_type", setup);
                        LinkData(source, "outer_type", param, "outer_type", setup);
                    }
                }
            }
            
        }
        auto bind_source = source;
        auto source_key = "label";

        //Data bind to the Variable, instead of the Member
        if(source_parent && source_parent->getNodeKind() == NODE_KIND::VARIABLE){
            bind_source = source_parent;
        }

        //BIND LABEL
        //QSet<NODE_KIND> bind_labels = {NODE_KIND::VARIABLE, NODE_KIND::ATTRIBUTE_IMPL, NODE_KIND::ENUM_MEMBER, NODE_KIND::DEPLOYMENT_ATTRIBUTE, NODE_KIND::BOOLEAN_EXPRESSION};

        //if(bind_labels.contains(bind_source->getNodeKind())){
        //    source_key = "label";
        //}

        LinkData(bind_source, source_key, destination, "value", setup);
        TypeKey::BindInnerAndOuterTypes(bind_source, destination, setup);
    }
}
