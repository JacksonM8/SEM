#include "medeamainwindow.h"
#include "medeaviewdockwidget.h"
#include "medeatooldockwidget.h"
#include "../../View/theme.h"
#include "selectioncontroller.h"
#include <QDebug>
#include <QHeaderView>
MedeaMainWindow::MedeaMainWindow(QWidget* parent):MedeaWindowNew(parent, true)
{
    setDockNestingEnabled(false);
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::North);

    setMinimumSize(1000,600);
    showNormal();

    setupInnerWindow();
    setupTools();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(MedeaWindowManager::manager(), SIGNAL(activeViewDockWidgetChanged(MedeaViewDockWidget*)), this, SLOT(activeViewDockWidgetChanged(MedeaViewDockWidget*)));
}

void MedeaMainWindow::setViewController(ViewController *vc)
{
    viewController = vc;
    SelectionController* controller = vc->getSelectionController();
    connect(controller, SIGNAL(activeSelectedItemChanged(ViewItem*)), this, SLOT(activeSelectedItemChanged(ViewItem*)));
    nodeView_Interfaces->setViewController(vc);
    nodeView_Behaviour->setViewController(vc);
    nodeView_Assemblies->setViewController(vc);
    nodeView_Hardware->setViewController(vc);

    connect(viewController, SIGNAL(viewItemConstructed(ViewItem*)), nodeView_Interfaces, SLOT(viewItem_Constructed(ViewItem*)));
    connect(viewController, SIGNAL(viewItemConstructed(ViewItem*)), nodeView_Behaviour, SLOT(viewItem_Constructed(ViewItem*)));
    connect(viewController, SIGNAL(viewItemConstructed(ViewItem*)), nodeView_Assemblies, SLOT(viewItem_Constructed(ViewItem*)));
    connect(viewController, SIGNAL(viewItemConstructed(ViewItem*)), nodeView_Hardware, SLOT(viewItem_Constructed(ViewItem*)));

    connect(viewController, SIGNAL(viewItemDestructing(int,ViewItem*)), nodeView_Interfaces, SLOT(viewItem_Destructed(int,ViewItem*)));
    connect(viewController, SIGNAL(viewItemDestructing(int,ViewItem*)), nodeView_Behaviour, SLOT(viewItem_Destructed(int,ViewItem*)));
    connect(viewController, SIGNAL(viewItemDestructing(int,ViewItem*)), nodeView_Assemblies, SLOT(viewItem_Destructed(int,ViewItem*)));
    connect(viewController, SIGNAL(viewItemDestructing(int,ViewItem*)), nodeView_Hardware, SLOT(viewItem_Destructed(int,ViewItem*)));
}

void MedeaMainWindow::themeChanged()
{
    Theme* theme = Theme::theme();
    QString BGColor = theme->getBackgroundColorHex();
    QString altBGColor = theme->getAltBackgroundColorHex();
    QString textColor = theme->getTextColorHex(Theme::CR_NORMAL);
    QString highlightColor = theme->getHighlightColorHex();
    QString pressedColor = theme->getPressedColorHex();

    setStyleSheet("QMainWindow {"
                  "background: " + BGColor + ";"
                  "color: " + textColor + ";"
                  "}"
                  "QGraphicsView {"
                  "background: " + BGColor + ";"
                  "}"
                  "QToolBar {"
                  "margin:1px 0px;"
                  "background: " + altBGColor + ";"
                  "border:1px solid gray;"
                  "}"
                  "QToolButton {"
                  "padding:0px;"
                  "border: none;"
                  "background:" + altBGColor + ";"
                  "}"
                  "QToolButton:hover {"
                  "background:" + highlightColor + ";"
                  "}"
                  "QToolButton:pressed {"
                  "background:" + pressedColor + ";"
                  "}"
                  "QTableView::item {"
                  //"background: white;"
                  "padding: 0px 4px;"
                  "}"
                  //"QTableView{ background: " + BGColor + ";}"
                  );

    //if (tableView && tableView->parentWidget()) {
    //    tableView->parentWidget()->setStyleSheet("QDockWidget{ background: " + BGColor + ";}");
    //}
}

