#include "aggregateinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../edgekinds.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::AGGREGATE_INSTANCE;
const QString kind_string = "Aggregate Instance";


void AggregateInstance::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new AggregateInstance(broker, is_temp_node);
    });
}

AggregateInstance::AggregateInstance(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::AGGREGATE);
    setChainableDefinition();

    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::MEMBER_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::VECTOR_INSTANCE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.ProtectData(this, "index", true);
    broker.AttachData(this, "type", QVariant::String, "", true);
}

bool AggregateInstance::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::DATA:{        //Can only connect to an AggregateInstance.
        if(dst->getNodeKind() == NODE_KIND::AGGREGATE_INSTANCE){
            //Can only connect to an AggregateInstance with the same definition.
            if(!getDefinition(true) || getDefinition(true) != dst->getDefinition(true)){
                return false;
            }
        }
        break;
    }
    
    default:
        break;

    }
    return DataNode::canAcceptEdge(edge_kind, dst);
}


void AggregateInstance::ParentSet(DataNode* child){
    if(child->getViewAspect() == VIEW_ASPECT::BEHAVIOUR){
        bool got_valid_producer = false;
        bool got_valid_receiver = false;

        QSet<NODE_KIND> valid_producer_parents;
        QSet<NODE_KIND> valid_receiver_parents = {NODE_KIND::OUTEVENTPORT_IMPL};
        
        QSet<NODE_KIND> inverse_parents = {NODE_KIND::FUNCTION_CALL, NODE_KIND::SERVER_REQUEST};
        QSet<NODE_KIND> invalid_parents = {NODE_KIND::VECTOR, NODE_KIND::VECTOR_INSTANCE, NODE_KIND::VARIABLE};
        

        QSet<NODE_KIND> ancestor_kinds;

        //Make a list of ancestor kinds
        for(auto ancestor : child->getParentNodes(-1)){
            ancestor_kinds += ancestor->getNodeKind();
        }


        if(ancestor_kinds.intersects(inverse_parents)){
            valid_producer_parents += {NODE_KIND::RETURN_PARAMETER_GROUP, NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE};
            valid_receiver_parents += {NODE_KIND::INPUT_PARAMETER_GROUP, NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE};
        }else{
            valid_producer_parents += {NODE_KIND::INPUT_PARAMETER_GROUP, NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE};
            valid_receiver_parents += {NODE_KIND::RETURN_PARAMETER_GROUP, NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE};
        }

        if(!ancestor_kinds.intersects(valid_receiver_parents)){
            valid_producer_parents += {NODE_KIND::INEVENTPORT_IMPL};
        }

        if(!ancestor_kinds.intersects(invalid_parents)){
            if(ancestor_kinds.intersects(valid_producer_parents)){
                got_valid_producer = true;
            }
            if(ancestor_kinds.intersects(valid_receiver_parents)){
                got_valid_receiver = true;
            }
        }

        child->setDataProducer(got_valid_producer);
        child->setDataReceiver(got_valid_receiver);
    }
}
void AggregateInstance::parentSet(Node* parent){
    switch(parent->getNodeKind()){
        case NODE_KIND::INEVENTPORT_IMPL:{
            getFactoryBroker().AttachData(this, "row", QVariant::Int, 0, true);
            getFactoryBroker().AttachData(this, "column", QVariant::Int, -1, true);
            break;
        }
        case NODE_KIND::AGGREGATE:{
            getFactoryBroker().ProtectData(this, "index", true);
            SetEdgeRuleActive(EdgeRule::DISALLOW_DEFINITION_CHAINING);
            break;
        }
        case NODE_KIND::VECTOR:
        case NODE_KIND::VARIABLE:
        case NODE_KIND::INPUT_PARAMETER_GROUP:
        case NODE_KIND::RETURN_PARAMETER_GROUP:{
            SetEdgeRuleActive(EdgeRule::DISALLOW_DEFINITION_CHAINING);
            break;
        }
        default:
            break;
    }

    AggregateInstance::ParentSet(this);
    DataNode::parentSet(parent);
}