#ifndef WINDOWITEM_H
#define WINDOWITEM_H
#include "../../Widgets/Windows/basewindow.h"
#include "../../Controllers/WindowManager/windowmanager.h"
#include "viewmanagerwidget.h"
#include <QToolBar>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>

class WindowItem : public QWidget{
    Q_OBJECT
public:
    WindowItem(ViewManagerWidget* manager, BaseWindow* window);
    ~WindowItem();
private slots:
    void themeChanged();
    void titleChanged(QString title="");

    void dockWidgetAdded(BaseDockWidget* widget);
private:

    void setupLayout();
    QLabel* label;
    QAction* closeAction;
    ViewManagerWidget* manager;
    BaseWindow* window;
    QWidget* dockContainer;
    QVBoxLayout* dockContainerLayout;
    QToolBar* windowToolbar;
};

#endif // WINDOWITEM_H