void MedeaMainWindow::activeViewDockWidgetChanged(MedeaViewDockWidget *viewDock)
{
    if(viewDock){
        NodeViewNew* view = viewDock->getNodeView();
        if(view){
            minimap->setScene(view->scene());
            minimap->disconnect();

            connect(minimap, SIGNAL(minimap_Pan(QPointF)), view, SLOT(minimap_Pan(QPointF)));
            connect(minimap, SIGNAL(minimap_Panning(bool)), view, SLOT(minimap_Panning(bool)));
            connect(minimap, SIGNAL(minimap_Zoom(int)), view, SLOT(minimap_Zoom(int)));
            connect(view, SIGNAL(viewportChanged(QRectF, qreal)), minimap, SLOT(viewportRectChanged(QRectF, qreal)));
            view->viewportChanged();
        }
    }
}

void MedeaMainWindow::activeSelectedItemChanged(ViewItem *item)
{
    QAbstractItemModel* model = 0;

    if(item){
        model = item->getTableModel();
    }
    if(tableView){
        bool update = !tableView->model();

        tableView->setModel(model);

        if(update && tableView->horizontalHeader()->count() == 2) {
            //Set the resize mode of the sections on first model launch.
            tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
            tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        }
    }
}

void MedeaMainWindow::setupTools()
{

    setupDataTable();
    setupMinimap();
}


void MedeaMainWindow::setupInnerWindow()
{
    innerWindow = MedeaWindowManager::constructSubWindow();
    setCentralWidget(innerWindow);

    nodeView_Interfaces = new NodeViewNew();
    nodeView_Behaviour = new NodeViewNew();
    nodeView_Assemblies = new NodeViewNew();
    nodeView_Hardware = new NodeViewNew();

    nodeView_Interfaces->setContainedViewAspect(VA_INTERFACES);
    nodeView_Behaviour->setContainedViewAspect(VA_BEHAVIOUR);
    nodeView_Assemblies->setContainedViewAspect(VA_ASSEMBLIES);
    nodeView_Hardware->setContainedViewAspect(VA_HARDWARE);


    MedeaDockWidget *dockWidget1 = MedeaWindowManager::constructViewDockWidget("Interface", Qt::TopDockWidgetArea);
    dockWidget1->setWidget(nodeView_Interfaces);
    dockWidget1->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dockWidget1->setParent(this);

    MedeaDockWidget *dockWidget2 = MedeaWindowManager::constructViewDockWidget("Behaviour", Qt::TopDockWidgetArea);
    dockWidget2->setWidget(nodeView_Behaviour);
    dockWidget2->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    MedeaDockWidget *dockWidget3 = MedeaWindowManager::constructViewDockWidget("Assemblies", Qt::BottomDockWidgetArea);
    dockWidget3->setWidget(nodeView_Assemblies);
    dockWidget3->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    MedeaDockWidget *dockWidget4 = MedeaWindowManager::constructViewDockWidget("Hardware", Qt::BottomDockWidgetArea);
    dockWidget4->setWidget(nodeView_Hardware);
    dockWidget4->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    dockWidget1->setProtected(true);
    dockWidget2->setProtected(true);
    dockWidget3->setProtected(true);
    dockWidget4->setProtected(true);

    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dockWidget1);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dockWidget2);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dockWidget3);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dockWidget4);
}

void MedeaMainWindow::setupDataTable()
{
    tableView = new QTableView(this);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Table");
    dockWidget->setWidget(tableView);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
}

void MedeaMainWindow::setupMinimap()
{
    minimap = new NodeViewMinimap(this);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Minimap");
    dockWidget->setWidget(minimap);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
}























