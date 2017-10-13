#include "notificationdialog.h"
#include "notificationitem.h"

#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Controllers/NotificationManager/notificationenumerations.h"
#include "../../Controllers/NotificationManager/notificationobject.h"
#include "../../Controllers/ViewController/viewcontroller.h"
#include <QScrollBar>
#include <QApplication>


#define FILTER_DEFAULT_WIDTH 150

/**
 * @brief NotificationDialog::NotificationDialog
 * @param parent
 */
NotificationDialog::NotificationDialog(ViewController* viewController, QWidget *parent)
    : QFrame(parent)
{
    this->viewController = viewController;
    setupLayout();
    initialisePanel();

    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationDialog::themeChanged);
    connect(viewController->getSelectionController(), &SelectionController::selectionChanged, this, &NotificationDialog::selectionChanged);

    connect(center_action, &QAction::triggered, this, &NotificationDialog::centerEntity);
    connect(popup_action, &QAction::triggered, this, &NotificationDialog::popupEntity);
    connect(clear_filtered_action, &QAction::triggered, this, &NotificationDialog::clearFilteredNotifications);

    auto manager = NotificationManager::manager();
    connect(manager, &NotificationManager::notificationUpdated, this, &NotificationDialog::updateNotificationsVisibility);
    connect(manager, &NotificationManager::notificationAdded, this, &NotificationDialog::notificationAdded);
    connect(manager, &NotificationManager::notificationDeleted, this, &NotificationDialog::notificationDeleted);
    connect(manager, &NotificationManager::showNotificationPanel, this, &NotificationDialog::updateNotificationsVisibility);
    connect(this, &NotificationDialog::mouseEntered, manager, &NotificationManager::notificationsSeen);

    auto v_scrollbar = notifications_scroll->verticalScrollBar();
    connect(v_scrollbar, &QScrollBar::valueChanged, this, &NotificationDialog::scrollBarValueChanged);
    connect(load_more_button, &QToolButton::clicked, this, &NotificationDialog::loadNextResults);

    connect(sort_time_action, &QAction::triggered, this, &NotificationDialog::filtersChanged);
    
    themeChanged();
}


/**
 * @brief NotificationDialog::filtersChanged
 */
void NotificationDialog::filtersChanged()
{
    //Reset so that only 50 notifications are shown initially
    max_visible = 30;
    //Reset the scroll bar back to the top
    auto v_scrollbar = notifications_scroll->verticalScrollBar();
    v_scrollbar->setValue(0);
    updateNotificationsVisibility();
}

QSet<QSharedPointer<NotificationObject> > NotificationDialog::getFilteredNotifications(){
    //Get the Checked filters
    auto checked_context_set = context_filters->getCheckedOptions<Notification::Context>().toSet();
    auto checked_severity_set = severity_filters->getCheckedOptions<Notification::Severity>().toSet();
    auto checked_category_set = category_filters->getCheckedOptions<Notification::Category>().toSet();
    auto checked_type_set = source_filters->getCheckedOptions<Notification::Type>().toSet();
    
    //We only need to check selection when the SELECTED is exclusively selected.
    auto check_selection = checked_context_set.size() == 1 && checked_context_set.contains(Notification::Context::SELECTED);
    QSet<int> selected_ids;
    if(viewController && check_selection){
        selected_ids = viewController->getSelectionController()->getSelectionIDs().toSet();
    }

    QSet<QSharedPointer<NotificationObject> > filtered;

    for(auto notification : NotificationManager::manager()->getNotifications()){
        auto matches_severity = checked_severity_set.contains(notification->getSeverity());
        auto matches_category = checked_category_set.contains(notification->getCategory());
        auto matches_type = checked_type_set.contains(notification->getType());
        auto matches_context = check_selection ? selected_ids.contains(notification->getEntityID()) : true;
        //Check if this notification matches all the filters
        auto matches_filters = matches_severity && matches_category && matches_type && matches_context;
        if(matches_filters){
            filtered.insert(notification);
        }
    }
    return filtered;
}

