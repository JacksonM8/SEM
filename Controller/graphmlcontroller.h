#ifndef GRAPHMLCONTROLLER_H
#define GRAPHMLCONTROLLER_H

#include <QObject>
#include <QHash>
#include <QStandardItemModel>
#include <QItemSelection>

#include <QDebug>
#include "../Model/model.h"
#include "../GUI/nodeview.h"
#include <QClipboard>
#include <QInputDialog>
#include <QHash>
#include <QStack>
enum ACTION_TYPE {CONSTRUCT, DESTRUCT, ADOPT, DISOWN};

struct GUIContainer{
    NodeItem* nodeItem;
    QStandardItem* modelItem;
    QString deleteXML;
    Node* node;
};

struct Action{
    GraphML::KIND itemKind;
    ACTION_TYPE actionType;
    GraphML *item;
    GraphMLContainer* src;
    GraphMLContainer* dst;
    GraphMLContainer *parent;
    QString removedXML;
};

class GraphMLController: public QObject
{
    Q_OBJECT
public:

    GraphMLController(NodeView *view);
    ~GraphMLController();


    Model* getModel();

    QStandardItemModel* getTreeModel();
signals:
    //View Signals
    void view_addNodeItem(NodeItem* nodeItem);
    void view_addNodeEdge(NodeEdge* nodeEdge);
    void view_centerNodeItem(NodeItem* nodeItem);

    void view_LabelChanged(QString value);
    void view_EnableGUI(bool enabled);

    void view_CopyText(QString value);

    //Model Signals
    void model_ImportGraphML(QStringList inputGraphML, GraphMLContainer *currentParent=0);
    void model_ExportGraphML(QString filePath);

    void model_ConstructNode(QString kind, GraphMLContainer* parentNode);
    void model_ConstructEdge(Edge* edge);


public slots:
    //MODEL SLOTS
    //Functions triggered by the Model
    void model_MadeEdge(Edge* edge);

    void model_MadeNode(GraphMLContainer* item);
    void model_RemoveNode(GraphMLContainer* item);

    void model_RemoveEdge(Edge* edge);

    void model_WriteToFile(QString filePath, QString data);
    void model_EnableGUI(bool enabled);

    //VIEW SLOTS
    //Functions triggered by the View.
    void view_ControlPressed(bool isDown);
    void view_ShiftTriggered(bool isDown);
    void view_DeleteTriggered(bool isDown);
    void view_SelectAll();
    void view_EmptyScenePressed();

    //Functions triggered by GUI Operation Slots.
    void view_ImportGraphML(QStringList inputGraphML);
    void view_ExportGraphML(QString filePath);
    void view_UpdateLabel(QString value);

    //Copy/Paste Operations
    void view_Undo();
    void view_Redo();
    void view_Copy();
    void view_Cut();
    void view_Paste(QString XMLData);

    //NODEITEM SLOTS
    //Functions triggered by the NodeItems in the View
    void nodeItem_Selected(NodeItem* nodeItem);
    void nodeItem_SetCentered(NodeItem* nodeItem);
    void nodeItem_MakeChildNode(QString type, Node* node);

    //NODEEDGE SLOTS
    //Functions triggered by the NodeEdges in the View
    void nodeEdge_Selected(NodeEdge* nodeEdge);


    //TREEVIEW SLOTs
    //Functions Triggered by the TreeModel in the View
    void view_SetCentered(QModelIndex index);
    void view_SetSelected(QModelIndex index);

private:
    bool KEY_CONTROL_DOWN;
    bool KEY_SHIFT_DOWN;

    void deselectNodeItems(NodeItem* selectedNodeItem = 0);
    void deselectEdgeItems(NodeEdge* selectedEdgeItem = 0);
    void selectNodeItem(NodeItem* selectedNodeItem);
    void selectEdgeItem(NodeEdge* selectedEdgeItem);

    void hideAllMatches(Node* node);
    void resetMatches();
    void deleteSelectedNodeItems();
    void deleteSelectedEdgeItems();

    NodeItem* getNodeItemFromNode(Node* node);
    NodeEdge* getNodeEdgeFromEdge(Edge* edge);
    GUIContainer* getGUIContainer(Node* node);
    GUIContainer* getGUIContainer(NodeItem* nodeItem);
    GUIContainer* getGUIContainer(QModelIndex nodeIndex);


    bool reverseAction(Action action);
    void removeGraphML(GraphML* node);
    void removeNodeItem(NodeItem* nodeItem);

    QVector<NodeItem*> selectedNodeItems;

    QVector<NodeEdge*> selectedEdgeItems;

    QStack<Action> undoStack;
    QStack<Action> redoStack;

    QVector<GUIContainer*> nodeContainers;
    QVector<NodeItem*> nodeItems;
    QVector<NodeEdge*> edgeItems;

    QHash<QString, GraphMLContainer*> nodeDeletionIDLookup;

    NodeItem* currentMaximized;

    Node* currentParent;

    QStandardItemModel* treeModel;

    QString copyBuffer;

    Model* model;
    NodeView* view;
    bool UNDOING;
    bool REDOING;
};

#endif // GRAPHMLCONTROLLER_H
