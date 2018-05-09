#include "entityfactory.h"

#include <iostream>
#include <exception>

#include <QDebug>
#include <algorithm>


#include "Entities/key.h"
#include "Entities/data.h"

//Keys
#include "Entities/Keys/labelkey.h"
#include "Entities/Keys/indexkey.h"
#include "Entities/Keys/exportidkey.h"
#include "Entities/Keys/replicatecountkey.h"
#include "Entities/Keys/frequencykey.h"
#include "Entities/Keys/rowkey.h"
#include "Entities/Keys/columnkey.h"
#include "Entities/Keys/typekey.h"
#include "Entities/Keys/innertypekey.h"
#include "Entities/Keys/outertypekey.h"
#include "Entities/Keys/namespacekey.h"

//Model Includes

//Aspects
#include "Entities/model.h"
#include "Entities/workerdefinitions.h"
#include "Entities/BehaviourDefinitions/behaviourdefinitions.h"
#include "Entities/DeploymentDefinitions/assemblydefinitions.h"
#include "Entities/DeploymentDefinitions/deploymentdefinitions.h"
#include "Entities/DeploymentDefinitions/hardwaredefinitions.h"
#include "Entities/InterfaceDefinitions/interfacedefinitions.h"

//Impl Elements
#include "Entities/BehaviourDefinitions/componentimpl.h"
#include "Entities/BehaviourDefinitions/attributeimpl.h"
#include "Entities/BehaviourDefinitions/ineventportimpl.h"
#include "Entities/BehaviourDefinitions/outeventportimpl.h"

//Behaviour Elements
#include "Entities/BehaviourDefinitions/IfStatement/ifstatement.h"
#include "Entities/BehaviourDefinitions/IfStatement/ifcondition.h"
#include "Entities/BehaviourDefinitions/IfStatement/elseifcondition.h"
#include "Entities/BehaviourDefinitions/IfStatement/elsecondition.h"


#include "Entities/BehaviourDefinitions/Loops/whileloop.h"
#include "Entities/BehaviourDefinitions/Loops/forloop.h"



#include "Entities/BehaviourDefinitions/code.h"
#include "Entities/BehaviourDefinitions/header.h"
#include "Entities/BehaviourDefinitions/inputparameter.h"
#include "Entities/BehaviourDefinitions/periodicevent.h"
#include "Entities/BehaviourDefinitions/returnparameter.h"
#include "Entities/BehaviourDefinitions/setter.h"
#include "Entities/BehaviourDefinitions/variable.h"
#include "Entities/BehaviourDefinitions/variableparameter.h"
#include "Entities/BehaviourDefinitions/variadicparameter.h"
#include "Entities/BehaviourDefinitions/functioncall.h"
#include "Entities/BehaviourDefinitions/externaltype.h"
#include "Entities/BehaviourDefinitions/booleanexpression.h"

//Instance Elements
#include "Entities/DeploymentDefinitions/componentinstance.h"
#include "Entities/DeploymentDefinitions/attributeinstance.h"
#include "Entities/DeploymentDefinitions/ineventportinstance.h"
#include "Entities/DeploymentDefinitions/outeventportinstance.h"

#include "Entities/DeploymentDefinitions/deploymentattribute.h"

#include "Entities/InterfaceDefinitions/aggregateinstance.h"
#include "Entities/InterfaceDefinitions/memberinstance.h"
#include "Entities/InterfaceDefinitions/vectorinstance.h"

#include "Entities/InterfaceDefinitions/enum.h"
#include "Entities/InterfaceDefinitions/enuminstance.h"
#include "Entities/InterfaceDefinitions/enummember.h"


//Deployment Elements
#include "Entities/DeploymentDefinitions/componentassembly.h"
#include "Entities/DeploymentDefinitions/hardwarecluster.h"
#include "Entities/DeploymentDefinitions/hardwarenode.h"
#include "Entities/DeploymentDefinitions/ineventportdelegate.h"
#include "Entities/DeploymentDefinitions/outeventportdelegate.h"
#include "Entities/DeploymentDefinitions/loggingprofile.h"
#include "Entities/DeploymentDefinitions/loggingserver.h"
#include "Entities/DeploymentDefinitions/openclplatform.h"
#include "Entities/DeploymentDefinitions/opencldevice.h"

