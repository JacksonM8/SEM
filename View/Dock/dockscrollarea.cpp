#include "dockscrollarea.h"
#include "docktogglebutton.h"
#include "docknodeitem.h"
#include <QDebug>


/**
 * @brief DockScrollArea::DockScrollArea
 * @param label
 * @param view
 * @param parent
 */
DockScrollArea::DockScrollArea(QString label, NodeView* view, DockToggleButton *parent) :
    QScrollArea(parent)
{
    nodeView = view;
    currentNodeItem = 0;

    this->label = label;
    activated = false;

    setParentButton(parent);
    setupLayout();
}


/**
 * @brief DockScrollArea::setNotAllowedKinds
 * @param kinds
 */
void DockScrollArea::setNotAllowedKinds(QStringList kinds)
{
    notAllowedKinds = kinds;
}


/**
 * @brief DockScrollArea::updateCurrentNodeItem
 * This sets nodeItem to be the currently selected node item.
 */
void DockScrollArea::updateCurrentNodeItem()
{
    currentNodeItem = nodeView->getSelectedNodeItem();

    if (currentNodeItem) {
        currentNodeItemID = currentNodeItem->getGraphML()->getID();
    } else {
        currentNodeItemID = "";
    }

    updateDock();
}


/**
 * @brief DockScrollArea::getCurrentNodeItem
 * Returns the current node item.
 * @return
 */
NodeItem *DockScrollArea::getCurrentNodeItem()
{
    return currentNodeItem;
}


/**
 * @brief DockScrollArea::getParentButton
 * Returns this dock's parent button (DockToggleButton).
 * @return
 */
DockToggleButton *DockScrollArea::getParentButton()
{
    return parentButton;
}


/**
 * @brief DockScrollArea::getLabel
 * Returns this dock's label.
 * @return
 */
QString DockScrollArea::getLabel()
{
    return label;
}


/**
 * @brief DockScrollArea::getNodeView
 * Returns the NodeView this dock is attached to.
 * @return
 */
NodeView *DockScrollArea::getNodeView()
{
    return nodeView;
}


/**
 * @brief DockScrollArea::getAdoptableNodeListFromView
 * Returns the list of adoptable nodes for the currently
 * selected item from this dock's node view.
 * @return
 */
QStringList DockScrollArea::getAdoptableNodeListFromView()
{
    return nodeView->getAdoptableNodeList(nodeView->getSelectedNode());
}


/**
 * @brief DockScrollArea::getLayout
 * @return
 */
QVBoxLayout *DockScrollArea::getLayout()
{
    return layout;
}


/**
 * @brief DockScrollArea::addDockNodeItem
 * This adds a new dock item to this dock's list and layout.
 * It also connects the dock item's basic signals to this dock.
 * @param item
 * @param insertIndex
 * @param addToLayout
 */
void DockScrollArea::addDockNodeItem(DockNodeItem *item, int insertIndex, bool addToLayout)
{
    dockNodeItems.append(item);

    if (addToLayout) {
        if (insertIndex == -1) {
            layout->addWidget(item);
        } else {
            layout->insertWidget(insertIndex, item);
        }
        //updateScrollBar();
    }

    connect(item, SIGNAL(dockItem_clicked()), this, SLOT(dockNodeItemClicked()));
    connect(item, SIGNAL(dockItem_removeFromDock(DockNodeItem*)), this, SLOT(removeDockNodeItemFromList(DockNodeItem*)));
}


/**
 * @brief DockScrollArea::getDockNodeItem
 * This checks if this dock already contains a dock node item attached to item.
 * @param item
 * @return
 */
DockNodeItem *DockScrollArea::getDockNodeItem(NodeItem *item)
{
    if (item) {
        foreach (DockNodeItem* dockItem, dockNodeItems) {
            if (dockItem->getNodeItem() == item) {
                return dockItem;
            }
        }
    }
    return 0;
}


/**
 * @brief DockScrollArea::getDockNodeItem
 * @param node
 * @return
 */
DockNodeItem *DockScrollArea::getDockNodeItem(Node *node)
{
    NodeItem* nodeItem = getNodeView()->getNodeItemFromNode(node);
    return getDockNodeItem(nodeItem);
}


/**
 * @brief DockScrollArea::getDockNodeItems
 * Returns the dock node items contained in this dock.
 * @return
 */
QList<DockNodeItem*> DockScrollArea::getDockNodeItems()
{
    return dockNodeItems;
}


/**
 * @brief DockScrollArea::updateDock
 * If the currently selected node kind is contained in notAllowedKinds,
 * it means that this dock can't be used for the selected node.
 * If so, disable this dock and its parentButton.
 */
void DockScrollArea::updateDock()
{
    if(currentNodeItemID != "-1"){
        if (currentNodeItem) {
            if (currentNodeItem->getNodeKind() == "Model") {
                parentButton->enableDock(false);
            } else if (notAllowedKinds.contains(currentNodeItem->getNodeKind())) {
                parentButton->enableDock(false);
            } else {
                parentButton->enableDock(true);
            }
        } else {
            // no current node item selected
            parentButton->enableDock(false);
        }
    } else {
        // current node item deleted
        parentButton->enableDock(false);
    }
}


