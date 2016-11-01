#include "actiongroup.h"
#include "rootaction.h"
#include <QDebug>

ActionGroup::ActionGroup(QObject* parent) : QActionGroup(parent)
{
    setExclusive(false);
    masterAction = 0;
}

RootAction *ActionGroup::getGroupVisibilityAction()
{
    if(!masterAction){
        masterAction = new RootAction("Test", 0);
        updateMasterAction();
    }
    return masterAction;
}

QList<RootAction *> ActionGroup::getRootActions()
{
    QList<RootAction* > rootActions;
    foreach(QAction* action, actions()){
        RootAction* rootAction = qobject_cast<RootAction*>(action);
        if(rootAction){
            rootActions.append(rootAction);
        }
    }
    return rootActions;
}

void ActionGroup::addSeperator()
{
    QAction* seperator = new QAction(this);
    seperator->setSeparator(true);
    addAction(seperator);
}

void ActionGroup::updateSpacers()
{

    QList<QAction*> actions = this->actions();

    QList<int> visibleActionsCount;
    QList<int> spacerActionsIndexes;

    int currentVisibleCount = 0;

    for(int i=0; i < actions.size(); i++){
        QAction* action = actions.at(i);

        if(action->isSeparator()){
            spacerActionsIndexes.append(i);
            visibleActionsCount.append(currentVisibleCount);
            currentVisibleCount = 0;
        }else if(action->isVisible()){
            currentVisibleCount ++;
        }
    }

    visibleActionsCount.append(currentVisibleCount);

    for(int i=0; i < spacerActionsIndexes.size(); i++){
        int seperatorIndex = spacerActionsIndexes.at(i);
        int leftOfSeperator = visibleActionsCount.at(i);

        QAction* seperatorAction = actions.at(seperatorIndex);
        seperatorAction->setVisible(leftOfSeperator > 0 && ((i + 1) != visibleActionsCount.size()));
    }
}

QAction* ActionGroup::addAction(QAction *a)
{

    if(a->isVisible()){
        updateMasterAction();
    }
    connect(a, SIGNAL(changed()), this, SLOT(updateMasterAction()));
    return QActionGroup::addAction(a);
}


void ActionGroup::removeAction(QAction *a)
{
    if(a->isVisible()){
        updateMasterAction();
    }
    disconnect(a, SIGNAL(changed()), this, SLOT(updateMasterAction()));
    QActionGroup::removeAction(a);
}

/**
 * @brief ActionGroup::updateMasterAction
 * Updates
 */
void ActionGroup::updateMasterAction()
{
    if(masterAction){
        bool anyEnabled = false;
        foreach(QAction* action, actions()){
            if(action->isEnabled()){
                anyEnabled = true;
                break;
            }
        }
        masterAction->setEnabled(anyEnabled);
    }
}
