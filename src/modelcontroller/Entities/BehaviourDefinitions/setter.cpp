#include "setter.h"
#include "../Keys/typekey.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../InterfaceDefinitions/datanode.h"

const NODE_KIND node_kind = NODE_KIND::SETTER;
const QString kind_string = "Setter";

void Setter::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new Setter(broker, is_temp_node);
        });
}

Setter::Setter(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setLabelFunctional(false);
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //setup Data
    auto label = broker.AttachData(this, "label", QVariant::String, "???", true);
    broker.ProtectData(this, "index", false);

    //Attach Children
    lhs_ = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);
    operator_ = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);
    rhs_ = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);

    operator_->setDataReceiver(false);
    operator_->setDataProducer(false);


    //Setup LHS
    broker.AttachData(lhs_, "label", QVariant::String, "lhs", true);
    broker.AttachData(lhs_, "icon", QVariant::String, "Variable", true);
    broker.AttachData(lhs_, "icon_prefix", QVariant::String, "EntityIcons", true);
    broker.AttachData(lhs_, "is_generic_param", QVariant::Bool, true, true);

    //Setup Comparator
    auto data_operator = broker.AttachData(operator_, "label", QVariant::String, "=", false);
    broker.AttachData(operator_, "icon", QVariant::String, "circlePlusDark", true);
    broker.AttachData(operator_, "icon_prefix", QVariant::String, "Icons", true);
    broker.RemoveData(operator_, "value");
    broker.RemoveData(operator_, "type");
    broker.RemoveData(operator_, "inner_type");
    broker.RemoveData(operator_, "outer_type");

    data_operator->addValidValues({"=", "+=", "-=", "*=", "/="});

    //Setup RHS
    broker.AttachData(rhs_, "label", QVariant::String, "rhs", true);
    broker.AttachData(rhs_, "icon", QVariant::String, "Variable", true);
    broker.AttachData(rhs_, "icon_prefix", QVariant::String, "EntityIcons", true);
    broker.AttachData(rhs_, "is_generic_param", QVariant::Bool, true, true);

    //Bind Value changing
    auto data_rhs_value = rhs_->getData("value");
    auto data_lhs_value = lhs_->getData("value");

    //Update Label on data Change
    connect(data_rhs_value, &Data::dataChanged, this, &Setter::updateLabel);
    connect(data_lhs_value, &Data::dataChanged, this, &Setter::updateLabel);
    connect(data_operator, &Data::dataChanged, this, &Setter::updateLabel);

    updateLabel();
    TypeKey::BindInnerAndOuterTypes(lhs_, rhs_, true);
}

bool Setter::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
    case NODE_KIND::INPUT_PARAMETER:
        if(getChildrenOfKindCount(child_kind) >= 3){
                return false;
            }
        break;
    default:
        break;
    }

    return Node::canAdoptChild(child);
}

void Setter::updateLabel(){
    QString new_label = "???";
    if(lhs_ && operator_ && rhs_){
        auto lhs_value = lhs_->getDataValue("value").toString();
        auto operator_value = operator_->getDataValue("label").toString();
        auto rhs_value = rhs_->getDataValue("value").toString();

        if(lhs_value.length() && rhs_value.length()){
            new_label = lhs_value + " " + operator_value + " " + rhs_value;
        }
    }
    setDataValue("label", new_label);
}

DataNode* Setter::GetLhs(){
    return lhs_;
}

DataNode* Setter::GetRhs(){
    return rhs_;
}

DataNode* Setter::GetOperator(){
    return operator_;
}