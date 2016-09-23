#include "medeamainwindow.h"
#include "medeaviewdockwidget.h"
#include "medeatooldockwidget.h"
#include "selectioncontroller.h"
#include "medeanodeviewdockwidget.h"
#include "viewmanagerwidget.h"

#include "../../View/theme.h"
#include "../../Controller/settingscontroller.h"
#include "../../Controller/settingscontroller.h"

#include <QDebug>
#include <QHeaderView>
#include <QPushButton>
#include <QMenuBar>
#include <QDateTime>
#include <QApplication>
#include <QStringBuilder>
#include <QStringListModel>
#include <QTabWidget>

#define TOOLBAR_HEIGHT 32


/**
 * @brief MedeaMainWindow::MedeaMainWindow
 * @param vc
 * @param parent
 */
MedeaMainWindow::MedeaMainWindow(ViewController *vc, QWidget* parent):MedeaWindowNew(parent, MedeaWindowNew::MAIN_WINDOW)
{
    qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
    SettingsController::initializeSettings();
    connect(SettingsController::settings(), SIGNAL(settingChanged(SETTING_KEY,QVariant)), this, SLOT(settingChanged(SETTING_KEY,QVariant)));

    initializeApplication();

    applicationToolbar = 0;
    jenkinsManager = 0;
    cutsManager = 0;
    viewController = vc;

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    setupTools(); //861MS
    setupInnerWindow(); //718
    setupJenkinsManager();
    setupCUTSManager();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(MedeaWindowManager::manager(), SIGNAL(activeViewDockWidgetChanged(MedeaViewDockWidget*,MedeaViewDockWidget*)), this, SLOT(activeViewDockWidgetChanged(MedeaViewDockWidget*, MedeaViewDockWidget*)));
    setViewController(vc);

    themeChanged();
    qint64 time2 = QDateTime::currentDateTime().toMSecsSinceEpoch();

    SettingsController* s = SettingsController::settings();

    int width = s->getSetting(SK_GENERAL_WIDTH).toInt();
    int height = s->getSetting(SK_GENERAL_HEIGHT).toInt();
    resize(width, height);

    if(SettingsController::settings()->getSetting(SK_GENERAL_MAXIMIZED).toBool()){
        showMaximized();
    }else{
        showNormal();
    }

    qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();

    toggleWelcomeScreen(true);

    qCritical() << "MedeaMainWindow in: " <<  time2 - timeStart << "MS";
    qCritical() << "MedeaMainWindow->show() in: " <<  timeFinish - time2 << "MS";
    setModelTitle("");

    resizeToolWidgets();
}


/**
 * @brief MedeaMainWindow::~MedeaMainWindow
 */
MedeaMainWindow::~MedeaMainWindow()
{
    cutsManager->deleteLater();

    saveSettings();

    SettingsController::teardownSettings();
    Theme::teardownTheme();
}


/**
 * @brief MedeaMainWindow::setViewController
 * @param vc
 */
void MedeaMainWindow::setViewController(ViewController *vc)
{
    viewController = vc;
    SelectionController* controller = vc->getSelectionController();
    ActionController* actionController = vc->getActionController();

    connect(controller, &SelectionController::itemActiveSelectionChanged, tableWidget, &TableWidget::itemActiveSelectionChanged);

    connect(vc, &ViewController::mc_projectModified, this, &MedeaMainWindow::setWindowModified);
    connect(vc, &ViewController::vc_projectPathChanged, this, &MedeaMainWindow::setModelTitle);
    connect(vc, &ViewController::vc_showNotification, this, &MedeaMainWindow::showNotification);

    connect(vc, &ViewController::vc_showWelcomeScreen, this, &MedeaMainWindow::toggleWelcomeScreen);
    connect(vc, &ViewController::mc_projectModified, this, &MedeaMainWindow::setWindowModified);
    connect(actionController, &ActionController::recentProjectsUpdated, this, &MedeaMainWindow::recentProjectsUpdated);

    if (vc->getActionController()) {
        connect(vc->getActionController()->getRootAction("Root_Search"), SIGNAL(triggered(bool)), this, SLOT(popupSearch()));
    }
}


/**
 * @brief MedeaMainWindow::showCompletion
 * @param list
 */
void MedeaMainWindow::updateSearchSuggestions(QStringList list)
{
    searchCompleterModel->setStringList(list);
}


/**
 * @brief MedeaMainWindow::searchEntered
 */
void MedeaMainWindow::searchEntered()
{
    QString query = searchBar->text();
    if (!query.isEmpty()) {
        qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
        searchDialog->searchResults(query, viewController->getSearchResults(query));
        qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();
        searchDialog->show();
        qCritical() << "searchEntered in: " <<  timeFinish - timeStart << "MS";
    }
}


/**
 * @brief MedeaMainWindow::showNotification
 * @param title
 * @param message
 */