void NotificationDialog::updateNotificationsVisibility(){
    
    
    //Get All the notifications 
    auto all_notifications = NotificationManager::manager()->getNotifications();
    //Get the set of filtered notifications
    filtered_notifications = getFilteredNotifications();
    
    //Update counts
    total_notifications = all_notifications.size();
    current_visible_notifications = 0;
    current_matched_notifications = 0;

    //Sort the list by ascending or descending time
    qSort(all_notifications.begin(), all_notifications.end(), 
        [=](const QSharedPointer<NotificationObject> a, const QSharedPointer<NotificationObject> b) -> bool{
            bool agtb = a->getModifiedTime() > b->getModifiedTime();
            return sort_time_action->isChecked() ? agtb : !agtb;
        });

    for(auto notification : all_notifications){
        //Check if this notification matches all the filters
        auto matches_filters = filtered_notifications.contains(notification);

        if(matches_filters){
            current_matched_notifications ++;
        }
        
        //Only set this notification as visible if it matches all filters and is less 
        auto set_visible = matches_filters && current_visible_notifications < max_visible;

        //If we've got a match WE have to construct a Notificationitem, else if we don't need, we only need to hide the previously constructed items
        auto notification_item = set_visible ? constructNotificationItem(notification) : getNotificationItem(notification);

        if(notification_item){
            if(set_visible){
                //Add, which because we are processing the notificationObjects in the correct order, will ensure they are added Visually in order too
                notifications_layout->addWidget(notification_item);
            }else{
                //Remove Widgets which aren't meant to be shown with the current filters
                notifications_layout->removeWidget(notification_item);
            }

            
            notification_item->setVisible(set_visible);
            if(set_visible){
                current_visible_notifications ++;
            }
        }
    }

    updateLabels();
}

void NotificationDialog::updateLabels(){
     //Show the No Notification label
     info_label->setVisible(total_notifications == 0);
     
    //Check if there are notifications which are hidden by the filters
    auto any_filtered = total_notifications > current_matched_notifications;
    //Check if all the matched filter results are being shown
    auto all_showing = current_visible_notifications == current_matched_notifications;

    //Update the text for the status_label
    if(any_filtered){
        status_label->setText(QString::number(total_notifications - current_matched_notifications) + "/" + QString::number(total_notifications) + " notifications hidden by filters");
    } 
    notifications_status_widget->setVisible(any_filtered);

    load_more_button->setVisible(!all_showing);
}

/**
 * @brief NotificationDialog::themeChanged
 */
void NotificationDialog::themeChanged()
{
    auto theme = Theme::theme();
    
    setStyleSheet(
                    "NotificationDialog {background-color: " % theme->getBackgroundColorHex() + ";border:1px solid " % theme->getDisabledBackgroundColorHex() % ";}" +
                    "QScrollArea {border: 1px solid " % theme->getAltBackgroundColorHex() % "; background: rgba(0,0,0,0); } " +
                    "QLabel {color:" + theme->getTextColorHex() + ";} " + 
                    theme->getToolBarStyleSheet() +
                    theme->getSplitterStyleSheet() +
                    "QToolButton::checked:!hover{background: " % theme->getAltBackgroundColorHex() % ";}"
                );

    notifications_widget->setStyleSheet("background: rgba(0,0,0,0);");
    filters_widget->setStyleSheet("background: rgba(0,0,0,0);");


    center_action->setIcon(theme->getIcon("Icons", "crosshair"));
    popup_action->setIcon(theme->getIcon("Icons", "popOut"));
    clear_filtered_action->setIcon(theme->getIcon("Icons", "bin"));
    
    sort_time_action->setIcon(theme->getIcon("ToggleIcons", "sort"));
    reset_filters_action->setIcon(theme->getIcon("Icons", "cross"));
    clock_action->setIcon(theme->getIcon("Icons", "clock"));
    
    //auto pixmap = Theme::theme()->getImage("Icons", "clock", QSize(16,16), theme->getMenuIconColor());
    //clock_label->setPixmap(pixmap);

    info_label->setStyleSheet("color:" + theme->getAltBackgroundColorHex() + ";");
    load_more_button->setStyleSheet(theme->getToolBarStyleSheet() + "QToolButton{border-radius:0px;}");

    top_toolbar->setIconSize(theme->getIconSize());
    bottom_toolbar->setIconSize(theme->getIconSize());
}

