#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QActionGroup>
#include <QSignalMapper>
#include <QToolBar>
#include <QToolButton>
#include <QSplitter>
#include <QPushButton>
#include <QMenu>

//#include "enumerations.h"
#include "../../enumerations.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "notificationobject.h"

class NotificationItem;
class NotificationDialog : public QDialog
{
    enum ITEM_ROLES{
        IR_ID = Qt::UserRole + 1,
        IR_ICONPATH = Qt::UserRole + 2,
        IR_ICONNAME = Qt::UserRole + 3,
        IR_ENTITYID = Qt::UserRole + 4,
        IR_TYPE = Qt::UserRole + 5,
        IR_CATEGORY = Qt::UserRole + 6,
        IR_SEVERITY = Qt::UserRole + 7,
        IR_TIMESTAMP = Qt::UserRole + 8
    };

    Q_OBJECT
public:
    explicit NotificationDialog(QWidget *parent = 0);

    void removeNotificationItem(int ID);

signals:
    void centerOn(int entityID);
    void popup(int entityID);

    void itemDeleted(int ID);

    void lastNotificationID(int ID);

    void updateSeverityCount(NOTIFICATION_SEVERITY severity, int count);

    void mouseEntered();

    void filtersCleared();
    void filterButtonToggled(NOTIFICATION_FILTER filter, int filterVal, bool checked);

public slots:
    void toggleVisibility();
    void showDialog();
    void resetDialog();

    void getLastNotificationID();

private slots:
    void themeChanged();
    void listSelectionChanged();

    void severityActionToggled(int actionSeverity);

    void filterMenuTriggered(QAction* action);
    void filterToggled(bool checked);

    void viewSelection();

    void clearSelected();
    void clearVisible();

    void notificationItemAdded(NotificationObject* item);
    void notificationItemClicked(QListWidgetItem* item);

private:
    void setupLayout();
    void setupLayout2();

    void constructFilterButton(ITEM_ROLES role, int roleVal, QString label = "", QString iconPath = "", QString iconName = "");

    void updateVisibilityCount(int val, bool set = false);

    void constructNotificationItem(int ID, NOTIFICATION_SEVERITY severity, QString title, QString description, QString iconPath, QString iconName, int entityID);
    void removeItem(QListWidgetItem* item);

    void clearNotificationsOfSeverity(NOTIFICATION_SEVERITY severity);
    void clearAll();

    void updateSeverityActions(QList<NOTIFICATION_SEVERITY> severities);
    void updateSeverityAction(NOTIFICATION_SEVERITY severity);

    QAction* getSeverityAction(NOTIFICATION_SEVERITY severity) const;
    QPair<QString, QString> getActionIcon(NOTIFICATION_SEVERITY severity) const;

    NOTIFICATION_FILTER getNotificationFilter(ITEM_ROLES role);

    QToolBar* topButtonsToolbar;
    QToolButton* filtersButton;
    QMenu* filtersMenu;
    QPushButton* filterButton;
    QToolBar* filtersToolbar;
    QSplitter* displaySplitter;

    QVBoxLayout* itemsLayout;

    QList<QActionGroup*> actionGroups;
    QList<QAction*> groupSeparators;
    QHash<QAction*, int> prevGroupIndex;
    QHash<ITEM_ROLES, int> indexMap;
    ITEM_ROLES topRole;

    QHash<ITEM_ROLES, QActionGroup*> filterGroups;
    QHash<ITEM_ROLES, QAction*> separators;

    QListWidget* listWidget;
    QToolBar* topToolbar;
    QToolBar* bottomToolbar;
    QToolBar* iconOnlyToolbar;

    QAction* sortTimeAction;
    QAction* sortSeverityAction;
    QAction* centerOnAction;
    QAction* popupAction;
    QAction* clearSelectedAction;
    QAction* clearVisibleAction;

    QHash<NOTIFICATION_TYPE2, QAction*> typeActionHash;
    QHash<NOTIFICATION_CATEGORY, QAction*> categoryActionHash;
    QHash<NOTIFICATION_SEVERITY, QAction*> severityActionHash;

    QHash<QAction*, QToolButton*> filterButtonHash;
    QList<QAction*> checkedActions;

    QMultiMap<NOTIFICATION_SEVERITY, QListWidgetItem*> notificationHash;
    QHash<int, QListWidgetItem*> notificationIDHash;

    int visibleCount;

protected:
    void enterEvent(QEvent* event);

};

#endif // NOTIFICATIONDIALOG_H
