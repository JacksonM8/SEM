#ifndef TOOLBARWIDGETMENU_H
#define TOOLBARWIDGETMENU_H

#include "toolbarwidgetaction.h"

#include <QMenu>

class ToolbarWidgetMenu : public QMenu
{
    Q_OBJECT
public:
    explicit ToolbarWidgetMenu(ToolbarWidgetAction* widgetAction = 0,
                               ToolbarWidgetAction *action = 0,
                               QWidget *parent = 0);

    void clearMenu();

    void addWidgetAction(ToolbarWidgetAction* action);
    QList<ToolbarWidgetAction*> getWidgetActions();

    void setParentAction(ToolbarWidgetAction* widgetAction);
    ToolbarWidgetAction* getParentAction();

protected:
    virtual void enterEvent(QEvent*);
    virtual void leaveEvent(QEvent*);

signals:
    void hideToolbar(bool triggered);
    void closeParentMenu(bool triggered);
    void connectToParentMenu(ToolbarWidgetMenu* menu);

    void resetActionState();

public slots:
    void close();
    void closeMenu(bool triggered);
    void hideMenu(QAction* action);
    void execMenu();

    void setupDefaultAction();
    void connectChildMenu(ToolbarWidgetMenu* menu);

private:    
    ToolbarWidgetAction* parentAction;
    QList<ToolbarWidgetAction*> widgetActions;

    bool eventFromMenu;
    bool actionTriggered;

    ToolbarWidgetAction* defaultAction;

};

#endif // TOOLBARWIDGETMENU_H