void NotificationDialog::popupEntity(){
    if(selected_notification){
        auto entity_id = selected_notification->getEntityID();
        if(entity_id != -1){
            viewController->popupItem(entity_id);
        }
    }
}

void NotificationDialog::centerEntity(){
    if(selected_notification){
        auto entity_id = selected_notification->getEntityID();
        if(entity_id != -1){
            viewController->centerOnID(entity_id);
        }
    }
}
void NotificationDialog::clearFilteredNotifications(){
    auto manager = NotificationManager::manager();
    for(auto notification : getFilteredNotifications()){
        manager->deleteNotification(notification->getID());
    }
}


void NotificationDialog::ToggleSelection(NotificationItem* item){
    bool set_selected = selected_notification != item;

    
    if(selected_notification){
        //unselect old notification
        selected_notification->setSelected(!set_selected);
    }

    if(item){
        item->setSelected(set_selected);
    }
    selected_notification = item;

    
    bool has_entity = selected_notification && selected_notification->getEntityID() != -1;
    
    center_action->setEnabled(has_entity);
    popup_action->setEnabled(has_entity);
}


void NotificationDialog::selectionChanged(){
    auto checked_context_set = context_filters->getCheckedOptions<Notification::Context>().toSet();
    if(checked_context_set.contains(Notification::Context::SELECTED)){
        filtersChanged();
    }
}

/**
 * @brief NotificationDialog::notificationAdded
 * @param obj
 */
void NotificationDialog::notificationAdded(QSharedPointer<NotificationObject> notification)
{
    updateNotificationsVisibility();
}


NotificationItem* NotificationDialog::constructNotificationItem(QSharedPointer<NotificationObject> notification){
    auto notification_item = getNotificationItem(notification);
    if(notification && !notification_item){
        auto id = notification->getID();
        notification_item = new NotificationItem(notification, this);
        notification_item->setVisible(false);
      
        //Add to map
        notification_items[id] = notification_item;
        connect(notification_item, &NotificationItem::highlightEntity, viewController, &ViewController::vc_highlightItem);
        connect(notification_item, &NotificationItem::itemClicked, this, &NotificationDialog::ToggleSelection);
    }
    return notification_item;
}

NotificationItem* NotificationDialog::getNotificationItem(QSharedPointer<NotificationObject> notification){
    if(notification){
        return notification_items.value(notification->getID(), 0);
    }
    return 0;
}

/**
 * @brief NotificationDialog::notificationDeleted
 * Delete notification item with the provided ID from the hash and the items layout.
 * @param ID
 */
void NotificationDialog::notificationDeleted(QSharedPointer<NotificationObject> notification){
    auto ID = notification->getID();
    auto notification_item = notification_items.value(ID, 0);
    auto is_visible = notification_item ? notification_item->isVisible() : false;

    if(filtered_notifications.contains(notification)){
        filtered_notifications.remove(notification);
        if(is_visible){
            current_visible_notifications--;
            notification_item->hide();
        }
        current_matched_notifications--;
        total_notifications--;
        updateLabels();
    }

    if(selected_notification == notification_item){
        ToggleSelection(0);
    }

    if(notification_item){
        notification_items.remove(ID);
        //delete notification;
        notification_item->deleteLater();
    }
}

/**
 * @brief NotificationDialog::initialisePanel
 * Construct items for notifications that were received before this dialog was constructed.
 * Show highlight alert and toast for the last notification item in the list.
 */
void NotificationDialog::initialisePanel()
{
    filtersChanged();
}


/**
 * @brief NotificationDialog::enterEvent
 * If this dialog is currently active, mark new notification as seen on mouse enter.
 * @param event
 */
void NotificationDialog::enterEvent(QEvent* event)
{
    QWidget::enterEvent(event);

    if (QApplication::activeWindow()->isAncestorOf(this)){
        emit mouseEntered();
    }
}


/**
 * @brief NotificationDialog::setupLayout
 */