//Definition Elements
#include "Entities/InterfaceDefinitions/aggregate.h"
#include "Entities/InterfaceDefinitions/attribute.h"
#include "Entities/InterfaceDefinitions/component.h"
#include "Entities/InterfaceDefinitions/ineventport.h"
#include "Entities/InterfaceDefinitions/member.h"
#include "Entities/InterfaceDefinitions/outeventport.h"
#include "Entities/InterfaceDefinitions/vector.h"

//Elements
#include "Entities/InterfaceDefinitions/shareddatatypes.h"
#include "Entities/InterfaceDefinitions/namespace.h"

#include "Entities/BehaviourDefinitions/class.h"
#include "Entities/BehaviourDefinitions/classinstance.h"
#include "Entities/BehaviourDefinitions/function.h"

#include "Entities/InterfaceDefinitions/ClientServer/serverinterface.h"
#include "Entities/InterfaceDefinitions/ClientServer/clientport.h"
#include "Entities/InterfaceDefinitions/ClientServer/serverport.h"

#include "Entities/BehaviourDefinitions/ClientServer/serverrequest.h"
#include "Entities/BehaviourDefinitions/ClientServer/serverportimpl.h"

#include "Entities/DeploymentDefinitions/ClientServer/serverportinstance.h"
#include "Entities/DeploymentDefinitions/ClientServer/clientportinstance.h"

#include "Entities/InterfaceDefinitions/inputparametergroup.h"
#include "Entities/InterfaceDefinitions/returnparametergroup.h"

#include "Entities/InterfaceDefinitions/inputparametergroupinstance.h"
#include "Entities/InterfaceDefinitions/returnparametergroupinstance.h"

#include "Entities/InterfaceDefinitions/voidtype.h"




//QOS Elements
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_qosprofile.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_deadlineqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_destinationorderqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_durabilityqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_durabilityserviceqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_entityfactoryqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_groupdataqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_historyqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_latencybudgetqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_lifespanqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_livelinessqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_ownershipqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_ownershipstrengthqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_partitionqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_presentationqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_readerdatalifecycleqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_reliabilityqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_resourcelimitsqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_timebasedfilterqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_topicdataqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_transportpriorityqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_userdataqospolicy.h"
#include "Entities/DeploymentDefinitions/QOS/DDS/dds_writerdatalifecycleqospolicy.h"

//Edges
#include "Entities/Edges/aggregateedge.h"
#include "Entities/Edges/assemblyedge.h"
#include "Entities/Edges/dataedge.h"
#include "Entities/Edges/definitionedge.h"
#include "Entities/Edges/deploymentedge.h"
#include "Entities/Edges/qosedge.h"

EntityFactory* EntityFactory::global_factory = 0;


Node *EntityFactory::CreateTempNode(NODE_KIND nodeKind)
{
    return _createNode(nodeKind, true, false);
}

Node *EntityFactory::CreateNode(NODE_KIND node_kind, bool complex)
{
    return _createNode(node_kind, false, complex);
}


Edge *EntityFactory::CreateEdge(Node *source, Node *destination, EDGE_KIND edge_kind)
{
    return _createEdge(source, destination, edge_kind);
}


QString EntityFactory::getNodeKindString(NODE_KIND kind)
{
    QString kind_str;
    auto node_struct = globalFactory()->getNodeStruct(kind);
    if(node_struct){
        kind_str = node_struct->kind_str;
    }
    return kind_str;
}

QList<VIEW_ASPECT> EntityFactory::getViewAspects(){
    return {
        VIEW_ASPECT::INTERFACES,
        VIEW_ASPECT::BEHAVIOUR,
        VIEW_ASPECT::ASSEMBLIES,
        VIEW_ASPECT::HARDWARE,
        VIEW_ASPECT::WORKERS
    };
}

