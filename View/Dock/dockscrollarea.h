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
    explicit DockScrollArea(QString label, NodeView *view, DockToggleButton *parent);

    void setNotAllowedKinds(QStringList kinds);

    void updateScrollBar();

    void addDockNodeItem(DockNodeItem* item, int insertIndex = -1, bool addToLayout = true);
    DockNodeItem* getDockNodeItem(NodeItem* item);
    DockNodeItem* getDockNodeItem(Node* node);
    QList<DockNodeItem*> getDockNodeItems();

    DockToggleButton* getParentButton();
    NodeItem* getCurrentNodeItem();
    QString getCurrentNodeID();

    QString getLabel();
    QVBoxLayout* getLayout();

    NodeView* getNodeView();
    QStringList getAdoptableNodeListFromView();

protected:
    void paintEvent(QPaintEvent *e);

public slots:
    virtual void dockNodeItemClicked() = 0;
    virtual void updateDock();

    void updateCurrentNodeItem();
    void nodeDeleted(QString nodeID, QString parentID);
    void removeDockNodeItemFromList(DockNodeItem* item);

    void activate();
    void clear();

    void parentHeightChanged(double height);

private:
    void setupLayout();
    void setParentButton(DockToggleButton* parent);

    NodeView* nodeView;
    QString currentNodeItemID;
    NodeItem* currentNodeItem;
    DockToggleButton *parentButton;

    QVBoxLayout* mainLayout;
    QVBoxLayout* layout;
    QString label;
    bool activated;

    QList<DockNodeItem*> dockNodeItems;
    QStringList notAllowedKinds;

};

#endif // DOCKSCROLLAREA_H