void MedeaMainWindow::showNotification(NOTIFICATION_TYPE type, QString title, QString description, QString iconPath, QString iconName, int ID)
{
    notificationDialog->addNotificationItem(type, title, description, QPair<QString, QString>(iconPath, iconName), ID);
    notificationTimer->stop();

    if (!welcomeScreenOn) {
        notificationLabel->setText(description);
        QPixmap pixmap = Theme::theme()->getIcon(iconPath, iconName).pixmap(QSize(32,32));
        if (pixmap.isNull()) {
            pixmap = Theme::theme()->getIcon("Actions", "Info").pixmap(QSize(32,32));
        }
        notificationIconLabel->setPixmap(pixmap);
        notificationPopup->setSize(notificationWidget->sizeHint().width() + 15, notificationWidget->sizeHint().height() + 10);
        moveWidget(notificationPopup, 0, Qt::AlignBottom);
        notificationPopup->show();
        notificationTimer->start(5000);
    }
}


/**
 * @brief MedeaMainWindow::showProgressBar
 * @param show
 * @param description
 */
void MedeaMainWindow::showProgressBar(bool show, QString description)
{
    if (show && progressLabel->text() != description) {
        progressLabel->setText(description);
    }

    if (show) {
        Qt::Alignment alignment = welcomeScreenOn ? Qt::AlignBottom : Qt::AlignCenter;
        moveWidget(progressPopup, this, alignment);
    } else {
        progressBar->reset();
    }

    progressPopup->setVisible(show);
}


/**
 * @brief MedeaMainWindow::updateProgressBar
 * @param value
 */
void MedeaMainWindow::updateProgressBar(int value)
{
    if (progressPopup->isVisible()) {
        if (value == -1) {
            progressBar->setRange(0,0);
        } else {
            progressBar->setRange(0,100);
            progressBar->setValue(value);
        }
    }
}


/**
 * @brief MedeaMainWindow::resetToolDockWidgets
 */
void MedeaMainWindow::resetToolDockWidgets()
{
    foreach (MedeaDockWidget* child, getDockWidgets()) {
        child->setVisible(true);
    }
}


/**
 * @brief MedeaMainWindow::themeChanged
 */
void MedeaMainWindow::themeChanged()
{
    Theme* theme = Theme::theme();

    QString menuStyle = theme->getMenuStyleSheet();
    viewController->getActionController()->menu_file->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_file_recentProjects->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_edit->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_view->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_model->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_jenkins->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_help->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_window->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_options->setStyleSheet(menuStyle);

    searchCompleter->popup()->setStyleSheet(theme->getAbstractItemViewStyleSheet() % theme->getScrollBarStyleSheet() % "QAbstractItemView::item{ padding: 2px 0px; }");
    searchPopup->setStyleSheet(theme->getPopupWidgetStyleSheet());
    searchToolbar->setStyleSheet(theme->getToolBarStyleSheet());
    searchBar->setStyleSheet(theme->getLineEditStyleSheet());
    searchButton->setIcon(theme->getIcon("Actions", "Search"));

    progressPopup->setStyleSheet(theme->getPopupWidgetStyleSheet());
    progressBar->setStyleSheet(theme->getProgressBarStyleSheet());
    progressLabel->setStyleSheet("background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + ";");

    notificationPopup->setStyleSheet(theme->getPopupWidgetStyleSheet());
    notificationLabel->setStyleSheet("background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + ";");

    restoreAspectsButton->setIcon(theme->getIcon("Actions", "MenuView"));
    restoreToolsButton->setIcon(theme->getIcon("Actions", "Build"));
    restoreToolsAction->setIcon(theme->getIcon("Actions", "Refresh"));

    interfaceButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_INTERFACES));
    behaviourButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_BEHAVIOUR));
    assemblyButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_ASSEMBLIES));
    hardwareButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_HARDWARE));

    minimap->setStyleSheet(theme->getNodeViewStyleSheet());
}


/**
 * @brief MedeaMainWindow::activeViewDockWidgetChanged
 * @param viewDock
 * @param prevDock
 */
void MedeaMainWindow::activeViewDockWidgetChanged(MedeaViewDockWidget *viewDock, MedeaViewDockWidget *prevDock)
{
    if(viewDock && viewDock->isNodeViewDock()){
        MedeaNodeViewDockWidget* nodeViewDock = (MedeaNodeViewDockWidget*) viewDock;
        NodeViewNew* view = nodeViewDock->getNodeView();

        if(prevDock && prevDock->isNodeViewDock()){
            MedeaNodeViewDockWidget* prevNodeViewDock = (MedeaNodeViewDockWidget*) prevDock;
            NodeViewNew* prevView = prevNodeViewDock->getNodeView();
            if(prevView){
                disconnect(minimap, SIGNAL(minimap_Pan(QPointF)), prevView, SLOT(minimap_Pan(QPointF)));
                disconnect(minimap, SIGNAL(minimap_Panning(bool)), prevView, SLOT(minimap_Panning(bool)));
                disconnect(minimap, SIGNAL(minimap_Zoom(int)), prevView, SLOT(minimap_Zoom(int)));

                disconnect(prevView, &NodeViewNew::sceneRectChanged, minimap, &NodeViewMinimap::sceneRectChanged);

                disconnect(prevView, SIGNAL(viewportChanged(QRectF, qreal)), minimap, SLOT(viewportRectChanged(QRectF, qreal)));
            }
        }

        if(view){
            minimap->setBackgroundColor(view->getBackgroundColor());
            minimap->setScene(view->scene());

            connect(minimap, SIGNAL(minimap_Pan(QPointF)), view, SLOT(minimap_Pan(QPointF)));
            connect(minimap, SIGNAL(minimap_Panning(bool)), view, SLOT(minimap_Panning(bool)));
            connect(minimap, SIGNAL(minimap_Zoom(int)), view, SLOT(minimap_Zoom(int)));
            connect(view, SIGNAL(viewportChanged(QRectF, qreal)), minimap, SLOT(viewportRectChanged(QRectF, qreal)));
            connect(view, &NodeViewNew::sceneRectChanged, minimap, &NodeViewMinimap::sceneRectChanged);

            view->viewportChanged();
        }
    }
}


