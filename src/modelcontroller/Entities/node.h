#ifndef NODE_H
#define NODE_H

#include "entity.h"
#include "../nodekinds.h"
#include "../edgekinds.h"
#include <functional>

#include <QVariant>
#include <QSet>

class Edge;
class EntityFactory;

class Node : public Entity
{
    Q_OBJECT
    friend class Edge;
    friend class EntityFactory;

    public:
        enum class EdgeRule{
            ALLOW_EXTERNAL_DEFINITIONS,
            IGNORE_REQUIRED_INSTANCE_DEFINITIONS,
            ALWAYS_CHECK_VALID_DEFINITIONS,
            ALLOW_MULTIPLE_IMPLEMENTATIONS
        };

    protected:
        //Factory Static Functions
        static void RegisterNodeKind(EntityFactory* factory, NODE_KIND kind, QString kind_string, std::function<Node* ()> constructor);
        static void RegisterWithEntityFactory(EntityFactory& factory, const NODE_KIND& kind, const QString& kind_string, std::function<Node* (EntityFactory&, bool)> constructor);
        
        static void RegisterComplexNodeKind(EntityFactory* factory, NODE_KIND kind, std::function<Node* (EntityFactory*)> constructor);
        static void RegisterComplexNodeKind(EntityFactory* factory, NODE_KIND kind, std::function<Node* (EntityFactory*, bool)> constructor);
        static void RegisterDefaultData(EntityFactory* factory, NODE_KIND kind, QString key_name, QVariant::Type type, bool is_protected = false, QVariant value = QVariant());
        static void RegisterValidDataValues(EntityFactory* factory, NODE_KIND kind, QString key_name, QVariant::Type type, QList<QVariant> values);
        
        //Static Helper Functions
        static void BindDefinitionToInstance(Node* definition, Node* instance, bool setup);
        static void LinkData(Node* source, const QString &source_key, Node* destination, const QString &destination_key, bool setup);
        
        //Constuctor
        Node(EntityFactory& factory, NODE_KIND node_kind, bool is_temp_node);
        ~Node();


        //Valid Definition/Instance/Impl Registrations
        void addInstanceKind(NODE_KIND kind);
        void addImplKind(NODE_KIND kind);
        void addImplsDefinitionKind(NODE_KIND kind);
        void addInstancesDefinitionKind(NODE_KIND kind);
        void setChainableDefinition();

        //Rule Getters/Setters
        void SetEdgeRuleActive(EdgeRule rule, bool active = true);
        bool IsEdgeRuleActive(EdgeRule rule);

        virtual QSet<Node*> getListOfValidAncestorsForChildrenDefinitions();
        virtual QSet<Node*> getParentNodesForValidDefinition();
        
        QList<int> getTreeIndex();
        
        
        bool setAsRoot(int root_index);
        
        void setParentNode(Node* parent, int branch = -1);
        void updateOrphanedDescendants();

        void addEdge(Edge *edge);
        void removeEdge(Edge *edge);

        //Notifier Functions
        virtual void childAdded(Node* child){};
        virtual void childRemoved(Node* child);
        virtual void parentSet(Node* parent){};
        virtual void definitionSet(Node* definition){};

        void setDefinition(Node* definition);
        void unsetDefinition();

        bool ancestorOf(Node *node);
        bool ancestorOf(Edge* edge);
        bool containsEdge(Edge* edge);

        void setAcceptsNodeKind(NODE_KIND node_kind, bool accept = true);
        bool canAcceptNodeKind(NODE_KIND node_kind) const;
        bool canAcceptNodeKind(const Node* node) const;

        void setNodeType(NODE_TYPE type);
        void removeNodeType(NODE_TYPE type);
        void setAcceptsEdgeKind(EDGE_KIND edge_kind, EDGE_DIRECTION direction, bool accept = true);
        bool canAcceptEdgeKind(EDGE_KIND edge_kind, EDGE_DIRECTION direction) const;
        bool canCurrentlyAcceptEdgeKind(EDGE_KIND edgeKind, Node* dst) const;
        void updateTreeIndex(QList<int> parent_tree_index);
        virtual void updateViewAspect(VIEW_ASPECT parent_view_aspect);
    signals:
        void acceptedEdgeKindsChanged(Node* node);
    public:
        //Valid Definition/Instance/Impl Getters
        bool isValidInstanceKind(NODE_KIND kind) const;
        bool isValidImplKind(NODE_KIND kind) const;
        bool isValidDefinitionKind(NODE_KIND kind) const;
        QSet<NODE_KIND> getInstanceKinds() const;
        QSet<NODE_KIND> getDefinitionKinds() const;
        QSet<NODE_KIND> getImplKinds() const;

