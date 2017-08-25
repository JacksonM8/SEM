#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QProgressBar>
#include <QCompleter>
#include <QStringListModel>
#include <QTimer>

#include "basewindow.h"
#include "welcomescreenwidget.h"

#include "../../Views/Dock/docktabwidget.h"
#include "../../Views/Search/searchdialog.h"
#include "../../Views/Table/datatablewidget.h"
#include "../../Views/QOSBrowser/qosbrowser.h"

#include "../../Views/NodeView/nodeview.h"
#include "../../Views/NodeView/nodeviewminimap.h"

#include "../../Controllers/ViewController/viewcontroller.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"

#include "../../Controllers/JenkinsManager/jenkinsmanager.h"

#include "../../Widgets/Dialogs/popupwidget.h"
#include "../../Views/Notification/notificationtoolbar.h"
#include "../../Views/Notification/notificationdialog.h"



class MainWindow : public BaseWindow
{
    friend class WindowManager;
    Q_OBJECT
protected:
    MainWindow(ViewController* vc, QWidget *parent=0);
    ~MainWindow();

public:
signals:
    void requestSuggestions();
    void preloadImages();
    void jenkins_validated(bool);
    void welcomeScreenToggled(bool);
public slots:
    void setModelTitle(QString modelTitle="");

    void resetToolDockWidgets();
private slots:

    void themeChanged();
    void activeViewDockWidgetChanged(ViewDockWidget* widget, ViewDockWidget* prevDock);

    void toolbarOrientationChanged(Qt::Orientation orientation);
private:
    void showSearchDialog();
    void showNotificationDialog();
    
    void swapCentralWidget(QWidget* widget);
    void setViewController(ViewController* vc);
    void initializeApplication();
    void toggleWelcomeScreen(bool on);
    
    void saveWindowState();
    void restoreWindowState(bool restore_geo = true);

    void setupTools();
    void setupInnerWindow();
    void setupWelcomeScreen();
    void setupMenuBar();
    void setupToolBar();
    void setupProgressBar();
    void setupDock();
    void setupDataTable();
    void setupViewManager();
    void setupMinimap();
    void setupMenuCornerWidget();
    void setupDockablePanels();

    void setupJenkinsManager();
    void setupXMIImporter();

    void resizeToolWidgets();
    void moveWidget(QWidget* widget, QWidget* parentWidget = 0, Qt::Alignment alignment = Qt::AlignCenter);

    ViewController* viewController;

    BaseWindow* innerWindow = 0;
    BaseDockWidget* dockwidget_Jenkins = 0;
    BaseDockWidget* dockwidget_Search = 0;
    BaseDockWidget* dockwidget_Notification = 0;

    BaseDockWidget* dockwidget_Table = 0;
    BaseDockWidget* dockwidget_ViewManager = 0;
    BaseDockWidget* dockwidget_Minimap = 0;
    BaseDockWidget* dockwidget_Dock = 0;
    BaseDockWidget* dockwidget_InnerWindow = 0;

    QMenuBar* menuBar = 0;
    QWidget* applicationToolbar_spacer1 = 0;
    QWidget* applicationToolbar_spacer2 = 0;
    QToolBar* applicationToolbar = 0;



    DockTabWidget* dockTabWidget;
    DataTableWidget* tableWidget;
    NodeViewMinimap* minimap;
    ViewManagerWidget* viewManager;

    NotificationToolbar* notificationToolbar = 0;

    QToolButton* restoreToolsButton;
    QAction* restoreToolsAction;

    WelcomeScreenWidget* welcomeScreen = 0;
    bool welcomeScreenOn = false;

protected:
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *event);

};

#endif // MAINWINDOW_H
