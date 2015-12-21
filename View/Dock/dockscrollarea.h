#ifndef DOCKSCROLLAREA_H
#define DOCKSCROLLAREA_H

#include <QScrollArea>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QLabel>

#include "../nodeview.h"
#include "../GraphicsItems/nodeitem.h"
#include "docknodeitem.h"

class DockToggleButton;
class DockNodeItem;
class NodeView;

class DockScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit DockScrollArea(QString label, NodeView* view, DockToggleButton* parent, QString dockEmptyText = "");

    void setNotAllowedKinds(QStringList kinds);
    QStringList getNotAllowedKinds();

    void addDockNodeItem(DockNodeItem* dockItem, int insertIndex = -1, bool addToLayout = true);
    void removeDockNodeItem(DockNodeItem* dockItem, bool deleteItem = false);

    DockNodeItem* getDockNodeItem(QString dockItemID);
    virtual QList<DockNodeItem*> getDockNodeItems();

    bool isDockOpen();
    void setDockOpen(bool open);

    bool isDockEnabled();
    void setDockEnabled(bool enabled);

    DockToggleButton* getParentButton();
    DOCK_TYPE getDockType();

    NodeItem* getCurrentNodeItem();
    QString getCurrentNodeKind();
    int getCurrentNodeID();

    QString getLabel();
    QVBoxLayout* getLayout();

    NodeView* getNodeView();
    QStringList getAdoptableNodeListFromView();

    void setInfoText(QString text);
    bool hasVisibleItems();

    virtual void connectToView() = 0;
    virtual void nodeDeleted(QString nodeID);

signals:
    void dock_opened(bool open = true);
    void dock_closed(bool open = false);
    void dock_forceOpenDock(DOCK_TYPE type, QString filterForKind = "");

public slots:
    virtual void dockNodeItemClicked() = 0;
    virtual void updateDock();
    virtual void clear();

    void onNodeDeleted(int nodeID, int parentID);
    void onEdgeDeleted(int srcID = -1, int dstID = -1);

    void clearSelected();
    void updateCurrentNodeItem();

    void parentHeightChanged(double height);

    void updateInfoLabel();

private:
    void setupLayout();
    void setParentButton(DockToggleButton* parent);

    NodeView* nodeView;
    DockToggleButton* parentButton;

    NodeItem* currentNodeItem;
    int currentNodeItemID;

    QVBoxLayout* mainLayout;
    QVBoxLayout* layout;
    
    QLabel* infoLabel;
    QString infoText;
    QString defaultInfoText;
    bool infoLabelVisible;

    QString label;
    bool dockOpen;

    QHash<QString, DockNodeItem*> dockNodeItems;
    QList<int> dockNodeIDs;
    QStringList notAllowedKinds;

};

#endif // DOCKSCROLLAREA_H
