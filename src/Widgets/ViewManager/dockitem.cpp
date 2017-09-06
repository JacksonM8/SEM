#include "dockitem.h"
#include "../../theme.h"

DockItem::DockItem(ViewManagerWidget *manager, BaseDockWidget *dockWidget)
{
    this->manager = manager;
    this->dockWidget = dockWidget;

    isNodeViewDockWidget = WindowManager::isViewDockWidget(dockWidget);

    setFocusPolicy(Qt::ClickFocus);
    setProperty("ID", dockWidget->getID());

    setContentsMargins(0,0,0,0);
    setIconSize(QSize(16,16));
    setupLayout();

    connect(Theme::theme(), &Theme::theme_Changed, this, &DockItem::themeChanged);
    connect(dockWidget, &BaseDockWidget::dockSetActive, this, &DockItem::themeChanged);
    connect(dockWidget, &BaseDockWidget::titleChanged, this, &DockItem::titleChanged);
    connect(dockWidget, &BaseDockWidget::iconChanged, this, &DockItem::updateIcon);
    themeChanged();
}

void DockItem::updateIcon()
{
    if(iconLabel && dockWidget){
        QPair<QString, QString> icon = dockWidget->getIcon();
        iconLabel->setPixmap(Theme::theme()->getIcon(icon.first, icon.second).pixmap(16,16, QIcon::Normal, QIcon::On));
    }
}

void DockItem::themeChanged()
{
    Theme* theme = Theme::theme();
    if (isNodeViewDockWidget) {
        setStyleSheet(theme->getDockTitleBarStyleSheet(dockWidget->isActive(), "DockItem"));
    } else {
        setStyleSheet(theme->getDockTitleBarStyleSheet(false, "DockItem") +
                      "DockItem {"
                      "background: rgba(0,0,0,0);"
                      "border: 2px solid " + theme->getDisabledBackgroundColorHex() + ";"
                      "}"
                      "DockItem QToolButton::hover{ background:" + theme->getHighlightColorHex() + "}"
                      "DockItem QToolButton::!hover{ background: rgba(0,0,0,0); }");
    }
}

void DockItem::titleChanged()
{
    label->setText(dockWidget->getTitle());
}

void DockItem::setupLayout()
{
    DockTitleBar * titleBar = dockWidget->getTitleBar();

    iconLabel = new QLabel(this);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("margin-right: 2px;");

    label = new QLabel(this);

    QWidget* labelWidget = new QWidget(this);
    QHBoxLayout* labelWidgetLayout = new QHBoxLayout(labelWidget);
    labelWidgetLayout->setMargin(0);
    labelWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    labelWidgetLayout->addWidget(label);
    labelWidgetLayout->addStretch();


    iconAction = addWidget(iconLabel);
    labelAction = addWidget(labelWidget);

    if(titleBar){
        addActions(titleBar->getToolActions());
    }

    titleChanged();
    updateIcon();
}
