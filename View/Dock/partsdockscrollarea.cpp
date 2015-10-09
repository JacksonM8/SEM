#include "partsdockscrollarea.h"
#include "docktogglebutton.h"
#include "docknodeitem.h"
#include <QDebug>

/**
 * @brief PartsDockScrollArea::PartsDockScrollArea
 * @param label
 * @param view
 * @param parent
 */
PartsDockScrollArea::PartsDockScrollArea(QString label, NodeView *view, DockToggleButton *parent) :
    DockScrollArea(label, view, parent)
{
    kindsRequiringDefinition.append("BlackBoxInstance");
    kindsRequiringDefinition.append("ComponentInstance");
    kindsRequiringDefinition.append("ComponentImpl");
}


/**
 * @brief PartsDockScrollArea::updateDock
 * This is called whenever a node item is selected, constructed or destructed.
 * This checks the new adoptable nodes list to see which dock node
 * items need to either be hidden or shown from this dock.
 */
void PartsDockScrollArea::updateDock()
{
    bool noUpdateRequired = false;

    // this will enable/disable the dock depending on whether there's a selected item
    DockScrollArea::updateDock();

    // if the dock is disabled, there is no need to update
    if (!isDockEnabled()) {
        noUpdateRequired = true;
    }

    QStringList kindsToShow = getAdoptableNodeListFromView();

    // if there are no adoptable node kinds, disable the dock
    if (kindsToShow.isEmpty()) {
        setDockEnabled(false);
        noUpdateRequired = true;
    }

    // when disabling this dock, if HIDE_DISABLED is turned off,
    // instead of completely hiding this dock and disabling its
    // parent button, keep it visible but hide all of its dock items
    if (noUpdateRequired) {
        if (!hideDisabledDock()) {
            foreach (QString kind, displayedItems) {
                DockNodeItem* dockNodeItem = getDockNodeItem(kind);
                dockNodeItem->hide();
            }
            displayedItems.clear();
        }
        return;
    }

    // for each kind to show, check if it is already displayed
    // if it is, remove it from the list of kinds to show
    // if it's not, kind shouldn't be displayed anymore; hide it
    foreach (QString kind, displayedItems) {
        if (kindsToShow.contains(kind)) {
            kindsToShow.removeAll(kind);
        } else {
            DockNodeItem* dockNodeItem = getDockNodeItem(kind);
            if (dockNodeItem) {
                dockNodeItem->hide();
                displayedItems.removeAll(kind);
            }
        }
    }

    // show remaining kinds that aren't already displayed
    // and add it to the displayed kinds list
    foreach (QString kind, kindsToShow) {
        DockNodeItem* dockNodeItem = getDockNodeItem(kind);
        if (dockNodeItem) {
            dockNodeItem->show();
            displayedItems.append(kind);
        }
    }
}


/**
 * @brief PartsDockScrollArea::addDockNodeItems
 * Add adoptable dock node items to dock.
 * This creates a groupbox for each adoptable node kind
 * which contains the new dock node item and its label.
 * @param nodeKinds
 */
void PartsDockScrollArea::addDockNodeItems(QStringList nodeKinds)
{
    nodeKinds.removeDuplicates();
    nodeKinds.sort();

    foreach (QString kind, nodeKinds) {
        if (!getDockNodeItem(kind)) {
            DockNodeItem* dockNodeItem = new DockNodeItem(kind, 0, this);
            addDockNodeItem(dockNodeItem);
        }
    }

    // initialise list of displayed items
    displayedItems = nodeKinds;
}


/**
 * @brief PartsDockScrollArea::dockNodeItemPressed
 * This gets called when a dock adoptable node item is clicked.
 * It tells the view to create a NodeItem with the specified
 * kind inside the currently selected node.
 */
void PartsDockScrollArea::dockNodeItemClicked()
{
    DockNodeItem* sender = qobject_cast<DockNodeItem*>(QObject::sender());
    QString nodeKind = sender->getKind();
    if (kindsRequiringDefinition.contains(nodeKind)) {
        emit dock_openDefinitionsDock();
    } else {
        getNodeView()->constructNode(nodeKind, 0);
    }
}