void NotificationDialog::setupLayout()
{
    auto left_widget = new QWidget(this);
    auto right_widget = new QWidget(this);

    {
        //LEFT WIDGET
        left_widget->setContentsMargins(5,5,1,5);
        auto v_layout = new QVBoxLayout(left_widget);
        v_layout->setMargin(0);

        filters_widget = new QWidget(this);
        filters_widget->setContentsMargins(5,0,5,5);
        filters_layout = new QVBoxLayout(filters_widget);
        filters_layout->setAlignment(Qt::AlignTop);
        filters_layout->setMargin(0);
        filters_layout->setSpacing(0);

        filters_scroll = new QScrollArea(this);
        filters_scroll->setWidget(filters_widget);
        filters_scroll->setWidgetResizable(true);

        v_layout->addWidget(filters_scroll, 1);
    }

    {
        //RIGHT WIDGET
        right_widget->setContentsMargins(1,5,5,5);
        auto v_layout = new QVBoxLayout(right_widget);
        v_layout->setMargin(0);
        v_layout->setSpacing(5);

        top_toolbar = new QToolBar(this);
        top_toolbar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding));

        /*clock_label = new QLabel(this);
        clock_label->setFixedSize(QSize(16, 16));
        clock_label->setAlignment(Qt::Alivoid showSeverity(Notification::Severity severity);void showSeverity(Notification::Severity severity);gnCenter);
        clock_label->setToolTip("Sort Notifications by time (Ascending/Descending)");
        
        top_toolbar->addWidget(clock_label);*/

        clock_action = top_toolbar->addAction("Sort Notifications by time (Ascending/Descending)");

        auto button = (QToolButton*) top_toolbar->widgetForAction(clock_action);
        button->setAutoRaise(false);
        button->setStyleSheet("QToolButton{background:none;border:none;}");


        sort_time_action = top_toolbar->addAction("Sort by time");
        sort_time_action->setToolTip(clock_action->toolTip());
        sort_time_action->setCheckable(true);
        sort_time_action->setChecked(true);



        
        top_toolbar->addSeparator();
        center_action = top_toolbar->addAction("Center On Notification");
        popup_action = top_toolbar->addAction("Popup On Notification");
        top_toolbar->addSeparator();
        clear_filtered_action = top_toolbar->addAction("Delete filtered notifications");
        
    

        info_label = new QLabel("No notifications", this);
        info_label->setAlignment(Qt::AlignCenter);
        info_label->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        
        //Used for the ScrollArea
        notifications_widget = new QWidget(this);
        {
            auto v2_layout = new QVBoxLayout(notifications_widget);
            v2_layout->setAlignment(Qt::AlignTop);
            v2_layout->setSpacing(0);
            v2_layout->setMargin(0);

            notifications_layout = new QVBoxLayout();
            notifications_layout->setAlignment(Qt::AlignTop);
            notifications_layout->setSpacing(0);
            notifications_layout->setMargin(0);

            //Add the No Results info label to the results layout
            notifications_layout->addWidget(info_label);
            
            load_more_button = new QToolButton(this);
            load_more_button->setText("Load next 10 notifications");
            load_more_button->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred));
            load_more_button->setVisible(false);

            v2_layout->addLayout(notifications_layout);
            v2_layout->addWidget(load_more_button);
        }

    
        notifications_scroll = new QScrollArea(this);
        notifications_scroll->setWidget(notifications_widget);
        notifications_scroll->setWidgetResizable(true);


        {
            //Is the Notification label in the status bar
            status_label = new QLabel(this);
            status_label->setAlignment(Qt::AlignCenter);
            status_label->setFont(QFont(font().family(), 12));
            status_label->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred));

            bottom_toolbar = new QToolBar(this);
            bottom_toolbar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding));
            reset_filters_action = bottom_toolbar->addAction("Reset Filters");

            notifications_status_widget = new QWidget(this);
            notifications_status_widget->hide();
            auto status_layout = new QHBoxLayout(notifications_status_widget);
            status_layout->setContentsMargins(0, 0, 0, 0);
            status_layout->addWidget(status_label, 1);
            status_layout->addWidget(bottom_toolbar);
        }


        //Right
        v_layout->addWidget(top_toolbar, 0, Qt::AlignRight);
        v_layout->addWidget(notifications_scroll, 1);
        v_layout->addWidget(notifications_status_widget);
    }

    splitter = new QSplitter(this);
    splitter->addWidget(left_widget);
    splitter->addWidget(right_widget);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes(QList<int>() << FILTER_DEFAULT_WIDTH << 2 * FILTER_DEFAULT_WIDTH);

    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(splitter, 1);
    setupFilters();

    
}