/**
 * @brief MedeaMainWindow::popupSearch
 */
void MedeaMainWindow::popupSearch()
{
    emit requestSuggestions();
    moveWidget(searchPopup);
    searchPopup->show();
    searchBar->setFocus();
}


/**
 * @brief MedeaMainWindow::toolbarChanged
 * @param area
 */
void MedeaMainWindow::toolbarChanged(Qt::DockWidgetArea area)
{
    if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {
        applicationToolbar->setOrientation(Qt::Horizontal);
        applicationToolbar->setFixedHeight(QWIDGETSIZE_MAX);
        applicationToolbar->setFixedWidth(QWIDGETSIZE_MAX);
    } else {
        applicationToolbar->setOrientation(Qt::Vertical);
        resizeEvent(0);
    }
}


/**
 * @brief MedeaMainWindow::toolbarTopLevelChanged
 * @param undocked
 */
void MedeaMainWindow::toolbarTopLevelChanged(bool undocked)
{
    if (undocked) {
        if (applicationToolbar->orientation() == Qt::Vertical) {
            applicationToolbar->setOrientation(Qt::Horizontal);
            applicationToolbar->setFixedHeight(QWIDGETSIZE_MAX);
        }
        //applicationToolbar->parentWidget()->resize(applicationToolbar->sizeHint() +  QSize(12,0));
        QWidget* topWidget =  applicationToolbar->parentWidget()->parentWidget();
        if (topWidget) {
            topWidget->resize(applicationToolbar->sizeHint() + QSize(15,0));
            topWidget->updateGeometry();
        }
    }
}


/**
 * @brief MedeaMainWindow::setModelTitle
 * @param modelTitle
 */
void MedeaMainWindow::setModelTitle(QString modelTitle)
{
    if(!modelTitle.isEmpty()){
        modelTitle = "- " % modelTitle;
    }
    QString title = "MEDEA " % modelTitle % "[*]";
    setWindowTitle(title);

    searchDialog->searchResults("ment", viewController->getSearchResults("ment"));
}


/**
 * @brief MedeaMainWindow::settingChanged
 * @param setting
 * @param value
 */
void MedeaMainWindow::settingChanged(SETTING_KEY setting, QVariant value)
{
    /*
    bool boolValue = value.toBool();
    //Handle stuff.
    switch(setting){
case SK_WINDOW_INTERFACES_VISIBLE:{
        nodeView_Interfaces->parentWidget()->setVisible(boolValue);
        break;
    }
case SK_WINDOW_BEHAVIOUR_VISIBLE:{
        nodeView_Behaviour->parentWidget()->setVisible(boolValue);
        break;
    }
case SK_WINDOW_ASSEMBLIES_VISIBLE:{
        nodeView_Assemblies->parentWidget()->setVisible(boolValue);
        break;
    }
case SK_WINDOW_HARDWARE_VISIBLE:{
        nodeView_Hardware->parentWidget()->setVisible(boolValue);
        break;
    }
case SK_WINDOW_TABLE_VISIBLE:
case SK_WINDOW_MINIMAP_VISIBLE:
case SK_WINDOW_BROWSER_VISIBLE:
case SK_WINDOW_TOOLBAR_VISIBLE:
    default:
        break;
}*/

}


/**
 * @brief MedeaMainWindow::initializeApplication
 */
void MedeaMainWindow::initializeApplication()
{
    //Allow Drops
    setAcceptDrops(true);

    //Set QApplication information.
    QApplication::setApplicationName("MEDEA");
    QApplication::setApplicationVersion(APP_VERSION);
    QApplication::setOrganizationName("CDIT-MA");
    QApplication::setOrganizationDomain("https://github.com/cdit-ma/");
    QApplication::setWindowIcon(Theme::theme()->getIcon("Actions", "MEDEA"));

    //Set Font.
    //int opensans_FontID = QFontDatabase::addApplicationFont(":/Resources/Fonts/OpenSans-Regular.ttf");
    //QString opensans_fontname = QFontDatabase::applicationFontFamilies(opensans_FontID).at(0);
    //QFont font = QFont(opensans_fontname);

    QFont font("Verdana");
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPointSizeF(8.5);
    QApplication::setFont(font);
}


/**
 * @brief MedeaMainWindow::connectNodeView
 * @param nodeView
 */
void MedeaMainWindow::connectNodeView(NodeViewNew *nodeView)
{
    if(nodeView && viewController){
        nodeView->setViewController(viewController);
    }
}


/**
 * @brief MedeaMainWindow::toggleWelcomeScreen
 * @param on
 */
