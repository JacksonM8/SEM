#include "toolbarwidgetmenu.h"

#include <QDebug>


/**
 * @brief ToolbarWidgetMenu::ToolbarWidgetMenu
 * @param action
 * @param parent
 */
ToolbarWidgetMenu::ToolbarWidgetMenu(ToolbarWidgetAction *widgetAction, ToolbarWidgetAction* action, QWidget *parent) :
    QMenu(parent)
{
    parentAction = widgetAction;
    defaultAction = action;
    eventFromMenu = false;

    // attach this menu to its parentAction
    if (parentAction) {
        parentAction->setMenu(this);
    }

    // if the parent of this menu is of type ToolbarWidgetMenu, connect to it
    ToolbarWidgetMenu* parentMenu = qobject_cast<ToolbarWidgetMenu*>(parent);
    if (parentMenu) {
        connect(this, SIGNAL(connectToParentMenu(ToolbarWidgetMenu*)),
                parentMenu, SLOT(connectChildMenu(ToolbarWidgetMenu*)));
        emit connectToParentMenu(this);
    }

    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(hideMenu(QAction*)));
    connect(this, SIGNAL(aboutToHide()), this, SLOT(close()));

    // this should always have a parent; print warning if it doesn't
    if (!parent) {
        qDebug() << "WARNING: " << this << " doesn't have a parent. This may cause errors.";
    }
}


/**
 * @brief ToolbarWidgetMenu::execMenu
 * This executes this menu next to its parent ToolbarWidgetAction.
 */
void ToolbarWidgetMenu::execMenu()
{
    if (parentAction) {
        if (widgetActions.count() == 0) {
            if (defaultAction) {
                setupDefaultAction();
            } else {
                return;
            }
        }
        exec(parentAction->getButtonPos());
    }
}


/**
 * @brief ToolbarWidgetMenu::addWidgetAction
 * This stores the newly added ToolbarWidgetAction to the widgetActions list.
 * @param action
 */
void ToolbarWidgetMenu::addWidgetAction(ToolbarWidgetAction* action)
{
    // if widgetActions contains the defaultAction, remove it
    if (widgetActions.count() == 1 && defaultAction) {
        widgetActions.removeAll(defaultAction);
        removeAction(defaultAction);
    }
    widgetActions.append(action);
    addAction(action);
}


/**
 * @brief ToolbarWidgetMenu::getWidgetActions
 * This returns the widgetActions list.
 * @return
 */
QList<ToolbarWidgetAction*> ToolbarWidgetMenu::getWidgetActions()
{
    return widgetActions;
}


/**
 * @brief ToolbarWidgetMenu::getParentAction
 * this returns this menu's parentAction.
 * @return
 */
ToolbarWidgetAction* ToolbarWidgetMenu::getParentAction()
{
    return parentAction;
}


/**
 * @brief ToolbarWidgetMenu::clearMenu
 * This clears this menu. It deletes all of this menu's actions except
 * addInstanceAction and defaultAction if they exist.
 */
void ToolbarWidgetMenu::clearMenu()
{
    QMutableListIterator<ToolbarWidgetAction*> it(widgetActions);
    while (it.hasNext()) {
        ToolbarWidgetAction *action = it.next();
        // NOTE: make sure to check for all kinds that are being stored in the ToolbarWidget
        if (action && action->getKind() != "ComponentInstance" && action->getKind() != "InEventPortDelegate" && action->getKind() != "OutEventPortDelegate" && action->getKind() != "info") {
            delete action;
        } else {
            removeAction(action);
        }
    }
    widgetActions.clear();
}


/**
 * @brief ToolbarWidgetMenu::enterEvent
 * This picks up if the mouse is hovering over this menu.
 */
void ToolbarWidgetMenu::enterEvent(QEvent *e)
{
    eventFromMenu = true;
    QMenu::enterEvent(e);
}


/**
 * @brief ToolbarWidgetMenu::leaveEvent
 * This picks up if the mouse is leaving this menu.
 */
void ToolbarWidgetMenu::leaveEvent(QEvent * e)
{
    eventFromMenu = false;
    QMenu::leaveEvent(e);
}


/**
 * @brief ToolbarWidgetMenu::close
 * If the close event didn't come from this menu, check if it came from its parent menu.
 */
void ToolbarWidgetMenu::close()
{
    if (!eventFromMenu) {
        emit closeParentMenu();
        emit hideToolbar();
    }
}


/**
 * @brief ToolbarWidgetMenu::closeMenu
 * This is called by the children menus when they are closed by an
 * event not from them. If the event didn't come from this menu either,
 * send the same check signal to its parent then close it.
 */
void ToolbarWidgetMenu::closeMenu()
{
    if (!eventFromMenu) {
        QToolButton* button = qobject_cast<QToolButton*>(parent());
        if (button) {
            emit hideToolbar();
        } else {
            emit closeParentMenu();
        }
        hide();
    }
}


/**
 * @brief ToolbarWidgetMenu::hideMenu
 * If an action in this menu is triggered and that action doesn't have a menu, hide this menu.
 * @param action
 */
void ToolbarWidgetMenu::hideMenu(QAction *action)
{
    ToolbarWidgetAction* widgetAction = dynamic_cast<ToolbarWidgetAction*>(action);
    if (widgetAction && widgetAction->getMenu() == 0) {
        hide();
        emit hideToolbar();
    }
}


/**
 * @brief ToolbarWidgetMenu::connectChildMenu
 * This connects the child menu to this menu and its parentAction.
 * @param menu
 */
void ToolbarWidgetMenu::connectChildMenu(ToolbarWidgetMenu* menu)
{
    connect(menu, SIGNAL(closeParentMenu()), this, SLOT(closeMenu()));
}


/**
 * @brief ToolbarWidgetMenu::setupDefaultAction
 */
void ToolbarWidgetMenu::setupDefaultAction()
{
    if (defaultAction) {
        widgetActions.append(defaultAction);
        QMenu::addAction(defaultAction);
    }
}