        //Definition Getters
        QList<Node*> getRequiredInstanceDefinitions();
       
        bool addChild(Node *child);
        bool removeChild(Node *child);

        
        virtual VIEW_ASPECT getViewAspect() const;
        int getDepth() const;

        //Ancestor Functions
        int getDepthFromCommonAncestor(Node* dst);
        Node* getCommonAncestor(Node* dst);

        Node* getDefinition(bool recurse=false) const;
        
        QSet<Node*> getInstances() const;
        QSet<Node*> getImplementations() const;
        virtual QSet<Node*> getDependants() const;
        QSet<Node*> getNestedDependants();
        

        QString toGraphML(int indentDepth = 0, bool functional_export = false);
   
    
        //Node kind getters
        NODE_KIND getNodeKind() const;
        NODE_KIND getParentNodeKind() const;

        //Parent Node Getters
        QList<Node*> getParentNodes(int depth = 1);
        Node* getParentNode(int depth = 1);
        int getParentNodeID();

        //Returns whether or not this Node can Adopt the child Node.
        virtual bool canAdoptChild(Node* node);
        virtual bool canAcceptEdge(EDGE_KIND edge_kind, Node* destination);

        //Children Getters
        bool containsChild(Node* child);
        QList<Node *> getChildren(int depth =-1);
        QList<Node *> getChildrenOfKind(NODE_KIND kind, int depth =-1);
        QList<Node *> getChildrenOfKinds(QSet<NODE_KIND> kinds, int depth =-1);
        QList<Node *> getSiblings();

        int getChildrenCount();
        int getChildrenOfKindCount(NODE_KIND kind);

        //Ancestor Getters
        bool isAncestorOf(GraphML* item);
        bool isDescendantOf(Node *node);

        //Edge Getters
        Edge* getEdgeTo(Node* node, EDGE_KIND edge_kind);
        bool gotEdgeTo(Node* node, EDGE_KIND edge_kind);


        QList<Edge*> getEdges(int depth=-1);
        
        int getEdgeCount();
        int getEdgeOfKindCount(EDGE_KIND kind);
        QList<Edge *> getEdgesOfKind(EDGE_KIND edge_kind, int depth = -1);
        QList<Key *> getKeys(int depth=-1);

        bool isDefinition() const;
        bool isInstance() const;
        bool isInstanceImpl() const;
        bool isAspect() const;
        bool isImpl() const;

        QSet<NODE_TYPE> getTypes() const;
    
        void addInstance(Node* instance);
        void removeInstance(Node* instance);
        void addImplementation(Node* implementation);
        void removeImplementation(Node* implementation);

        QSet<NODE_KIND> getAcceptedNodeKinds() const;
        virtual QSet<NODE_KIND> getUserConstructableNodeKinds() const;
        bool isNodeOfType(NODE_TYPE type) const;
        QSet<EDGE_KIND> getCurrentAcceptedEdgeKind(EDGE_DIRECTION direction) const;
        QSet<EDGE_KIND> getAcceptedEdgeKind(EDGE_DIRECTION direction) const;

        bool canCurrentlyAcceptEdgeKind(EDGE_KIND edge_kind, EDGE_DIRECTION direction) const;

        
    private:
        
        bool indirectlyConnectedTo(Node* node);
        QString _toGraphML(int indentDepth, bool ignoreVisuals=false);
        QList<Node*> getOrderedChildNodes();
        void setTop(int index = 0);

        //Variables
        QList<int> tree_index_;
        int branch_ = 0;
        int branch_count_ = 0;

        Node* parent_node_ = 0;
        Node* definition_ = 0;

        NODE_KIND node_kind_ = NODE_KIND::NONE;
        NODE_KIND parent_node_kind_ = NODE_KIND::NONE;
        VIEW_ASPECT view_aspect_ = VIEW_ASPECT::NONE;

        QSet<Node*> instances_;
        QSet<Node*> implementations_;

        QMultiMap<EDGE_KIND, Edge*> edges_;
        QMultiMap<NODE_KIND, Node*> children_;

        QSet<NODE_TYPE> node_types_;
        QSet<NODE_KIND> accepted_node_kinds_;
    
        QSet<EDGE_KIND> accepted_edge_kinds_as_source_;
        QSet<EDGE_KIND> accepted_edge_kinds_as_target_;

        QSet<NODE_KIND> definition_kinds_;
        QSet<NODE_KIND> instance_kinds_;
        QSet<NODE_KIND> impl_kinds_;

        QSet<EdgeRule> active_edge_rules_;

        
};

inline uint qHash(Node::EdgeRule key, uint seed){
    return ::qHash(static_cast<uint>(key), seed);
};

#endif // NODE_H
