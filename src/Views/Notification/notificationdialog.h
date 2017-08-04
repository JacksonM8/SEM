#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSignalMapper>
#include <QToolBar>
#include <QToolButton>
#include <QSplitter>
#include <QPushButton>
#include <QMenu>
#include <QLabel>
#include <QTimer>

#include "../../theme.h"
#include "../../enumerations.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Controllers/NotificationManager/notificationEnumerations.h"
#include "../../Utils/actiongroup.h"
#include "../../Utils/filtergroup.h"
#include "../../Widgets/optiongroupbox.h"

class NotificationItem;
class NotificationDialog : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationDialog(QWidget *parent = 0);

signals:
    void centerOn(int entityID);
    void popup(int entityID);

    void deleteNotification(int ID);
    void lastNotificationID(int ID);

    void mouseEntered();

    void itemHoverEnter(int ID);
    void itemHoverLeave(int ID);

public slots:
    void initialisePanel();
    void resetPanel();

    void notificationAdded(NotificationObject* obj);
    void notificationDeleted(int ID);

    void entitySelectionChanged(int ID);

    void getLastNotificationID();

private slots:
    void themeChanged();
    void filtersChanged();
    void selectionChanged(NotificationItem* item, bool selected, bool controlDown);

    void selectionFilterToggled(bool);

    void viewSelection();

    void clearSelected();
    void clearVisible();
    void clearNotifications(NOTIFICATION_FILTER filter, int filterVal);

private:
    void setupLayout();
    void setupFilters();

    void removeItem(NotificationItem* item);

    void clearAll();
    void clearSelection();

    void updateSelectionBasedButtons();
    
    QWidget* mainWidget;

    QSplitter* displaySplitter;
    QToolBar* filtersToolbar;
    QToolBar* topToolbar;
    QToolBar* bottomToolbar;

    QVBoxLayout* itemsLayout;

    QAction* displayAllAction;
    QAction* displayToggleAction;
    QAction* displayLinkedItemsAction;

    QAction* sortTimeAction;
    QAction* sortSeverityAction;
    QAction* centerOnAction;
    QAction* popupAction;
    QAction* clearSelectedAction;
    QAction* clearVisibleAction;

    QHash<int, NotificationItem*> notificationItems;
    QHash<NOTIFICATION_FILTER, OptionGroupBox*> filters;

    QList<NotificationItem*> selectedItems;
    QList<int> linkedEntityIDs;
    int selectedEntityID;

protected:
    void enterEvent(QEvent* event);

};

#endif // NOTIFICATIONDIALOG_H