NODE_KIND EntityFactory::getViewAspectKind(VIEW_ASPECT aspect){
    switch(aspect){
        case VIEW_ASPECT::INTERFACES:
            return NODE_KIND::INTERFACE_DEFINITIONS;
        case VIEW_ASPECT::BEHAVIOUR:
            return NODE_KIND::BEHAVIOUR_DEFINITIONS;
        case VIEW_ASPECT::ASSEMBLIES:
            return NODE_KIND::ASSEMBLY_DEFINITIONS;
        case VIEW_ASPECT::HARDWARE:
            return NODE_KIND::HARDWARE_DEFINITIONS;
        case VIEW_ASPECT::WORKERS:
            return NODE_KIND::WORKER_DEFINITIONS;
        default:
            return NODE_KIND::NONE;
    }
}

QString EntityFactory::getEdgeKindString(EDGE_KIND kind)
{
    QString kind_str = "INVALID_EDGE";
    auto edge_struct = globalFactory()->getEdgeStruct(kind);
    if(edge_struct){
        kind_str = edge_struct->kind_str;
    }
    return kind_str;
}

QList<NODE_KIND> EntityFactory::getNodeKinds()
{
    return globalFactory()->node_struct_lookup.keys();
}

QList<EDGE_KIND> EntityFactory::getEdgeKinds()
{
    return globalFactory()->edge_struct_lookup.keys();
}


NODE_KIND EntityFactory::getNodeKind(QString kind)
{
    return globalFactory()->node_kind_lookup.value(kind, NODE_KIND::NONE);
}

EDGE_KIND EntityFactory::getEdgeKind(QString kind)
{
    return globalFactory()->edge_kind_lookup.value(kind, EDGE_KIND::NONE);
}

void EntityFactory::RegisterNodeKind2(const NODE_KIND kind, const QString& kind_string, std::function<Node* (EntityFactory&, bool)> constructor){
    if(doesNodeStructExist(kind)){
        throw std::invalid_argument("Trying to Register duplicate Node with kind '" + kind_string.toStdString() + "'");
    }

    auto node = getNodeStruct(kind);

    if(node){
        qCritical() << this << "REGISTERING NODE[" << (uint)kind << "]: " << kind_string;
        node->kind_str = kind_string;
        node->constructor = constructor;

        //Insert into our reverse lookup
        if(!node_kind_lookup.contains(kind_string)){
            node_kind_lookup.insert(kind_string, kind);
        }
    }
}
void EntityFactory::RegisterEdgeKind2(const EDGE_KIND kind, const QString& kind_string, std::function<Edge* (EntityFactory&,Node*, Node*)> constructor){
    auto edge = getEdgeStruct(kind);
    if(edge){
        edge->kind_str = kind_string;
        edge->constructor = constructor;

        //Insert into our reverse lookup
        if(!edge_kind_lookup.contains(kind_string)){
            edge_kind_lookup.insert(kind_string, kind);
        }
    }
}



void EntityFactory::RegisterEdgeKind(EDGE_KIND kind, QString kind_string, std::function<Edge* (Node*, Node*)> constructor){
    auto edge = getEdgeStruct(kind);
    if(edge){
        edge->kind = kind;
        edge->kind_str = kind_string;
        //edge->constructor = constructor;

        //Insert into our reverse lookup
        if(!edge_kind_lookup.contains(kind_string)){
            edge_kind_lookup.insert(kind_string, kind);
        }
    }
}

QSet<Node*> EntityFactory::GetNodesWhichAcceptEdgeKinds(EDGE_KIND edge_kind, EDGE_DIRECTION direction){
    auto& map = direction == EDGE_DIRECTION::SOURCE ? accepted_source_edge_map : accepted_target_edge_map;
    return map.value(edge_kind);
}


