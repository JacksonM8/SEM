#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>
#include <QSignalMapper>
#include <QAction>
#include <QSet>

//Forward Class Declarations
class BaseDockWidget;
class DefaultDockWidget;
class ViewDockWidget;
class ToolDockWidget;
class InvisibleDockWidget;
class DockReparenterPopup;


class BaseWindow;
class ViewController;
class ViewManagerWidget;
class ViewItem;

class WindowManager : public QObject
{
    friend class BaseWindow;
    friend class BaseDockWidget;
    Q_OBJECT
public:
    static bool Sort(const BaseDockWidget *a, const BaseDockWidget *b);
    //Public Static functions.
    static WindowManager* manager();
    static void teardown();

    static void MoveWidget(QWidget* widget, QWidget* parent_widget = 0, Qt::Alignment = Qt::AlignCenter);
    static bool isViewDockWidget(BaseDockWidget* base_dock_widget);

    BaseWindow* getMainWindow();
    BaseWindow* getCentralWindow();
    ViewManagerWidget* getViewManagerGUI();
    BaseWindow* getActiveWindow();
protected:
    WindowManager();
    ~WindowManager();
    
    DockReparenterPopup* getDockPopup();
    void destructWindow(BaseWindow* window);
public:
    void destructDockWidget(BaseDockWidget* widget);
public:
    //Factory constructor Functions
    BaseWindow* constructMainWindow(ViewController* vc);
    BaseWindow* constructSubWindow(QString title="");
    BaseWindow* constructCentralWindow(BaseWindow* parent_window, QString title="");
    BaseWindow* constructInvisibleWindow(BaseWindow* parent_window, QString title="");
    
    ViewDockWidget* constructViewDockWidget(QString title="", Qt::DockWidgetArea area = Qt::TopDockWidgetArea);
    DefaultDockWidget* constructDockWidget(QString title="", Qt::DockWidgetArea area = Qt::TopDockWidgetArea);
    ToolDockWidget* constructToolDockWidget(QString title="");
    InvisibleDockWidget* constructInvisibleDockWidget(QString title="");

    bool reparentDockWidget(BaseDockWidget* dockWidget);
    bool reparentDockWidget(BaseDockWidget* dockWidget, BaseWindow* window);
    void showDockWidget(BaseDockWidget* widget);
signals:
    void windowConstructed(BaseWindow* window);
    void windowDestructed(int ID);
    
    void dockWidgetConstructed(BaseDockWidget* widget);
    void dockWidgetDestructed(int ID);
    
    void activeViewDockWidgetChanged(ViewDockWidget* widget, ViewDockWidget* prevWidget = 0);
public:
    //Getters
    ViewDockWidget* getActiveViewDockWidget();

    void setActiveViewDockWidget(ViewDockWidget *view = 0);


    BaseWindow* getWindow(int ID);
    QList<BaseWindow*> getWindows();
    QList<BaseDockWidget*> getDockWidgets();
    QList<ViewDockWidget*> getViewDockWidgets();
    ViewDockWidget* getViewDockWidget(ViewItem* item);
private slots:
    void focusChanged(QWidget *old, QWidget *now);
    void dockWidget_Close(int ID);
    void dockWidget_PopOut(int ID);
    void activeDockWidgetVisibilityChanged();

private:
    void addWindow(BaseWindow *window);
    void removeWindow(BaseWindow *window);
    void addDockWidget(BaseDockWidget* dockWidget);
    void removeDockWidget(BaseDockWidget* dockWidget);
    
    BaseWindow* mainWindow = 0;
    BaseWindow* centralWindow = 0;
    ViewDockWidget* activeViewDockWidget = 0;
    
    ViewManagerWidget* viewManagerWidget = 0;

    DockReparenterPopup* dock_popup = 0;
    
    QHash<int, BaseWindow*> windows;
    QHash<int, BaseDockWidget*> dockWidgets;

    QSet<int> window_ids;
    QSet<int> view_dock_ids;
    Q_INVOKABLE void MoveWidgetEvent(QWidget* widget, QWidget* parent_widget, Qt::Alignment alignment);
    
    static WindowManager* managerSingleton;
};

#endif // WINDOWMANAGER_H
