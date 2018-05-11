
#include "entityfactory.h"
#include "entityfactorybroker.h"
#include "entityfactoryregistrybroker.h"

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
#include "Entities/DeploymentDefinitions/periodiceventinstance.h"

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

void EntityFactory::RegisterNodeKind(const NODE_KIND node_kind, const QString& kind_string, std::function<Node* (EntityFactoryBroker&, bool)> constructor){
    if(doesNodeStructExist(node_kind)){
        throw std::invalid_argument("EntityFactory: Trying to register duplicate Node with kind '" + kind_string.toStdString() + "'");
    }

    auto node = getNodeStruct(node_kind);
    node->kind_str = kind_string;
    node->constructor = constructor;

    if(!node_kind_lookup.contains(kind_string)){
        qCritical() << "EntityFactory: Registered Node Kind [" << (uint)node_kind << "]: " << kind_string;
        //Insert into our reverse lookup
        node_kind_lookup.insert(kind_string, node_kind);
    }else{
        throw std::invalid_argument("EntityFactory: Trying to register duplicate Node with kind string '" + kind_string.toStdString() + "'");
    }
}
void EntityFactory::RegisterEdgeKind(const EDGE_KIND edge_kind, const QString& kind_string, std::function<Edge* (EntityFactoryBroker&,Node*, Node*)> constructor){
    if(doesEdgeStructExist(edge_kind)){
        throw std::invalid_argument("EntityFactory: Trying to register duplicate Edge with kind '" + kind_string.toStdString() + "'");
    }

    auto edge = getEdgeStruct(edge_kind);
    edge->kind_str = kind_string;
    edge->constructor = constructor;

    if(!edge_kind_lookup.contains(kind_string)){
        qCritical() << "EntityFactory: Registered Edge Kind [" << (uint)edge_kind << "]: " << kind_string;
        //Insert into our reverse lookup
        edge_kind_lookup.insert(kind_string, edge_kind);
    }else{
        throw std::invalid_argument("EntityFactory: Trying to register duplicate Edge with kind string '" + kind_string.toStdString() + "'");
    }
}



QSet<Node*> EntityFactory::GetNodesWhichAcceptEdgeKinds(EDGE_KIND edge_kind, EDGE_DIRECTION direction){
    auto& map = direction == EDGE_DIRECTION::SOURCE ? accepted_source_edge_map : accepted_target_edge_map;
    return map.value(edge_kind);
}