EntityFactory::EntityFactory()
{
    auto& ref = *this;
    Model::RegisterWithEntityFactory(ref);

    BehaviourDefinitions::RegisterWithEntityFactory(ref);
    AssemblyDefinitions::RegisterWithEntityFactory(ref);
    DeploymentDefinitions::RegisterWithEntityFactory(ref);
    HardwareDefinitions::RegisterWithEntityFactory(ref);
    WorkerDefinitions::RegisterWithEntityFactory(ref);
    InterfaceDefinitions::RegisterWithEntityFactory(ref);

    //Impl Elements
    ComponentImpl::RegisterWithEntityFactory(ref);
    AttributeImpl::RegisterWithEntityFactory(ref);
    InEventPortImpl::RegisterWithEntityFactory(ref);
    OutEventPortImpl::RegisterWithEntityFactory(ref);

    //Behaviour Elements
    MEDEA::IfStatement::RegisterWithEntityFactory(ref);
    MEDEA::IfCondition::RegisterWithEntityFactory(ref);
    MEDEA::ElseIfCondition::RegisterWithEntityFactory(ref);
    MEDEA::ElseCondition::RegisterWithEntityFactory(ref);

    MEDEA::WhileLoop::RegisterWithEntityFactory(ref);
    MEDEA::ForLoop::RegisterWithEntityFactory(ref);
    MEDEA::ExternalType::RegisterWithEntityFactory(ref);

    Code::RegisterWithEntityFactory(ref);
    Header::RegisterWithEntityFactory(ref);
    InputParameter::RegisterWithEntityFactory(ref);
    PeriodicEvent::RegisterWithEntityFactory(ref);
    ReturnParameter::RegisterWithEntityFactory(ref);
    Setter::RegisterWithEntityFactory(ref);
    Variable::RegisterWithEntityFactory(ref);
    VariableParameter::RegisterWithEntityFactory(ref);
    VariadicParameter::RegisterWithEntityFactory(ref);
    
    
    FunctionCall::RegisterWithEntityFactory(ref);

    //Instance Elements
    ComponentInstance::RegisterWithEntityFactory(ref);
    AttributeInstance::RegisterWithEntityFactory(ref);
    InEventPortInstance::RegisterWithEntityFactory(ref);
    OutEventPortInstance::RegisterWithEntityFactory(ref);
    AggregateInstance::RegisterWithEntityFactory(ref);
    MemberInstance::RegisterWithEntityFactory(ref);
    VectorInstance::RegisterWithEntityFactory(ref);
    
    //Deployment Elements
    ComponentAssembly::RegisterWithEntityFactory(ref);
    HardwareNode::RegisterWithEntityFactory(ref);
    HardwareCluster::RegisterWithEntityFactory(ref);
    InEventPortDelegate::RegisterWithEntityFactory(ref);
    OutEventPortDelegate::RegisterWithEntityFactory(ref);
    LoggingProfile::RegisterWithEntityFactory(ref);
    LoggingServer::RegisterWithEntityFactory(ref);
    
    OpenCLDevice::RegisterWithEntityFactory(ref);
    OpenCLPlatform::RegisterWithEntityFactory(ref);

    //Definition Elements
    Aggregate::RegisterWithEntityFactory(ref);
    Attribute::RegisterWithEntityFactory(ref);
    Component::RegisterWithEntityFactory(ref);
    InEventPort::RegisterWithEntityFactory(ref);
    Member::RegisterWithEntityFactory(ref);
    OutEventPort::RegisterWithEntityFactory(ref);
    Vector::RegisterWithEntityFactory(ref);


    Enum::RegisterWithEntityFactory(ref);
    EnumMember::RegisterWithEntityFactory(ref);
    EnumInstance::RegisterWithEntityFactory(ref);
    MEDEA::Class::RegisterWithEntityFactory(ref);
    MEDEA::ClassInstance::RegisterWithEntityFactory(ref);
    MEDEA::Function::RegisterWithEntityFactory(ref);
    MEDEA::DeploymentAttribute::RegisterWithEntityFactory(ref);

    //QOS Profiles
    
    DDS_QOSProfile::RegisterWithEntityFactory(ref);
    DDS_DeadlineQosPolicy::RegisterWithEntityFactory(ref);
    DDS_DestinationOrderQosPolicy::RegisterWithEntityFactory(ref);
    DDS_DurabilityQosPolicy::RegisterWithEntityFactory(ref);
    DDS_DurabilityServiceQosPolicy::RegisterWithEntityFactory(ref);
    DDS_EntityFactoryQosPolicy::RegisterWithEntityFactory(ref);
    DDS_GroupDataQosPolicy::RegisterWithEntityFactory(ref);
    DDS_HistoryQosPolicy::RegisterWithEntityFactory(ref);
    DDS_LatencyBudgetQosPolicy::RegisterWithEntityFactory(ref);
    DDS_LifespanQosPolicy::RegisterWithEntityFactory(ref);
    DDS_LivelinessQosPolicy::RegisterWithEntityFactory(ref);
    DDS_OwnershipQosPolicy::RegisterWithEntityFactory(ref);
    DDS_OwnershipStrengthQosPolicy::RegisterWithEntityFactory(ref);
    DDS_PartitionQosPolicy::RegisterWithEntityFactory(ref);
    DDS_PresentationQosPolicy::RegisterWithEntityFactory(ref);
    DDS_ReaderDataLifecycleQosPolicy::RegisterWithEntityFactory(ref);
    DDS_ReliabilityQosPolicy::RegisterWithEntityFactory(ref);
    DDS_ResourceLimitsQosPolicy::RegisterWithEntityFactory(ref);
    DDS_TimeBasedFilterQosPolicy::RegisterWithEntityFactory(ref);
    DDS_TopicDataQosPolicy::RegisterWithEntityFactory(ref);
    DDS_TransportPriorityQosPolicy::RegisterWithEntityFactory(ref);
    DDS_UserDataQosPolicy::RegisterWithEntityFactory(ref);
    DDS_WriterDataLifecycleQosPolicy::RegisterWithEntityFactory(ref);

    //Elements
    SharedDatatypes::RegisterWithEntityFactory(ref);
    Namespace::RegisterWithEntityFactory(ref);

    MEDEA::ServerInterface::RegisterWithEntityFactory(ref);
    MEDEA::ServerPort::RegisterWithEntityFactory(ref);
    MEDEA::ClientPort::RegisterWithEntityFactory(ref);

    MEDEA::ServerPortInstance::RegisterWithEntityFactory(ref);
    MEDEA::ClientPortInstance::RegisterWithEntityFactory(ref);

    MEDEA::ServerPortImpl::RegisterWithEntityFactory(ref);
    MEDEA::ServerRequest::RegisterWithEntityFactory(ref);
    
    MEDEA::BooleanExpression::RegisterWithEntityFactory(ref);


    MEDEA::InputParameterGroup::RegisterWithEntityFactory(ref);
    MEDEA::ReturnParameterGroup::RegisterWithEntityFactory(ref);

    MEDEA::InputParameterGroupInstance::RegisterWithEntityFactory(ref);
    MEDEA::ReturnParameterGroupInstance::RegisterWithEntityFactory(ref);

    VoidType::RegisterWithEntityFactory(ref);

    //Edges
    DefinitionEdge::RegisterWithEntityFactory(ref);
    AggregateEdge::RegisterWithEntityFactory(ref);
    AssemblyEdge::RegisterWithEntityFactory(ref);
    DataEdge::RegisterWithEntityFactory(ref);
    DeploymentEdge::RegisterWithEntityFactory(ref);
    QosEdge::RegisterWithEntityFactory(ref);
}