/**
 * @brief DockScrollArea::nodeDeleted
 * This tells the dock if a node has been deleted.
 * It either updates dock for the selected node or it disables this dock.
 * @param nodeID
 * @param parentID
 */
void DockScrollArea::nodeDeleted(QString nodeID, QString parentID)
{
    if (parentID == getCurrentNodeID()) {
        updateDock();
    } else if (nodeID == getCurrentNodeID()) {
        currentNodeItemID = "-1";
    }
}


/**
 * @brief DockScrollArea::paintEvent
 * Still trying to catch when the scrollbar appears/disappears to adjust dock size.
 * @param e
 */
void DockScrollArea::paintEvent(QPaintEvent *e)
{
    //qDebug() << verticalScrollBar()->isVisible();
    QScrollArea::paintEvent(e);
}


/**
 * @brief DockScrollArea::removeDockNodeItemFromList
 * This is called whenever a DockNodeItem is deleted.
 * It removes the deleted item from this dock's list.
 * @param item
 */
void DockScrollArea::removeDockNodeItemFromList(DockNodeItem *item)
{
    dockNodeItems.removeAll(item);
}


/**
 * @brief DockScrollArea::getCurrentNodeID
 * This method returns the current node item's ID.
 * @return -1 = node item was deleted
 *         "" = no selected node item
 */
QString DockScrollArea::getCurrentNodeID()
{
    return currentNodeItemID;
}


/**
 * @brief DockScrollArea::dock_itemClicked
 * This is called whenever an item in this dock is clicked.
 */
void DockScrollArea::dockNodeItemClicked() {}


/**
 * @brief DockScrollArea::setupLayout
 * This sets up the groupbox widget contained inside this dock's scroll area.
 * It sets up the visual layout, size and alignment of things.
 */
void DockScrollArea::setupLayout()
{
    QFont guiFont = QFont("Verdana");
    guiFont.setPointSizeF(8.5);

    QLabel* dockLabel = new QLabel(label, this);
    mainLayout = new QVBoxLayout();
    layout = new QVBoxLayout();

    dockLabel->setFont(guiFont);
    dockLabel->setFixedWidth(width());
    dockLabel->setStyleSheet("padding-left: 8px;");

    QGroupBox* groupBox = new QGroupBox(0);
    groupBox->setLayout(layout);
    groupBox->setStyleSheet("QGroupBox {"
                            "background-color: rgba(0,0,0,0);"
                            "border: 0px;"
                            "padding: 25px 10px;"
                            "}");

    layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    layout->setSpacing(5);

    mainLayout->addWidget(dockLabel);
    mainLayout->setAlignment(dockLabel, Qt::AlignLeft);
    mainLayout->addWidget(groupBox);
    mainLayout->addStretch();

    setLayout(mainLayout);
    setWidget(groupBox);
    setVisible(false);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("QScrollArea {"
                  "background-color: rgba(250,250,250,240);"
                  "border: 0px;"
                  "border-radius: 10px;"
                  "padding-top: 10px;"
                  "}");
}


/**
 * @brief DockScrollArea::setParentButton
 * Attach and connect this dock to its parent button.
 */
void DockScrollArea::setParentButton(DockToggleButton *parent)
{
    parentButton = parent;
    parentButton->setContainer(this);
    connect(parentButton, SIGNAL(pressed()), this, SLOT(activate()));
}


/**
 * @brief DockScrollArea::updateScrollBar
 */
void DockScrollArea::updateScrollBar()
{
    bool scrollbarVisible = false;
    if (layout->sizeHint().height() > height()) {
        scrollbarVisible = true;
    }

    // update stylesheet to add/remove extra padding
    QString extraPadding = "padding-left: 0px; padding-right: 0px;";
    if (scrollbarVisible) {
        extraPadding = "padding-left: 8px; padding-right: 8px;";
    }

    setStyleSheet("QScrollArea {"
                  "background-color: rgba(255,255,255,180);"
                  "border: 0px;"
                  "border-radius: 10px;"
                  "padding-top: 10px;"
                  + extraPadding + "}");
}


/**
 * @brief DockScrollArea::activate
 * This shows or hides the scroll area and its groupbox.
 */
void DockScrollArea::activate()
{
    if (activated) {
        setVisible(false);
        activated = false;
    } else {
        setVisible(true);
        activated = true;
    }
}


/**
 * @brief DockScrollArea::clear
 * This method clears/deletes the nodes from this container.
 */
void DockScrollArea::clear()
{
    for (int i=0; i<dockNodeItems.count(); i++) {
        layout->removeWidget(dockNodeItems.at(i));
        delete dockNodeItems.at(i);
    }
    dockNodeItems.clear();
    updateScrollBar();
}


/**
 * @brief DockScrollArea::parentHeightChanged
 */
void DockScrollArea::parentHeightChanged(double height)
{
    resize(width(), height);
}