void MedeaMainWindow::toggleWelcomeScreen(bool on)
{
    if (welcomeScreenOn == on) {
        return;
    }

    // show/hide the menu bar and close all dock widgets
    menuBar->setVisible(!on);
    setDockWidgetsVisible(!on);

    if (on) {
        holderLayout->removeWidget(welcomeScreen);
        holderLayout->addWidget(innerWindow);
        setCentralWidget(welcomeScreen);
    } else {
        holderLayout->removeWidget(innerWindow);
        holderLayout->addWidget(welcomeScreen);
        setCentralWidget(innerWindow);
    }

    welcomeScreenOn = on;
}


/**
 * @brief MedeaMainWindow::saveSettings
 */
void MedeaMainWindow::saveSettings()
{
    SettingsController* s = SettingsController::settings();
    if(s && s->getSetting(SK_GENERAL_SAVE_WINDOW_ON_EXIT).toBool()){
        s->setSetting(SK_GENERAL_MAXIMIZED, isMaximized());
        if(!isMaximized()){
            s->setSetting(SK_GENERAL_WIDTH, width());
            s->setSetting(SK_GENERAL_HEIGHT, height());
        }
    }
}


/**
 * @brief MedeaMainWindow::setupTools
 */
void MedeaMainWindow::setupTools()
{
    qint64 t1 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupWelcomeScreen();
    qint64 t2 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupMenuBar();
    qint64 t3 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupSearchBar();
    qint64 t4 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupProgressBar();
    qint64 t5 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupNotificationBar();
    qint64 t6 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupDock();
    qint64 t7 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupToolBar();
    qint64 t8 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupDataTable();
    qint64 t9 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupWindowManager();
    qint64 t10 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupMinimap();
    qint64 t11 = QDateTime::currentDateTime().toMSecsSinceEpoch();

    qCritical() << "setupWelcomeScreen in: " <<  t2 - t1 << "MS";
    qCritical() << "setupMenuBar in: " <<  t3 - t2 << "MS";
    qCritical() << "setupSearchBar in: " <<  t4 - t3 << "MS";
    qCritical() << "setupProgressBar in: " <<  t5 - t4 << "MS";
    qCritical() << "setupNotificationBar in: " <<  t6 - t5 << "MS";
    qCritical() << "setupDock in: " <<  t7 - t6 << "MS";
    qCritical() << "setupToolBar in: " <<  t8 - t7 << "MS";
    qCritical() << "setupDataTable in: " <<  t9 - t8 << "MS";
    qCritical() << "setupWindowManager in: " <<  t10 - t9 << "MS";
    qCritical() << "setupMinimap in: " <<  t11 - t10 << "MS";
}


/**
 * @brief MedeaMainWindow::setupInnerWindow
 */