EntityFactory::EntityFactory() : factory_broker_(*this){
    auto& ref = *this;
    auto registry_broker = EntityFactoryRegistryBroker(ref);
    
    Model::RegisterWithEntityFactory(registry_broker);

    BehaviourDefinitions::RegisterWithEntityFactory(registry_broker);
    AssemblyDefinitions::RegisterWithEntityFactory(registry_broker);
    DeploymentDefinitions::RegisterWithEntityFactory(registry_broker);
    HardwareDefinitions::RegisterWithEntityFactory(registry_broker);
    WorkerDefinitions::RegisterWithEntityFactory(registry_broker);
    InterfaceDefinitions::RegisterWithEntityFactory(registry_broker);

    //Impl Elements
    ComponentImpl::RegisterWithEntityFactory(registry_broker);
    AttributeImpl::RegisterWithEntityFactory(registry_broker);
    InEventPortImpl::RegisterWithEntityFactory(registry_broker);
    OutEventPortImpl::RegisterWithEntityFactory(registry_broker);

    //Behaviour Elements
    MEDEA::IfStatement::RegisterWithEntityFactory(registry_broker);
    MEDEA::IfCondition::RegisterWithEntityFactory(registry_broker);
    MEDEA::ElseIfCondition::RegisterWithEntityFactory(registry_broker);
    MEDEA::ElseCondition::RegisterWithEntityFactory(registry_broker);

    MEDEA::WhileLoop::RegisterWithEntityFactory(registry_broker);
    MEDEA::ForLoop::RegisterWithEntityFactory(registry_broker);
    MEDEA::ExternalType::RegisterWithEntityFactory(registry_broker);

    Code::RegisterWithEntityFactory(registry_broker);
    Header::RegisterWithEntityFactory(registry_broker);
    InputParameter::RegisterWithEntityFactory(registry_broker);
    PeriodicEvent::RegisterWithEntityFactory(registry_broker);
    PeriodicEventInstance::RegisterWithEntityFactory(registry_broker);
    ReturnParameter::RegisterWithEntityFactory(registry_broker);
    Setter::RegisterWithEntityFactory(registry_broker);
    Variable::RegisterWithEntityFactory(registry_broker);
    VariableParameter::RegisterWithEntityFactory(registry_broker);
    VariadicParameter::RegisterWithEntityFactory(registry_broker);
    
    
    FunctionCall::RegisterWithEntityFactory(registry_broker);

    //Instance Elements
    ComponentInstance::RegisterWithEntityFactory(registry_broker);
    AttributeInstance::RegisterWithEntityFactory(registry_broker);
    InEventPortInstance::RegisterWithEntityFactory(registry_broker);
    OutEventPortInstance::RegisterWithEntityFactory(registry_broker);
    AggregateInstance::RegisterWithEntityFactory(registry_broker);
    MemberInstance::RegisterWithEntityFactory(registry_broker);
    VectorInstance::RegisterWithEntityFactory(registry_broker);
    
    //Deployment Elements
    ComponentAssembly::RegisterWithEntityFactory(registry_broker);
    HardwareNode::RegisterWithEntityFactory(registry_broker);
    HardwareCluster::RegisterWithEntityFactory(registry_broker);
    InEventPortDelegate::RegisterWithEntityFactory(registry_broker);
    OutEventPortDelegate::RegisterWithEntityFactory(registry_broker);
    LoggingProfile::RegisterWithEntityFactory(registry_broker);
    LoggingServer::RegisterWithEntityFactory(registry_broker);
    
    OpenCLDevice::RegisterWithEntityFactory(registry_broker);
    OpenCLPlatform::RegisterWithEntityFactory(registry_broker);

    //Definition Elements
    Aggregate::RegisterWithEntityFactory(registry_broker);
    Attribute::RegisterWithEntityFactory(registry_broker);
    Component::RegisterWithEntityFactory(registry_broker);
    InEventPort::RegisterWithEntityFactory(registry_broker);
    Member::RegisterWithEntityFactory(registry_broker);
    OutEventPort::RegisterWithEntityFactory(registry_broker);
    Vector::RegisterWithEntityFactory(registry_broker);


    Enum::RegisterWithEntityFactory(registry_broker);
    EnumMember::RegisterWithEntityFactory(registry_broker);
    EnumInstance::RegisterWithEntityFactory(registry_broker);
    MEDEA::Class::RegisterWithEntityFactory(registry_broker);
    MEDEA::ClassInstance::RegisterWithEntityFactory(registry_broker);
    MEDEA::Function::RegisterWithEntityFactory(registry_broker);
    MEDEA::DeploymentAttribute::RegisterWithEntityFactory(registry_broker);

    //QOS Profiles
    
    DDS_QOSProfile::RegisterWithEntityFactory(registry_broker);
    DDS_DeadlineQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_DestinationOrderQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_DurabilityQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_DurabilityServiceQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_EntityFactoryQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_GroupDataQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_HistoryQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_LatencyBudgetQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_LifespanQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_LivelinessQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_OwnershipQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_OwnershipStrengthQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_PartitionQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_PresentationQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_ReaderDataLifecycleQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_ReliabilityQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_ResourceLimitsQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_TimeBasedFilterQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_TopicDataQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_TransportPriorityQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_UserDataQosPolicy::RegisterWithEntityFactory(registry_broker);
    DDS_WriterDataLifecycleQosPolicy::RegisterWithEntityFactory(registry_broker);

    //Elements
    SharedDatatypes::RegisterWithEntityFactory(registry_broker);
    Namespace::RegisterWithEntityFactory(registry_broker);

    MEDEA::ServerInterface::RegisterWithEntityFactory(registry_broker);
    MEDEA::ServerPort::RegisterWithEntityFactory(registry_broker);
    MEDEA::ClientPort::RegisterWithEntityFactory(registry_broker);

    MEDEA::ServerPortInstance::RegisterWithEntityFactory(registry_broker);
    MEDEA::ClientPortInstance::RegisterWithEntityFactory(registry_broker);

    MEDEA::ServerPortImpl::RegisterWithEntityFactory(registry_broker);
    MEDEA::ServerRequest::RegisterWithEntityFactory(registry_broker);
    
    MEDEA::BooleanExpression::RegisterWithEntityFactory(registry_broker);


    MEDEA::InputParameterGroup::RegisterWithEntityFactory(registry_broker);
    MEDEA::ReturnParameterGroup::RegisterWithEntityFactory(registry_broker);

    MEDEA::InputParameterGroupInstance::RegisterWithEntityFactory(registry_broker);
    MEDEA::ReturnParameterGroupInstance::RegisterWithEntityFactory(registry_broker);

    VoidType::RegisterWithEntityFactory(registry_broker);

    //Edges
    DefinitionEdge::RegisterWithEntityFactory(registry_broker);
    AggregateEdge::RegisterWithEntityFactory(registry_broker);
    AssemblyEdge::RegisterWithEntityFactory(registry_broker);
    DataEdge::RegisterWithEntityFactory(registry_broker);
    DeploymentEdge::RegisterWithEntityFactory(registry_broker);
    QosEdge::RegisterWithEntityFactory(registry_broker);
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
bool EntityFactory::doesEdgeStructExist(EDGE_KIND kind){
    return edge_struct_lookup.contains(kind);
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
            edge = edge_struct->constructor(factory_broker_, source, destination);
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
        if(node_struct->constructor){
            try{
                node = node_struct->constructor(factory_broker_, is_temporary);
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
            key = new LabelKey(factory_broker_);
        }else if(key_name == "index"){
            key = new IndexKey(factory_broker_);    
        }else if(key_name == "row"){
            key = new RowKey(factory_broker_);    
        }else if(key_name == "column"){
            key = new ColumnKey(factory_broker_);    
        }else if(key_name == "uuid"){
            key = new ExportIDKey(factory_broker_, std::bind(&EntityFactory::EntitiesUUIDChanged, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
        }else if(key_name == "replicate_count"){
            key = new ReplicateCountKey(factory_broker_);    
        }else if(key_name == "frequency"){
            key = new FrequencyKey(factory_broker_);    
        }else if(key_name == "inner_type"){
            key = new InnerTypeKey(factory_broker_);    
        }else if(key_name == "outer_type"){
            key = new OuterTypeKey(factory_broker_);    
        }else if(key_name == "type"){
            key = new TypeKey(factory_broker_);    
        }else if(key_name == "namespace"){
            key = new NamespaceKey(factory_broker_);
        }else{
        key = new Key(factory_broker_, key_name, type);
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
    if(!node){
        return;
    }
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

void EntityFactory::DeregisterEdge(Edge* edge){

}

void EntityFactory::DeregisterGraphML(GraphML* graphml){
    if(graphml){
        auto id = graphml->getID();
        hash_.remove(id);
        unregistered_hash_.remove(id);
    }
}

void EntityFactory::DestructEntity(GraphML* graphml){
    if(graphml){
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
        auto data = new Data(factory_broker_, key, value, is_protected);
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

int EntityFactory::getFreeID(int pregistry_brokererred_id){

    int id = pregistry_brokererred_id;
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
    auto graphml_factory = &(graphml->getFactoryBroker().GetEntityFactory());
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
                AcceptedEdgeKindsChanged(node);
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

void EntityFactory::AcceptedEdgeKindsChanged(Node* node){
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


void EntityFactory::EntitiesUUIDChanged(Entity* entity, QString old_uuid, QString new_uuid){
    if(entity){
        uuid_lookup_.remove(old_uuid);

        if(!new_uuid.isEmpty()){
            uuid_lookup_.insert(new_uuid, entity->getID());
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