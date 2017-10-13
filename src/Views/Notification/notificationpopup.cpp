#include "notificationpopup.h"
#include "../../theme.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Controllers/NotificationManager/notificationobject.h"
#include "../../Controllers/WindowManager/windowmanager.h"
#include <QHBoxLayout>

NotificationPopup::NotificationPopup():PopupWidget(PopupWidget::TYPE::SPLASH, 0) {
    setupLayout();
    timer = new QTimer(this);
    timer->setInterval(5000);

    setAttribute(Qt::WA_ShowWithoutActivating);

    //Hide the notification popup on timeout
    connect(timer, &QTimer::timeout, this, &QDialog::hide);
    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationPopup::themeChanged);

    themeChanged();
}

void NotificationPopup::DisplayNotification(QSharedPointer<NotificationObject> notification){
    timer->stop();

    auto font_metrics = label->fontMetrics();
    auto notification_text  = font_metrics.elidedText(notification->getDescription(), Qt::ElideMiddle, 500);
    
    if(notification_text != label->text()){
        label->setText(notification_text);
    }

    auto icon_size = icon->size();
    if(notification->getSeverity() == Notification::Severity::RUNNING){
        auto movie = Theme::theme()->getGif("Icons", "loading");
        icon->setMovie(movie);
    }else{
        auto icon = notification->getIcon();
        auto icon_color = Theme::theme()->getSeverityColor(notification->getSeverity());
        auto pixmap = Theme::theme()->getImage(icon.first, icon.second, icon_size, icon_color);
        
        if (pixmap.isNull()) {
            pixmap = Theme::theme()->getImage("Icons", "circleInfo", icon_size, icon_color);
        }
    
        if(!this->icon->pixmap() || this->icon->pixmap()->cacheKey() != pixmap.cacheKey()){
            this->icon->setPixmap(pixmap);
        }
    }

    QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);

 
    timer->start();
    
}

void NotificationPopup::themeChanged(){
    auto theme = Theme::theme();
    setStyleSheet("QLabel{ background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + ";}");
    label->setFont(theme->getLargeFont());
    icon->setFixedSize(theme->getLargeIconSize());
}

void NotificationPopup::setupLayout(){
    widget = new QWidget(this);
    
    icon = new QLabel(this);
    icon->setScaledContents(true);
    icon->setAlignment(Qt::AlignCenter);
    
    
    label = new QLabel(this);
    
    
    widget->setContentsMargins(5, 2, 5, 2);
    
    auto layout = new QHBoxLayout(widget);
    layout->setMargin(0);
    layout->setSpacing(5);
    layout->addWidget(icon);
    layout->addWidget(label);
    
    setWidget(widget);
}