EntityFactory::~EntityFactory()
{
    
    //Delete the keys
    for(auto key : key_lookup_.values()){
        DestructEntity(key);
    }
}

EntityFactory *EntityFactory::globalFactory()
{
    if(global_factory == 0){
        global_factory = new EntityFactory();
    }
    return global_factory;
}

EntityFactory *EntityFactory::getNewFactory()
{
    return new EntityFactory();
}
bool EntityFactory::doesNodeStructExist(NODE_KIND kind){
    return node_struct_lookup.contains(kind);
}

EntityFactory::NodeLookupStruct* EntityFactory::getNodeStruct(NODE_KIND kind){
    auto node = node_struct_lookup.value(kind, 0);
    if(!node){
        node = new NodeLookupStruct();
        node->kind = kind;
        node_struct_lookup.insert(kind, node);
    }
    return node;
}

EntityFactory::EdgeLookupStruct* EntityFactory::getEdgeStruct(EDGE_KIND kind){
    auto edge = edge_struct_lookup.value(kind, 0);
    if(!edge && kind != EDGE_KIND::NONE){
        edge = new EdgeLookupStruct();
        edge->kind = kind;
        edge_struct_lookup.insert(kind, edge);
    }
    return edge;
}



Edge *EntityFactory::_createEdge(Node *source, Node *destination, EDGE_KIND kind)
{
    Edge* edge = 0;

    auto edge_struct = getEdgeStruct(kind);
  
    if(edge_struct && edge_struct->constructor){
        try{
            edge = edge_struct->constructor(*this, source, destination);
        }catch(const std::exception& ex){
            std::cerr << ex.what() << std::endl;
            edge = 0;
        }
    }

    CacheEntityAsUnregistered(edge);
    return edge;
}


