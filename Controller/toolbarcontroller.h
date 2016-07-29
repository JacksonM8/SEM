#ifndef TOOLBARCONTROLLER_H
#define TOOLBARCONTROLLER_H

#include <QToolBar>
#include <QObject>
#include "../View/viewitem.h"
#include "nodeviewitemaction.h"
#include "../Widgets/New/actiongroup.h"
#include "rootaction.h"

class ViewController;
class ToolActionController : public QObject
{
    Q_OBJECT
public:
    ToolActionController(ViewController* viewController);

    QList<QAction*> getNodeActionsOfKind(QString kind, bool stealth);
    QAction* getNodeActionOfKind(QString kind, bool stealth);

    QList<QAction*> getEdgeActionsOfKind(Edge::EDGE_CLASS kind, bool stealth);
    QAction* getEdgeActionOfKind(Edge::EDGE_CLASS kind, bool stealth);

    QList<QAction*> getAdoptableKindsActions(bool stealth);
    QAction* getAdoptableKindsAction(bool stealth);

    QList<QAction*> getConnectedNodesActions(bool stealth);
    QAction* getConnectedNodesAction(bool stealth);

    QList<QAction*> getInstancesActions(bool stealth);
    QAction* getInstancesAction(bool stealth);

    //QAction* getDefinitionAction(bool stealth);
    //QAction* getImplementationAction(bool stealth);

    QStringList getKindsRequiringSubActions();
    QList<NodeViewItemAction*> getRequiredSubActionsForKind(QString kind);

private slots:
    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem* viewItem);

    void selectionChanged(int selected);
    void addChildNode();

private:
    void setupNodeActions();

    QHash<QString, RootAction*> nodeKindActions;
    QHash<int, NodeViewItemAction*> actions;

public:
    QToolBar* toolbar;

    ActionGroup* actionGroup;
    ViewController* viewController;

    QStringList interfaceKinds;
    QStringList kindsWithSubActions;

    ActionGroup* adoptableKindsGroup;

};

#endif // TOOLBARCONTROLLER_H
