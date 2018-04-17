#include "externaltype.h"

#include "../edge.h"
#include "../data.h"
#include "../Keys/typekey.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::EXTERNAL_TYPE;
const QString kind_string = "ExternalType";

MEDEA::ExternalType::ExternalType(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ExternalType();});

    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true);
};

MEDEA::ExternalType::ExternalType() : Node(node_kind)
{
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
}



bool MEDEA::ExternalType::canAdoptChild(Node* child)
{
    return false;
}


bool MEDEA::ExternalType::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
        case EDGE_KIND::DEFINITION:{
            if(dst->getNodeKind() != NODE_KIND::EXTERNAL_TYPE){
                return false;
            }

            auto container = getTopLevelContainer();
            auto dst_parent = dst->getParentNode();

            if(dst_parent->getNodeKind() != NODE_KIND::WORKER_DEFINITION){
                return false;
            }


            bool got_instance = false;
            if(container){
                for(auto child : container->getChildrenOfKind(NODE_KIND::WORKER_INSTANCE)){
                    if(child->getDefinition(true) == dst_parent){
                        got_instance = true;
                        break;
                    }
                }
            }

            if(!got_instance){
                return false;
            }
            return true;

            break;
        }
        default:
            break;
    }

    return Node::canAcceptEdge(edgeKind, dst);
}

Node* MEDEA::ExternalType::getTopLevelContainer(){
    if(!top_level_calculated){
        Node* container_node = 0;

        for(auto parent_node : getParentNodes(-1)){
            if(parent_node->isNodeOfType(NODE_TYPE::BEHAVIOUR_CONTAINER)){
                container_node = parent_node;
            }
        }

        
        top_level_container = container_node;
        top_level_calculated = true;
    }
    return top_level_container;
}