void MedeaMainWindow::setupInnerWindow()
{
    innerWindow = MedeaWindowManager::constructCentralWindow("Main Window");
    setCentralWidget(innerWindow);

    NodeViewNew* nodeView_Interfaces = new NodeViewNew();
    NodeViewNew* nodeView_Behaviour = new NodeViewNew();
    NodeViewNew* nodeView_Assemblies = new NodeViewNew();
    NodeViewNew* nodeView_Hardware = new NodeViewNew();
    QOSBrowser* qosBrowser = new QOSBrowser(viewController, this);

    nodeView_Interfaces->setContainedViewAspect(VA_INTERFACES);
    nodeView_Behaviour->setContainedViewAspect(VA_BEHAVIOUR);
    nodeView_Assemblies->setContainedViewAspect(VA_ASSEMBLIES);
    nodeView_Hardware->setContainedViewAspect(VA_HARDWARE);

    MedeaDockWidget *dwInterfaces = MedeaWindowManager::constructNodeViewDockWidget("Interface", Qt::TopDockWidgetArea);
    dwInterfaces->setWidget(nodeView_Interfaces);
    dwInterfaces->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwInterfaces->setIcon("Items", "InterfaceDefinitions");
    dwInterfaces->setIconVisible(false);

    MedeaDockWidget *dwBehaviour = MedeaWindowManager::constructNodeViewDockWidget("Behaviour", Qt::TopDockWidgetArea);
    dwBehaviour->setWidget(nodeView_Behaviour);
    dwBehaviour->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwBehaviour->setIcon("Items", "BehaviourDefinitions");
    dwBehaviour->setIconVisible(false);

    MedeaDockWidget *dwAssemblies = MedeaWindowManager::constructNodeViewDockWidget("Assemblies", Qt::BottomDockWidgetArea);
    dwAssemblies->setWidget(nodeView_Assemblies);
    dwAssemblies->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwAssemblies->setIcon("Items", "AssemblyDefinitions");
    dwAssemblies->setIconVisible(false);

    MedeaDockWidget *dwHardware = MedeaWindowManager::constructNodeViewDockWidget("Hardware", Qt::BottomDockWidgetArea);
    dwHardware->setWidget(nodeView_Hardware);
    dwHardware->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwHardware->setIcon("Items", "HardwareDefinitions");
    dwHardware->setIconVisible(false);

    MedeaDockWidget *qosDockWidget = MedeaWindowManager::constructViewDockWidget("QOS Browser");
    qosDockWidget->setWidget(qosBrowser);
    qosDockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    qosDockWidget->setIcon("Items", "QOSProfile");

    dwInterfaces->setProtected(true);
    dwBehaviour->setProtected(true);
    dwAssemblies->setProtected(true);
    dwHardware->setProtected(true);
    qosDockWidget->setProtected(true);

    SettingsController* s = SettingsController::settings();

    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dwInterfaces);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dwBehaviour);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dwAssemblies);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dwHardware);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, qosDockWidget);

    innerWindow->setDockWidgetVisibility(dwInterfaces,   s->getSetting(SK_WINDOW_INTERFACES_VISIBLE).toBool());
    innerWindow->setDockWidgetVisibility(dwBehaviour,    s->getSetting(SK_WINDOW_BEHAVIOUR_VISIBLE).toBool());
    innerWindow->setDockWidgetVisibility(dwAssemblies,   s->getSetting(SK_WINDOW_ASSEMBLIES_VISIBLE).toBool());
    innerWindow->setDockWidgetVisibility(dwHardware,     s->getSetting(SK_WINDOW_HARDWARE_VISIBLE).toBool());
    innerWindow->setDockWidgetVisibility(qosDockWidget,  s->getSetting(SK_WINDOW_QOS_VISIBLE).toBool());

    // NOTE: Apparently calling innerWindow's createPopupMenu crashes the
    // application if it's called before the dock widgets are added above
    // This function needs to be called after the code above and before the connections below
    setupMainDockWidgetToggles();

    connectNodeView(nodeView_Interfaces);
    connectNodeView(nodeView_Behaviour);
    connectNodeView(nodeView_Assemblies);
    connectNodeView(nodeView_Hardware);

    // connect aspect toggle buttons
    connect(dwInterfaces, SIGNAL(visibilityChanged(bool)), interfaceButton, SLOT(setChecked(bool)));
    connect(dwBehaviour, SIGNAL(visibilityChanged(bool)), behaviourButton, SLOT(setChecked(bool)));
    connect(dwAssemblies, SIGNAL(visibilityChanged(bool)), assemblyButton, SLOT(setChecked(bool)));
    connect(dwHardware, SIGNAL(visibilityChanged(bool)), hardwareButton, SLOT(setChecked(bool)));
    connect(interfaceButton, SIGNAL(clicked(bool)), dwInterfaces, SLOT(setVisible(bool)));
    connect(behaviourButton, SIGNAL(clicked(bool)), dwBehaviour, SLOT(setVisible(bool)));
    connect(assemblyButton, SIGNAL(clicked(bool)), dwAssemblies, SLOT(setVisible(bool)));
    connect(hardwareButton, SIGNAL(clicked(bool)), dwHardware, SLOT(setVisible(bool)));
    connect(restoreAspectsButton, SIGNAL(clicked(bool)), innerWindow, SLOT(resetDockWidgets()));
}


/**
 * @brief MedeaMainWindow::setupWelcomeScreen
 */
void MedeaMainWindow::setupWelcomeScreen()
{
    welcomeScreen = new WelcomeScreenWidget(viewController->getActionController(), this);
    welcomeScreenOn = false;

    QWidget* holderWidget = new QWidget(this);
    holderWidget->hide();

    holderLayout = new QVBoxLayout(holderWidget);
    holderLayout->addWidget(welcomeScreen);

    connect(this, &MedeaMainWindow::recentProjectsUpdated, welcomeScreen, &WelcomeScreenWidget::recentProjectsUpdated);
}


/**
 * @brief MedeaMainWindow::setupMenuBar
 */
void MedeaMainWindow::setupMenuBar()
{
    menuBar = new QMenuBar(this);
    menuBar->addMenu(viewController->getActionController()->menu_file);
    menuBar->addMenu(viewController->getActionController()->menu_edit);
    menuBar->addMenu(viewController->getActionController()->menu_view);
    menuBar->addMenu(viewController->getActionController()->menu_model);
    menuBar->addMenu(viewController->getActionController()->menu_jenkins);
    menuBar->addMenu(viewController->getActionController()->menu_window);
    menuBar->addMenu(viewController->getActionController()->menu_options);
    menuBar->addMenu(viewController->getActionController()->menu_help);

    // TODO - Find out how to set the height of the menubar items
    menuBar->setFixedHeight(TOOLBAR_HEIGHT);
    menuBar->setNativeMenuBar(false);
    setMenuBar(menuBar);
}


/**
 * @brief MedeaMainWindow::setupToolBar
 */
void MedeaMainWindow::setupToolBar()
{
    applicationToolbar = new QToolBar(this);
    applicationToolbar->setMovable(false);
    applicationToolbar->setFloatable(false);
    applicationToolbar->setIconSize(QSize(20,20));

    QWidget* w1 = new QWidget(this);
    QWidget* w2 = new QWidget(this);
    w1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    w2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QFrame* frame = new QFrame(this);
    QHBoxLayout* layout = new QHBoxLayout(frame);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(w1);
    layout->addWidget(applicationToolbar);
    layout->addWidget(w2);

    //applicationToolbar->addWidget(w1);
    applicationToolbar->addActions(viewController->getActionController()->applicationToolbar->actions());
    //applicationToolbar->addWidget(w2);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Toolbar");
    //dockWidget->setTitleBarWidget(applicationToolbar);
    dockWidget->setTitleBarWidget(frame);
    dockWidget->setAllowedAreas(Qt::TopDockWidgetArea);
    //dockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::LeftDockWidgetArea | Qt::BottomDockWidgetArea);

    connect(dockWidget, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(toolbarChanged(Qt::DockWidgetArea)));
    connect(dockWidget, SIGNAL(topLevelChanged(bool)), this, SLOT(toolbarTopLevelChanged(bool)));

    //Check visibility state.
    dockWidget->setVisible(SettingsController::settings()->getSetting(SK_WINDOW_TOOLBAR_VISIBLE).toBool());
    addDockWidget(Qt::TopDockWidgetArea, dockWidget, Qt::Horizontal);
}


