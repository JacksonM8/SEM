#ifndef NOTIFICATIONTOOLBAR_H
#define NOTIFICATIONTOOLBAR_H

#include "../../Controllers/NotificationManager/notificationenumerations.h"

#include <QToolBar>
#include <QHash>
#include <QLabel>
#include <QMovie>

class NotificationToolbar : public QToolBar
{
    Q_OBJECT
public:
    explicit NotificationToolbar(QWidget *parent = 0);
private slots:
    void themeChanged();
    void notificationAdded();
    void notificationsSeen();
    void loadingGifTicked();
private:
    void setLoadingMode(bool loading);

    void updateCount();
    void setupLayout();
    void initialiseToolbar();
    void updateButtonIcon();

    QAction* show_notification_dialog_action = 0;
    QAction* show_most_recent_action = 0;
    QMovie* loading_gif = 0;

    QIcon default_icon;
    QHash<Notification::Severity, QLabel*> severity_labels;
};

#endif // NOTIFICATIONTOOLBAR_H
