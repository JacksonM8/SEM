#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include "entityadapter.h"
#include "../View/viewitem.h"
#include "selectionhandler.h"
#include "../Widgets/New/selectioncontroller.h"
#include "actioncontroller.h"
#include "toolbarcontroller.h"

class NewController;
class ToolbarWidgetNew;
class ViewController : public QObject
{
    Q_OBJECT
public:
    ViewController();

    QStringList getNodeKinds();
    SelectionController* getSelectionController();
    ActionController* getActionController();
    ToolActionController* getToolbarController();

    QList<int> getValidEdges(Edge::EDGE_CLASS kind);
    QStringList getAdoptableNodeKinds();
    void setDefaultIcon(ViewItem* viewItem);
    ViewItem* getModel();
    bool isModelReady();

    void setController(NewController* c);
signals:
    void initializeModel();
    void modelReady(bool);
    void viewItemConstructed(ViewItem* viewItem);
    void viewItemDestructing(int ID, ViewItem *viewItem);
    void triggerAction(QString action);
    void dataChanged(int, QString, QVariant);
    void showToolbar(QPoint globalPos, QPointF itemPos = QPointF());


    void view_undo();
    void view_redo();




    void canUndo(bool);
    void canRedo(bool);


    void deleteEntities(QList<int> IDs);
    void constructChildNode(int parentID, QString kind, QPointF pos = QPointF());

public slots:
    void controller_entityConstructed(int ID, ENTITY_KIND eKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties);
    void controller_entityDestructed(int ID, ENTITY_KIND eKind, QString kind);
    void controller_dataChanged(int ID, QString key, QVariant data);
    void controller_dataRemoved(int ID, QString key);

    void controller_propertyChanged(int ID, QString property, QVariant data);
    void controller_propertyRemoved(int ID, QString property);


    void newProject();
private slots:
    void initializeController();


    void table_dataChanged(int ID, QString key, QVariant data);

    void setModelReady(bool okay);
    void deleteSelection();

    void constructDDSQOSProfile();


private:
    QList<int> getIDsOfKind(QString kind);
    bool _modelReady;

    ViewItem* getViewItem(int ID);
    QHash<QString, QList<int> > itemKindLists;
    QHash<int, ViewItem*> viewItems;
    ViewItem* modelItem;

    SelectionController* selectionController;
    ActionController* actionController;
    ToolActionController* toolbarController;

    ToolbarWidgetNew* toolbar;
    NewController* controller;
};

#endif // VIEWCONTROLLER_H
