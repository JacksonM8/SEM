#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include "entityadapter.h"
#include "../View/viewitem.h"
#include "selectionhandler.h"
#include "../Widgets/New/selectioncontroller.h"
#include "actioncontroller.h"

class ViewController : public QObject
{
    Q_OBJECT
public:
    ViewController();

    SelectionController* getSelectionController();
    ActionController* getActionController();

    void setDefaultIcon(ViewItem* viewItem);
    ViewItem* getModel();
    bool isModelReady();
signals:
    void modelReady(bool);
    void viewItemConstructed(ViewItem* viewItem);
    void viewItemDestructing(int ID, ViewItem *viewItem);

private slots:
    void setModelReady(bool okay);
    void entityConstructed(EntityAdapter* entity);
    void entityDestructed(EntityAdapter* entity);


private:
    bool _modelReady;
    QHash<int, ViewItem*> viewItems;
    ViewItem* modelItem;

    SelectionController* selectionController;
    ActionController* actionController;
};

#endif // VIEWCONTROLLER_H