void NotificationDialog::showSeverity(Notification::Severity severity){
    reset_filters_action->trigger();
    severity_filters->setOptionChecked(QVariant::fromValue(severity), true);
}

/**
 * @brief NotificationDialog::setupFilters
 */
void NotificationDialog::setupFilters()
{
    context_filters = new OptionGroupBox("CONTEXT", this);

    auto contexts = Notification::getContexts().toList();
    qSort(contexts.begin(), contexts.end());
    for(auto context : contexts){
        auto context_str = Notification::getContextString(context);
        context_filters->addOption(QVariant::fromValue(context), context_str, "Notification", context_str);
    }
    //Hide the NOT_SELECTED option
    context_filters->setOptionVisible(QVariant::fromValue(Notification::Context::NOT_SELECTED), false);

    filters_layout->addWidget(context_filters);


    severity_filters = new OptionGroupBox("STATUS", this);
    
    auto severities = Notification::getSeverities().toList();
    qSort(severities.begin(), severities.end());
    for (auto severity : severities) {
        auto severity_str = Notification::getSeverityString(severity);
        severity_filters->addOption(QVariant::fromValue(severity), severity_str, "Notification", severity_str);
    }
    severity_filters->setOptionVisible(QVariant::fromValue(Notification::Severity::NONE), false);
    
    filters_layout->addWidget(severity_filters);

    category_filters = new OptionGroupBox("CATEGORY", this);
    auto categories = Notification::getCategories().toList();
    qSort(categories.begin(), categories.end());
    for (auto category : categories) {
        auto category_str = Notification::getCategoryString(category);
        category_filters->addOption(QVariant::fromValue(category), category_str, "Notification", category_str);
    }
    category_filters->setOptionVisible(QVariant::fromValue(Notification::Category::NONE), false);
    filters_layout->addWidget(category_filters);

    source_filters = new OptionGroupBox("SOURCE", this);
    auto types = Notification::getTypes().toList();
    qSort(types.begin(), types.end());
    for (auto type : types) {
        auto type_str = Notification::getTypeString(type);
        source_filters->addOption(QVariant::fromValue(type), type_str, "Notification", type_str);
    }
    filters_layout->addWidget(source_filters);

    connect(context_filters, &OptionGroupBox::checkedOptionsChanged, this, &NotificationDialog::filtersChanged);
    connect(severity_filters, &OptionGroupBox::checkedOptionsChanged, this, &NotificationDialog::filtersChanged);
    connect(category_filters, &OptionGroupBox::checkedOptionsChanged, this, &NotificationDialog::filtersChanged);
    connect(source_filters, &OptionGroupBox::checkedOptionsChanged, this, &NotificationDialog::filtersChanged);

    connect(reset_filters_action, &QAction::triggered, context_filters, &OptionGroupBox::reset);
    connect(reset_filters_action, &QAction::triggered, severity_filters, &OptionGroupBox::reset);
    connect(reset_filters_action, &QAction::triggered, category_filters, &OptionGroupBox::reset);
    connect(reset_filters_action, &QAction::triggered, source_filters, &OptionGroupBox::reset);
}

void NotificationDialog::scrollBarValueChanged()
{
    auto v_scroll = notifications_scroll->verticalScrollBar();
    if(v_scroll && v_scroll->value() > 0 && v_scroll->value() == v_scroll->maximum()){
        loadNextResults();
    }
}

void NotificationDialog::loadNextResults(){
    if(current_visible_notifications < current_matched_notifications){
        max_visible += 10;
        updateNotificationsVisibility();
    }
}