/**
 * @brief MedeaMainWindow::setupSearchBar
 */
void MedeaMainWindow::setupSearchBar()
{
    searchButton = new QToolButton(this);
    searchButton->setToolTip("Submit Search");

    searchCompleterModel = new QStringListModel(this);

    searchCompleter = new QCompleter(this);
    searchCompleter->setModel(searchCompleterModel);
    searchCompleter->setFilterMode(Qt::MatchContains);
    searchCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    searchCompleter->popup()->setItemDelegate(new QStyledItemDelegate(this));
    searchCompleter->popup()->setFont(QFont(font().family(), 10));

    searchBar = new QLineEdit(this);
    searchBar->setFont(QFont(font().family(), 13));
    searchBar->setPlaceholderText("Search Here...");
    searchBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    searchBar->setCompleter(searchCompleter);

    searchToolbar = new QToolBar(this);
    searchToolbar->setIconSize(QSize(24,24));
    searchToolbar->setMovable(false);
    searchToolbar->setFloatable(false);
    searchToolbar->addWidget(searchBar);
    searchToolbar->addWidget(searchButton);

    searchPopup = new PopupWidget(PopupWidget::POPUP, this);
    searchPopup->setWidget(searchToolbar);
    searchPopup->setWidth(300);

    searchDialog = new SearchDialog(this);

    connect(this, &MedeaMainWindow::requestSuggestions, viewController, &ViewController::requestSearchSuggestions);
    connect(viewController, &ViewController::vc_gotSearchSuggestions, this, &MedeaMainWindow::updateSearchSuggestions);
    connect(viewController, &ViewController::mc_modelReady, searchDialog, &SearchDialog::resetDialog);
    connect(searchBar, SIGNAL(returnPressed()), searchButton, SLOT(click()));
    connect(searchButton, SIGNAL(clicked(bool)), searchPopup, SLOT(hide()));
    connect(searchButton, SIGNAL(clicked(bool)), this, SLOT(searchEntered()));
    connect(searchDialog, SIGNAL(centerOnViewItem(int)), viewController, SLOT(centerOnID(int)));
    connect(searchDialog, SIGNAL(popupViewItem(int)), viewController, SLOT(popupItem(int)));
    connect(searchDialog, SIGNAL(itemHoverEnter(int)), viewController->getToolbarController(), SLOT(actionHoverEnter(int)));
    connect(searchDialog, SIGNAL(itemHoverLeave(int)), viewController->getToolbarController(), SLOT(actionHoverLeave(int)));
}


/**
 * @brief MedeaMainWindow::setupProgressBar
 */
void MedeaMainWindow::setupProgressBar()
{
    progressLabel = new QLabel("", this);
    progressLabel->setFont(QFont(font().family(), 11));
    progressLabel->setFixedHeight(progressLabel->sizeHint().height());
    progressLabel->setAlignment(Qt::AlignCenter);
    progressLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0,100);
    progressBar->setTextVisible(false);

    QWidget* widget = new QWidget(this);
    widget->setStyleSheet("QWidget{ background: rgba(0,0,0,0); border: 0px; }");

    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setSpacing(5);
    layout->setMargin(2);
    layout->addWidget(progressLabel);
    layout->addWidget(progressBar);

    progressPopup = new PopupWidget(PopupWidget::DIALOG, this);
    progressPopup->setWidget(widget);
    progressPopup->setWidth(widget->sizeHint().width() + 200);
    progressPopup->setHeight(progressLabel->sizeHint().height() + 30);
    progressPopup->hide();

    connect(viewController, &ViewController::mc_showProgress, this, &MedeaMainWindow::showProgressBar);
    connect(viewController, &ViewController::mc_progressChanged, this, &MedeaMainWindow::updateProgressBar);
}


/**
 * @brief MedeaMainWindow::setupNotificationBar
 */