Node *EntityFactory::_createNode(NODE_KIND kind, bool is_temporary, bool use_complex)
{
    Node* node = 0;
    auto node_struct = getNodeStruct(kind);

    if(node_struct){
        qCritical() << "Trying to Construct node: " << this << " " << (uint)kind << " " << getNodeKindString(kind);//kind_string;
        
        if(node_struct->constructor){
            try{
                node = node_struct->constructor(*this, is_temporary);
            }catch(const std::exception& ex){
                std::cerr << ex.what() << std::endl;
                node = 0;
            }
        }

        if(node){
            if(!is_temporary){
                CacheEntityAsUnregistered(node);
            }
        }else{
            auto node_kind_str = getNodeKindString(kind);
            if(node_kind_str == "INVALID_NODE"){
                node_kind_str = "NODE_KIND: '" + QString::number((uint)kind) + "'";            
            }
            qCritical() << "EntityFactory: Node Kind: " << node_kind_str << " cannot be created!";
        }
    }else{
        qCritical() << "EntityFactory: Node Kind: " << QString::number((uint)kind) << " Not Registered";
    }
       
    return node;
}


Key *EntityFactory::GetKey(QString key_name)
{
    return key_lookup_.value(key_name, 0);
}

QSet<QString> VisualKeyNames(){
    return {"x", "y", "width", "height", "isExpanded", "readOnly"};
}

Key *EntityFactory::GetKey(QString key_name, QVariant::Type type)
{
    if(key_lookup_.contains(key_name)){
        return key_lookup_[key_name];
    }else{
        Key* key = 0;
        if(key_name == "label"){
            key = new LabelKey(*this);
        }else if(key_name == "index"){
            key = new IndexKey(*this);    
        }else if(key_name == "row"){
            key = new RowKey(*this);    
        }else if(key_name == "column"){
            key = new ColumnKey(*this);    
        }else if(key_name == "uuid"){
            key = new ExportIDKey(*this);
        }else if(key_name == "replicate_count"){
            key = new ReplicateCountKey(*this);    
        }else if(key_name == "frequency"){
            key = new FrequencyKey(*this);    
        }else if(key_name == "inner_type"){
            key = new InnerTypeKey(*this);    
        }else if(key_name == "outer_type"){
            key = new OuterTypeKey(*this);    
        }else if(key_name == "type"){
            key = new TypeKey(*this);    
        }else if(key_name == "namespace"){
            key = new NamespaceKey(*this);
        }else{
        key = new Key(*this, key_name, type);
        }
        
        if(VisualKeyNames().contains(key_name)){
            key->setVisual(true);
        }
        key_lookup_[key_name] = key;
        
        //Need to Cache as Unregistered
        CacheEntityAsUnregistered(key);
        RegisterEntity(key);
        return key;
    }
}

QList<Key*> EntityFactory::GetKeys(){
    return key_lookup_.values();
}

void EntityFactory::DeregisterNode(Node* node){

}
void EntityFactory::DeregisterEdge(Edge* edge){
}

