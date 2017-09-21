#include "centralwindow.h"


CentralWindow::CentralWindow(BaseWindow* parent_window):ViewWindow()
{
    this->parent_window = parent_window;
    setAcceptDrops(true);
    setDockNestingEnabled(true);

    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::West);
}

CentralWindow::~CentralWindow()
{
}

QMenu* CentralWindow::createPopupMenu(){
    if(parent_window){
        return parent_window->createPopupMenu();
    }else{
        return new QMenu(this);
    }
}

