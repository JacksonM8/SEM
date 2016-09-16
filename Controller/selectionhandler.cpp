#include "selectionhandler.h"
#include "../View/nodeviewitem.h"
#include "Widgets/New/selectioncontroller.h"

#include <QDebug>
int SelectionHandler::_SelectionHandlerID  = 0;
SelectionHandler::SelectionHandler(SelectionController *controller)
{
    ID = ++_SelectionHandlerID;
    currentActiveSelectedItem = 0;
    newActiveSelectedItem = 0;
    selectionController = controller;
    orderedSelectionValid = true;

    //Empty selection will result in destruction.
    connect(this, SIGNAL(lastRegisteredObjectRemoved()), this, SLOT(deleteLater()));
}

int SelectionHandler::getID()
{
    return ID;
}

void SelectionHandler::toggleItemsSelection(ViewItem *item, bool append)
{
    QList<ViewItem*> items;
    items << item;
    toggleItemsSelection(items, append);
}

void SelectionHandler::toggleItemsSelection(QList<ViewItem *> items, bool append)
{
    int changes = 0;
    if(!append){
        //Unselect for non-append
        changes += _clearSelection();
    }
    foreach(ViewItem* item, items){
        changes += _toggleItemsSelection(item);
    }

    _selectionChanged(changes);
}

void SelectionHandler::itemDeleted(int ID, ViewItem *item)
{
    int changes = _toggleItemsSelection(item, true);
    _selectionChanged(changes);
}

void SelectionHandler::clearSelection()
{
    int changes = _clearSelection();
    _selectionChanged(changes);
}

void SelectionHandler::setActiveSelectedItem(ViewItem *viewItem)
{
    if(viewItem && currentSelection.contains(viewItem)){
        newActiveSelectedItem = viewItem;
        _selectionChanged();
    }
}

void SelectionHandler::cycleActiveSelectedItem(bool forward)
{
    if(currentActiveSelectedItem){
        int index = currentSelection.indexOf(currentActiveSelectedItem);
        int lastPos = currentSelection.size() - 1;
        if(forward){
            index ++;
        }else{
            index --;
        }

        if(index > lastPos){
            index = 0;
        }else if(index < 0){
            index = lastPos;
        }

        newActiveSelectedItem = currentSelection.at(index);
        _selectionChanged();
    }
}

QVector<ViewItem *> SelectionHandler::getSelection() const
{
    return currentSelection;
}

QVector<ViewItem *> SelectionHandler::getOrderedSelection()
{
    if(!orderedSelectionValid && selectionController){
        orderedSelection = selectionController->getOrderedSelection(getSelectionIDs().toList());
        orderedSelectionValid = true;
    }

    return orderedSelection;
}

QVector<int> SelectionHandler::getSelectionIDs()
{
    QVector<int> IDs;
    foreach(ViewItem* item, currentSelection){
        IDs.append(item->getID());
    }
    return IDs;
}

int SelectionHandler::getSelectionCount()
{
    return currentSelection.count();
}

ViewItem *SelectionHandler::getFirstSelectedItem()
{
    ViewItem* item = 0;
    if(!currentSelection.isEmpty()){
        item = currentSelection.first();
    }
    return item;
}

ViewItem *SelectionHandler::getActiveSelectedItem()
{
    return newActiveSelectedItem;
}

void SelectionHandler::_selectionChanged(int changes)
{
    if(changes > 0){
        orderedSelectionValid = false;
        emit selectionChanged(currentSelection.size());
    }
    if(newActiveSelectedItem != currentActiveSelectedItem){
        if(currentActiveSelectedItem){
            emit itemActiveSelectionChanged(currentActiveSelectedItem, false);
        }
        currentActiveSelectedItem = newActiveSelectedItem;
        newActiveSelectedItem = currentActiveSelectedItem;

        emit itemActiveSelectionChanged(currentActiveSelectedItem, true);
    }
}

int SelectionHandler::_clearSelection()
{
    int itemsChanged = 0;
    foreach(ViewItem* item, currentSelection){
        itemsChanged += _toggleItemsSelection(item);
        //itemsChanged += _setItemSelected(item, false);
    }
    return itemsChanged;
}

/**
 * @brief SelectionHandler::isItemsAncestorSelected Returns whether or not an Item's ancestor is already selected.
 * @param item
 * @return
 */
bool SelectionHandler::isItemsAncestorSelected(ViewItem *item)
{
    bool selected = false;
    if(item->isNode()){
        NodeViewItem* nodeItem = (NodeViewItem*)item;
        foreach(ViewItem* selectedItem, currentSelection){
            if(selectedItem->isNode()){
                NodeViewItem* selectedNodeItem = (NodeViewItem*)selectedItem;
                if(selectedNodeItem->isAncestorOf(nodeItem)){
                    selected = true;
                    break;
                }
            }
        }
    }
    return selected;
}

int SelectionHandler::unsetItemsDescendants(ViewItem *item)
{
    int itemsUnset = 0;
    if(item->isNode()){
        NodeViewItem* nodeItem = (NodeViewItem*)item;
        foreach(ViewItem* selectedItem, currentSelection){
            if(selectedItem->isNode()){
                NodeViewItem* selectedNodeItem = (NodeViewItem*)selectedItem;

                if(nodeItem->isAncestorOf(selectedNodeItem)){
                    //Remove it.
                    itemsUnset += _toggleItemsSelection(selectedItem);
                }
            }
        }
    }
    return itemsUnset;
}

int SelectionHandler::_toggleItemsSelection(ViewItem *item, bool deletingItem)
{
    int changeCount = 0;

    bool inSelection = currentSelection.contains(item);
    if(deletingItem){
        if(!inSelection){
            //We don't need to unselect item.
            return changeCount;
        }
    }
    changeCount += _setItemSelected(item, !inSelection);

    if(changeCount > 0 && !deletingItem){
        emit itemSelectionChanged(item, !inSelection);
    }
    return changeCount;
}

int SelectionHandler::_setItemSelected(ViewItem *item, bool selected)
{
    int changeCount = 0;
    if(selected){
        //Register the selection handler
        item->registerObject(this);
        currentSelection.append(item);
        //If there is only 1 item there can only be 1 active item.
        if(currentSelection.size() == 1){
            newActiveSelectedItem = item;
        }
        changeCount += 1;
    }else{
        //Remove it from the map.
        changeCount = currentSelection.removeAll(item);
        if(changeCount > 0){
            //Unregister the selection handler
            item->unregisterObject(this);
        }

        //If there is no items left, there is no active item
        if(currentSelection.isEmpty()){
            newActiveSelectedItem = 0;
        }else{
            if(currentActiveSelectedItem == item || newActiveSelectedItem == item){
                newActiveSelectedItem = currentSelection.first();
            }
        }
    }
    return changeCount;
}