void EntityFactory::DestructEntity(GraphML* graphml){
    if(graphml){
        if(graphml->getGraphMLKind() == GRAPHML_KIND::NODE){
            auto node = (Node*) graphml;
            clearAcceptedEdgeKinds(node);

            auto children = node->getChildren(0);
            auto edges = node->getEdges(0);

            if(children.size()){
                qCritical() << "EntityFactory::DestructEntity:" << node->toString() << " Still has Children";
                for(auto child : children){
                    DestructEntity(child);
                }
            }

            if(edges.size()){
                qCritical() << "EntityFactory::DestructEntity:" << node->toString() << " Still has Edges";
                for(auto edge : edges){
                    DestructEntity(edge);
                }
            }
        }
        //This will deregister
        delete graphml;
    }
}


GraphML* EntityFactory::getGraphML(int id){
    if(hash_.contains(id)){
        return hash_.value(id);
    }else if(unregistered_hash_.contains(id)){
        return unregistered_hash_.value(id);
    }
    return 0;
}

Entity* EntityFactory::GetEntity(int id){
    auto item = getGraphML(id);
    if(item){
        switch(item->getGraphMLKind()){
            case GRAPHML_KIND::NODE:
            case GRAPHML_KIND::EDGE:
                return (Entity*) item;
            default:
                break;
        }
    }
    return 0;
}

Entity* EntityFactory::GetEntityByUUID(QString uuid){
    auto id = uuid_lookup_.value(uuid, -1);
    return GetEntity(id);
}

Node* EntityFactory::GetNode(int id){
    auto item = getGraphML(id);
    if(item && item->getGraphMLKind() == GRAPHML_KIND::NODE){
        return (Node*) item;
    }
    return 0;
}

Edge* EntityFactory::GetEdge(int id){
    auto item = getGraphML(id);
    if(item && item->getGraphMLKind() == GRAPHML_KIND::EDGE){
        return (Edge*) item;
    }
    return 0;
}

Data* EntityFactory::GetData(int id){
    auto item = getGraphML(id);
    if(item && item->getGraphMLKind() == GRAPHML_KIND::DATA){
        return (Data*) item;
    }
    return 0;
}

Key* EntityFactory::GetKey(int id){
    auto item = getGraphML(id);
    if(item && item->getGraphMLKind() == GRAPHML_KIND::KEY){
        return (Key*) item;
    }
    return 0;
}

Data* EntityFactory::CreateData(Key* key, QVariant value, bool is_protected){
    if(key){
        //Don't keep ID's for data
        auto data = new Data(*this, key, value, is_protected);
        return data;
    }
    return 0;
}
Data* EntityFactory::AttachData(Entity* entity, Key* key, QVariant value, bool is_protected){
    Data* data = 0;
    if(entity && key){
        data = entity->getData(key);
        if(!data){
            data = CreateData(key, value);
            if(data){
                entity->addData(data);
            }
        }
        data->setValue(value);
        data->setProtected(is_protected);
    }
    return data;
}

Data* EntityFactory::AttachData(Entity* entity, QString key_name, QVariant::Type type, QVariant value, bool is_protected){
    return AttachData(entity, GetKey(key_name, type), value, is_protected);
}

int EntityFactory::getFreeID(int preferred_id){

    int id = preferred_id;
    //If we haven't been given an id, or our hash contains our id already, we need to set a new one
    while(id == -1 || hash_.contains(id)){
        id = ++id_counter_;
    }
    return id;
}
int EntityFactory::getUnregisteredFreeID(){

    if(resuable_unregistered_ids_.size()){
        return resuable_unregistered_ids_.dequeue();
    }else{
        return --unregistered_id_counter_;
    }
}

bool EntityFactory::UnregisterTempID(GraphML* graphml){
    auto current_id = graphml->getID();

    if(unregistered_hash_.contains(current_id)){
        //Remove from the unregistered
        unregistered_hash_.remove(current_id);
        //Try and reuse the hash
        resuable_unregistered_ids_.enqueue(current_id);
        return true;
    }else{
        return false;
    }
}