void MedeaMainWindow::setupNotificationBar()
{
    notificationTimer = new QTimer(this);

    notificationIconLabel = new QLabel(this);
    notificationIconLabel->setAlignment(Qt::AlignCenter);

    notificationLabel = new QLabel("This is a notification.", this);
    notificationLabel->setFont(QFont(font().family(), 11));
    notificationLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    //notificationLabel->setWordWrap(true);

    notificationWidget = new QWidget(this);
    notificationWidget->setContentsMargins(5, 2, 5, 2);

    QHBoxLayout* layout = new QHBoxLayout(notificationWidget);
    layout->setMargin(0);
    layout->setSpacing(5);
    layout->addWidget(notificationIconLabel, 0, Qt::AlignCenter);
    layout->addWidget(notificationLabel, 1, Qt::AlignCenter);

    notificationPopup = new PopupWidget(PopupWidget::TOOL, this);
    notificationPopup->setWidget(notificationWidget);
    notificationPopup->hide();

    notificationDialog = new NotificationDialog(this);

    connect(notificationDialog, &NotificationDialog::notificationAdded, viewController, &ViewController::notificationAdded);
    connect(viewController, &ViewController::mc_modelReady, viewController, &ViewController::notificationsSeen);
    connect(viewController, &ViewController::mc_modelReady, notificationDialog, &NotificationDialog::resetDialog);
    connect(viewController->getActionController()->window_showNotifications, &QAction::triggered, notificationDialog, &NotificationDialog::toggleVisibility);
    connect(notificationTimer, &QTimer::timeout, notificationPopup, &QDialog::hide);
}


/**
 * @brief MedeaMainWindow::setupDock
 */
void MedeaMainWindow::setupDock()
{
    dockTabWidget = new DockTabWidget(viewController, this);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Dock");
    dockWidget->setWidget(dockTabWidget);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea);

    connect(viewController->getActionController()->toggleDock, SIGNAL(triggered(bool)), dockWidget, SLOT(setVisible(bool)));

    //Check visibility state.
    //dockWidget->setVisible(SettingsController::settings()->getSetting(SK_WINDOW_TABLE_VISIBLE).toBool());
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget, Qt::Vertical);
}


/**
 * @brief MedeaMainWindow::setupDataTable
 */
void MedeaMainWindow::setupDataTable()
{
    tableWidget = new TableWidget(viewController, this);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Table");
    dockWidget->setWidget(tableWidget);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    QAction* modelAction = viewController->getActionController()->model_selectModel;
    modelAction->setToolTip("Show Model's Table");
    dockWidget->getTitleBar()->addToolAction(modelAction, Qt::AlignLeft);

    //Check visibility state.
    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
    setDockWidgetVisibility(dockWidget, SettingsController::settings()->getSetting(SK_WINDOW_TABLE_VISIBLE).toBool());
}


/**
 * @brief MedeaMainWindow::setupMinimap
 */
void MedeaMainWindow::setupMinimap()
{
    minimap = new NodeViewMinimap(this);
    minimap->setMinimumHeight(150);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Minimap");
    dockWidget->setWidget(minimap);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
    setDockWidgetVisibility(dockWidget, SettingsController::settings()->getSetting(SK_WINDOW_MINIMAP_VISIBLE).toBool());
}


/**
 * @brief MedeaMainWindow::setupWindowManager
 */
void MedeaMainWindow::setupWindowManager()
{
    viewManager = MedeaWindowManager::manager()->getViewManagerGUI();
    viewManager->setMinimumHeight(210);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("View Manager");
    dockWidget->setWidget(viewManager);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
    setDockWidgetVisibility(dockWidget, SettingsController::settings()->getSetting(SK_WINDOW_BROWSER_VISIBLE).toBool());
}


/**
 * @brief MedeaMainWindow::setupMainDockWidgetToggles
 * NOTE: This neeeds to be called after the tool dock widgets
 * and both the central and inner windows are constructed.
 */
void MedeaMainWindow::setupMainDockWidgetToggles()
{
    interfaceButton = new QToolButton(this);
    behaviourButton = new QToolButton(this);
    assemblyButton = new QToolButton(this);
    hardwareButton = new QToolButton(this);
    restoreAspectsButton = new QToolButton(this);
    restoreToolsButton = new QToolButton(this);

    /*
    interfaceButton->setText("I");
    behaviourButton->setText("B");
    assemblyButton->setText("A");
    hardwareButton->setText("H");
    */

    interfaceButton->setToolTip("Toggle Interface Aspect");
    behaviourButton->setToolTip("Toggle Behaviour Aspect");
    assemblyButton->setToolTip("Toggle Assembly Aspect");
    hardwareButton->setToolTip("Toggle Hardware Aspect");
    restoreAspectsButton->setToolTip("Restore Main Dock Widgets");
    restoreToolsButton->setToolTip("Restore Tool Dock Widgets");

    restoreAspectsButton->hide();

    interfaceButton->setCheckable(true);
    behaviourButton->setCheckable(true);
    assemblyButton->setCheckable(true);
    hardwareButton->setCheckable(true);

    QMenu* menu = createPopupMenu();
    restoreToolsAction = menu->addAction("Show All Tool Widgets");
    restoreToolsButton->setMenu(menu);
    restoreToolsButton->setPopupMode(QToolButton::InstantPopup);

    if (innerWindow) {
        restoreAspectsButton->setMenu(innerWindow->createPopupMenu());
        restoreAspectsButton->setPopupMode(QToolButton::InstantPopup);
    }

    QToolBar* toolbar = new QToolBar(this);
    toolbar->setIconSize(QSize(20,20));
    toolbar->setFixedHeight(menuBar->height() - 6);
    toolbar->setStyleSheet("QToolButton{ padding: 2px 4px; }");

    toolbar->addAction(viewController->getActionController()->window_showNotifications);
    toolbar->addSeparator();
    toolbar->addWidget(interfaceButton);
    toolbar->addWidget(behaviourButton);
    toolbar->addWidget(assemblyButton);
    toolbar->addWidget(hardwareButton);
    toolbar->addSeparator();
    toolbar->addWidget(restoreAspectsButton);
    toolbar->addWidget(restoreToolsButton);

    menuBar->setCornerWidget(toolbar);

    connect(restoreToolsAction, SIGNAL(triggered(bool)), this, SLOT(resetToolDockWidgets()));
}


