#ifndef DATANODE_H
#define DATANODE_H
#include "../node.h"

class EntityFactory;
class DataNode : public Node
{
protected:
    DataNode(EntityFactory* factory, NODE_KIND kind, QString kind_str);
    DataNode(NODE_KIND kind);
public:
    bool hasInputData();
    bool hasOutputData();

    DataNode* getInputData();
    DataNode* getOutputData();

    void setPromiscuousDataLinker(bool set);
    void setMultipleDataReceiver(bool receiver);
    void setDataProducer(bool producer);
    void setDataReciever(bool reciever);
    bool isDataProducer() const;
    bool isDataReciever() const;

    bool isPromiscuousDataLinker() const;
    bool isMultipleDataReceiver() const;

    bool comparableTypes(DataNode* node);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst) = 0;

    bool isContainedInVector();
    bool isContainedInVariable();
    Node* getContainmentNode();
private:
    void RunContainmentChecks();
    bool _run_containment_checks = false;

    bool _contained_in_vector = false;
    bool _contained_in_variable = false;
    Node* _containment_node = 0;

    bool promiscuous_data_linker_ = false;


    bool _isProducer = false;
    bool _isReciever = false;;
    bool _isMultipleDataReceiver = false;;

};

#endif // DATANODE_H