int EntityFactory::RegisterEntity(GraphML* graphml, int id){
    auto graphml_factory = &(graphml->getFactory());
    //Get the current ID
    if(graphml && graphml_factory == this){
        auto success = UnregisterTempID(graphml);
        
        if(!success){
            //Check if the ID is already in the normal hash.
            id = graphml->getID();
        }else{
            id = getFreeID(id);
        }

        if(!hash_.contains(id)){
            //Dont allow a change from a permanent ID
            if(graphml->getID() > 0 ){
                qCritical() << "RIP" << id;
            }
            //Set the ID
            graphml->setID(id);
            //Insert
            hash_.insert(id, graphml);

            if(graphml->getGraphMLKind() == GRAPHML_KIND::NODE){
                auto node = (Node*) graphml;
                acceptedEdgeKindsChanged(node);
            }
        }else{
            auto hash_element = hash_.value(id, 0);
            if(graphml != hash_element){
                qCritical() << "EntityFactory: Registry hash collision @ ID: " << id;
                qCritical() << "\tExistent Element: " << hash_element->toString();
                qCritical() << "\tInserted Element: " << graphml->toString();
            }
        }
    }
    return graphml ? graphml->getID() : -1;
}

void EntityFactory::CacheEntityAsUnregistered(GraphML* graphml){
    if(graphml){
        //If we haven't been given an id, or our hash contains our id already, we need to set a new one
        auto id = getUnregisteredFreeID();
        
        //Get the id, post set
        graphml->setID(id);

        if(!unregistered_hash_.contains(id)){
            //qCritical() << "UCACHING: " << graphml->toString() << " AS " << id;
            unregistered_hash_.insert(id, graphml);
        }
    }
}

void EntityFactory::acceptedEdgeKindsChanged(Node* node){
    auto source_accepted_edge_kinds = node->getAcceptedEdgeKind(EDGE_DIRECTION::SOURCE);
    auto target_accepted_edge_kinds = node->getAcceptedEdgeKind(EDGE_DIRECTION::TARGET);

    
    for(auto edge_kind : getEdgeKinds()){
        auto& source_edge_map = accepted_source_edge_map[edge_kind];
        auto& target_edge_map = accepted_target_edge_map[edge_kind];

        if(source_accepted_edge_kinds.contains(edge_kind)){
            source_edge_map.insert(node);
        }else{
            source_edge_map.remove(node);
        }

        if(target_accepted_edge_kinds.contains(edge_kind)){
            target_edge_map.insert(node);
        }else{
            target_edge_map.remove(node);
        }
    }
}

void EntityFactory::clearAcceptedEdgeKinds(Node* node){
    for(auto edge_kind : getEdgeKinds()){
        auto& source_edge_map = accepted_source_edge_map[edge_kind];
        auto& target_edge_map = accepted_target_edge_map[edge_kind];
        source_edge_map.remove(node);
        target_edge_map.remove(node);
    }
}


void EntityFactory::DeregisterEntity(GraphML* graphml){
    if(graphml){
        auto id = graphml->getID();
        hash_.remove(id);
        unregistered_hash_.remove(id);
    }
}




void EntityFactory::EntityUUIDChanged(Entity* entity, QString uuid){
    if(entity){
        //Check for the old UUID
        auto old_uuid = entity->getDataValue("uuid").toString();
        uuid_lookup_.remove(old_uuid);

        if(!uuid.isEmpty()){
            uuid_lookup_.insert(uuid, entity->getID());
        }
    }
}

Node* EntityFactory::ConstructChildNode(Node& parent, NODE_KIND node_kind){
    auto child = CreateNode(node_kind);

    if(child){
        if(!parent.addChild(child)){
            DestructEntity(child);
            child = 0;
            throw std::invalid_argument(getNodeKindString(parent.getNodeKind()).toStdString() + " Cannot Adopt Child of Kind: " + getNodeKindString(node_kind).toStdString());
        }
    }else{
        throw std::invalid_argument(getNodeKindString(parent.getNodeKind()).toStdString() + " Cannot Construct Child of Kind: " + getNodeKindString(node_kind).toStdString());
    }
    return child;
}