/**
 * @brief MedeaMainWindow::setupJenkinsManager
 */
void MedeaMainWindow::setupJenkinsManager()
{
    if(!jenkinsManager){
        jenkinsManager = new JenkinsManager(this);
        connect(jenkinsManager, &JenkinsManager::settingsValidationComplete, viewController, &ViewController::jenkinsManager_SettingsValidated);

        connect(viewController->getActionController()->jenkins_importNodes, &QAction::triggered, jenkinsManager, &JenkinsManager::getJenkinsNodes);

        connect(jenkinsManager, &JenkinsManager::gotJenkinsNodeGraphml, viewController, &ViewController::jenkinsManager_GotJenkinsNodesList);
        connect(jenkinsManager, &JenkinsManager::jenkinsReady, viewController, &ViewController::vc_JenkinsReady);

        connect(viewController, &ViewController::vc_executeJenkinsJob, jenkinsManager, &JenkinsManager::executeJenkinsJob);

        jenkinsManager->validateSettings();
    }
}


/**
 * @brief MedeaMainWindow::setupCUTSManager
 */
void MedeaMainWindow::setupCUTSManager()
{
    if(!cutsManager ){
        cutsManager  = new CUTSManager();
        xmiImporter = new XMIImporter(cutsManager, this);

        //connect(cutsManager, &CUTSManager::localDeploymentOkay, viewController, &ViewController::cutsManager_DeploymentOkay);
        connect(viewController, &ViewController::vc_getCodeForComponent, cutsManager, &CUTSManager::getCPPForComponent);
        connect(viewController, &ViewController::vc_importXMEProject, cutsManager, &CUTSManager::executeXMETransform);

        connect(viewController, &ViewController::vc_validateModel, cutsManager, &CUTSManager::executeXSLValidation);
        connect(viewController, &ViewController::vc_launchLocalDeployment, cutsManager, &CUTSManager::showLocalDeploymentGUI, Qt::DirectConnection);

        connect(cutsManager, &CUTSManager::gotCodeForComponent, viewController, &ViewController::showCodeViewer);
        connect(cutsManager, &CUTSManager::gotXMETransform, viewController, &ViewController::importGraphMLFile);
        connect(cutsManager, &CUTSManager::executedXSLValidation, viewController, &ViewController::modelValidated);

        connect(viewController, &ViewController::vc_importXMIProject, xmiImporter, &XMIImporter::importXMI);
        connect(xmiImporter, &XMIImporter::loadingStatus, this, &MedeaMainWindow::showProgressBar);
        connect(xmiImporter, &XMIImporter::gotXMIGraphML, viewController, &ViewController::importGraphMLExtract);
    }
}


/**
 * @brief MedeaMainWindow::resizeToolWidgets
 */
void MedeaMainWindow::resizeToolWidgets()
{
    int windowHeight = height();
    int defaultWidth = 500;
    tableWidget->resize(defaultWidth, windowHeight/2);
    tableWidget->updateGeometry();
    minimap->parentWidget()->resize(defaultWidth, windowHeight/4);
    viewManager->parentWidget()->resize(defaultWidth, windowHeight/4);
}


/**
 * @brief MedeaMainWindow::moveWidget
 * @param widget
 * @param parentWidget
 * @param alignment
 */
void MedeaMainWindow::moveWidget(QWidget* widget, QWidget* parentWidget, Qt::Alignment alignment)
{
    QWidget* cw = parentWidget;
    QPointF widgetPos;
    if (cw == 0) {
        cw = QApplication::activeWindow();
        qDebug() << "active: " << cw;
        cw = MedeaWindowManager::manager()->getActiveWindow();
        //widgetPos = pos();
    }
    if (cw == this) {
        cw = centralWidget();
        widgetPos = pos();
    }
    if (cw && widget) {
        //QPointF widgetPos = cw->geometry().center();
        widgetPos += cw->geometry().center();
        switch (alignment) {
        case Qt::AlignBottom:
            //widgetPos.ry() += cw->height() / 2 - widget->height();
            widgetPos.ry() += cw->height() / 2;
            break;
        default:
            break;
        }
        widgetPos -= QPointF(widget->width()/2, widget->height()/2);
        widget->move(widgetPos.x(), widgetPos.y());
    }
}


/**
 * @brief MedeaMainWindow::resizeEvent
 * @param e
 */
void MedeaMainWindow::resizeEvent(QResizeEvent* e)
{
    if(e){
        QMainWindow::resizeEvent(e);
    }
    if(applicationToolbar && applicationToolbar->orientation() == Qt::Vertical){
        applicationToolbar->setFixedHeight(centralWidget()->rect().height());
    }
}


/**
 * @brief MedeaMainWindow::closeEvent
 * @param event
 */
void MedeaMainWindow::closeEvent(QCloseEvent *event)
{
    if (viewController) {
        viewController->closeMEDEA();
        event->ignore();
    }
}
