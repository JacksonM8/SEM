#include "toolbarwidget.h"
#include "toolbarmenu.h"

#include <QApplication>
#include <QClipboard>
#include <QHBoxLayout>
#include <QDebug>
#include <QWidgetAction>
#include <QPair>

/**
 * @brief ToolbarWidget::ToolbarWidget
 * @param parent
 */
ToolbarWidget::ToolbarWidget(NodeView* parent) :
    QWidget(parent)
{
    nodeView = parent;
    nodeItem = 0;

    showDeleteToolButton = false;
    showImportSnippetToolButton = false;
    showExportSnippetToolButton = false;
    showDefinitionToolButton = false;
    showImplementationToolButton = false;

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Popup);

    // these frames, combined with the set attribute and flags, allow
    // the toolbar to have a translucent background and a mock shadow
    shadowFrame = new QFrame(this);
    mainFrame = new QFrame(this);

    setupToolBar();
    setupTheme();

    makeConnections();
    resetButtonGroupFlags();
}


/**
 * @brief ToolbarWidget::updateToolbar
 * This only gets called when the toolbar is about to show.
 * Set nodeItem to the currectly selected item and update applicable tool buttons and menus.
 * If there are multiple items selected, only update the buttons that are for multiple selection.
 * @param nodeItems
 * @param edgeItems
 */
void ToolbarWidget::updateToolbar(QList<NodeItem *> nodeItems, QList<EdgeItem*> edgeItems)
{
    resetButtonGroupFlags();
    hideButtons();
    hideSeparators();

    // these buttons are enabled/disabled by NodeView whenever an item is selected/deselected
    deleteButton->setVisible(showDeleteToolButton);
    exportSnippetButton->setVisible(showExportSnippetToolButton);
    importSnippetButton->setVisible(showImportSnippetToolButton);

    alterModelButtonsVisible = showDeleteToolButton;
    snippetButtonsVisible = showExportSnippetToolButton || showImportSnippetToolButton;

    // update the rest of the tool buttons and menus if only node items are selected
    if (edgeItems.isEmpty()) {
        updateButtonsAndMenus(nodeItems);
    }

    // update button group separators after the buttons and menus have been updated
    updateSeparators();
}

void ToolbarWidget::updateFunctionList()
{

    if(nodeView){
        QPair<QString, QString> function;
        foreach (function, nodeView->getFunctionsList()){
            QString className = function.first;
            QString functionName = function.second;

            //Check if we have this item.
            QString actionName = className + "_" + functionName;
            if(!functionActionLookup.contains(actionName)){
                ToolbarMenu* classMenu = 0;
                //Get the Menu
                if(classToolbarMenuLookup.contains(className)){
                    classMenu = classToolbarMenuLookup[className];
                }

                if(!classMenu){
                    classMenu = new ToolbarMenu(this);
                    ToolbarMenuAction* classAction = new ToolbarMenuAction(className, functionsMenu, className, ":/Functions/");
                    functionsMenu->addAction(classAction);
                    classAction->setMenu(classMenu);
                    classToolbarMenuLookup[className] = classMenu;
                }
                ToolbarMenuAction* functionAction = new ToolbarMenuAction("Process", this, functionName, ":/Items/");
                classMenu->addAction(functionAction);
                functionActionLookup[actionName] = functionAction;
            }
        }
    }
}


/**
 * @brief ToolbarWidget::updateActionEnabled
 * @param actionName
 * @param enabled
 */
void ToolbarWidget::updateActionEnabledState(QString actionName, bool enabled)
{
    if (actionName == "delete") {
        showDeleteToolButton = enabled;
    } else if (actionName == "definition") {
        showDefinitionToolButton = enabled;
    } else if (actionName == "implementation") {
        showImplementationToolButton = enabled;
    } else if (actionName == "exportSnippet") {
        showExportSnippetToolButton = enabled;
    } else if (actionName == "importSnippet") {
        showImportSnippetToolButton = enabled;
    }
}


/**
 * @brief ToolbarWidget::updateDisplayedChildren
 * This sends a signal to the view to display all/connected/unconnected children nodes of the selected hardware cluster(s).
 */
void ToolbarWidget::updateDisplayedChildren()
{
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    if (rb == allNodes) {
        nodeView->hardwareClusterMenuClicked(0);
    } else if (rb == connectedNodes) {
        nodeView->hardwareClusterMenuClicked(1);
    } else if (rb == unconnectedNodes) {
        nodeView->hardwareClusterMenuClicked(2);
    }
}


/**
 * @brief ToolbarWidget::hardwareClusterMenuClicked
 * This slot is called everytime a HardwareCluster's menu is triggered.
 * It updates this toolbar's radio button set corresponding to the menu's actions.
 * This slot is also used to update the radio button set when there are multiple items selected.
 * @param viewMode
 */
void ToolbarWidget::hardwareClusterMenuClicked(int viewMode)
{
    switch (viewMode) {
    case 0:
        allNodes->setChecked(true);
        break;
    case 1:
        connectedNodes->setChecked(true);
        break;
    case 2:
        unconnectedNodes->setChecked(true);
        break;
    default:
        allNodes->setAutoExclusive(false);
        connectedNodes->setAutoExclusive(false);
        unconnectedNodes->setAutoExclusive(false);
        allNodes->setChecked(false);
        connectedNodes->setChecked(false);
        unconnectedNodes->setChecked(false);
        allNodes->setAutoExclusive(true);
        connectedNodes->setAutoExclusive(true);
        unconnectedNodes->setAutoExclusive(true);
        break;
    }
}


/**
 * @brief ToolbarWidget::addChildNode
 * This sends a signal to the view to construct a new node with the specified kind.
 * @param action
 */
void ToolbarWidget::addChildNode(ToolbarMenuAction* action)
{
    nodeView->constructNode(action->getActionKind(), 1);
}


/**
 * @brief ToolbarWidget::addConnectedNode
 * This sends a signal to the view to construct a connected node.
 * It can either be a ComponentImpl, ComponentInstance, BlackBoxInstance or In/Out EventPortDelegates.
 * @param action
 */
void ToolbarWidget::addConnectedNode(ToolbarMenuAction* action)
{    
    ToolbarMenu* menu = qobject_cast<ToolbarMenu*>(QObject::sender());
    QString kindToConstruct;

    if (menu == componentImplMenu) {
        kindToConstruct = "ComponentImpl";
    } else if (menu == componentInstMenu) {
        kindToConstruct = "ComponentInstance";
    } else if (menu == inEventPortDelegateMenu) {
        kindToConstruct = "InEventPortDelegate";
    } else if (menu == outEventPortDelegateMenu) {
        kindToConstruct = "OutEventPortDelegate";
    } else if (menu == blackBoxInstMenu) {
        kindToConstruct = "BlackBoxInstance";
    } else if (menu == aggregateInstMenu) {
        kindToConstruct = "AggregateInstance";
    } else if (menu == vectorInstMenu) {
        kindToConstruct = "VectorInstance";
    } else {
        qWarning() << "ToolbarWidget::addConnectedNode - Action not handled.";
        return;
    }

    nodeView->constructConnectedNode(nodeItem->getID(), action->getNodeItemID(), kindToConstruct, 1);
}


/**
 * @brief ToolbarWidget::connectNodes
 * This sends a signal to the view to construct an edge between the selected node and chosen node.
 */
void ToolbarWidget::connectNodes(ToolbarMenuAction* action)
{
    nodeView->constructDestructEdges(QList<int>(), action->getNodeItemID());
}


/**
 * @brief ToolbarWidget::displayConnectedNode
 * This sends a signal to the view to either center on or pop up a new window centered on
 * the selected node item's definition, implementation or chosen instance.
 * @param action
 */
void ToolbarWidget::displayConnectedNode(ToolbarMenuAction* action)
{
    // if there is no action, the sender is a QToolButton
    if (!action) {
        QToolButton* button = qobject_cast<QToolButton*>(QObject::sender());
        if (button == definitionButton) {
            nodeView->centerDefinition(nodeItem->getID());
        } else if (button == implementationButton) {
            nodeView->centerImplementation(nodeItem->getID());
        }
        return;
    }

    QString actionKind = action->getActionKind();
    QWidget* parentWidget = action->parentWidget();

    if (actionKind == "Goto") {
        if (parentWidget == definitionMenu) {
            nodeView->centerDefinition(nodeItem->getID());
        } else if (parentWidget == implementationMenu) {
            nodeView->centerImplementation(nodeItem->getID());
        } else if (parentWidget == instancesMenu) {
            nodeView->centerInstance(chosenInstanceID);
        }
    } else if (actionKind == "Popup") {
        if (parentWidget == definitionMenu) {
            nodeView->constructNewView(1);
        } else if (parentWidget == implementationMenu) {
            nodeView->constructNewView(2);
        } else if (parentWidget == instancesMenu) {
            nodeView->constructNewView(chosenInstanceID);
        }
    } else {
        qWarning() << "ToolbarWidget::displayConnectedNode -  Action kind not handled.";
    }
}


/**
 * @brief ToolbarWidget::expandContractNodes
 * This sends a signal to the view to either expand or contract the selected node items.
 */
void ToolbarWidget::expandContractNodes()
{
    QToolButton* button = qobject_cast<QToolButton*>(QObject::sender());
    if (button == expandButton) {
        nodeView->expandSelection(true);
    } else if (button == contractButton) {
        nodeView->expandSelection(false);
    } else {
        qWarning() << "ToolbarWidget::expandContractNodes - Sender object not handled.";
    }
}


/**
 * @brief ToolbarWidget::constructNewView
 * This sends a signal to the view to pop up a new window centered on the selected node.
 */
void ToolbarWidget::constructNewView()
{
    nodeView->constructNewView();
}


/**
 * @brief ToolbarWidget::hideToolbar
 * This slot is called when either one of the opened menus are triggered or the mouse was pressed outside of them.
 * If the menu was not triggered and the mouse press happened inside the toolbar, don't hide the toolbar.
 * @param action
 */
void ToolbarWidget::hideToolbar(QAction* action)
{
    if (!action) {
        QPoint cursorPos = mapFromGlobal(QCursor::pos());
        if (rect().contains(cursorPos)) {
            return;
        }
    }
    hide();
}


/**
 * @brief ToolbarWidget::hide
 */
void ToolbarWidget::hide()
{
    setVisible(false);
}


/**
 * @brief ToolbarWidget::setVisible
 * @param visible
 */
void ToolbarWidget::setVisible(bool visible)
{
    bool toolbarVisible = visible && (alterModelButtonsVisible || alignButtonsVisible
                                      || snippetButtonsVisible || goToButtonsVisible
                                      || alterViewButtonsVisible || expandContractButtonsVisible);

    // update the toolbar & frame sizes
    if (toolbarVisible) {
        mainFrame->setFixedSize(toolbarLayout->sizeHint());
        shadowFrame->setFixedSize(toolbarLayout->sizeHint() + QSize(3,3));
        setFixedSize(shadowFrame->size());
    } else {
        closeOpenMenus();
    }

    mainFrame->setVisible(toolbarVisible);
    shadowFrame->setVisible(toolbarVisible);
    QWidget::setVisible(toolbarVisible);
}


/**
 * @brief ToolbarWidget::setupTheme
 * @param theme
 */
void ToolbarWidget::setupTheme(VIEW_THEME theme)
{
    QString buttonBorder = "1px solid rgba(160,160,160,250);";
    QString hoverBorder = "1.5px solid rgba(170,170,170,250);";

    QString mainBackground = "rgba(250,250,250,200);";
    QString shadowBackground = "rgba(50,50,50,150);";

    QString background = "rgba(240,240,240,250);";
    QString textColor = "black;";
    QString selectedColor = "green";
    QString hoverTextColor = "black;";
    QString hoverBackground = "rgba(230,230,230,250);";

    switch (theme) {
    case VT_DARK_THEME:
        buttonBorder = "1px solid rgba(100,100,100,250);";
        hoverBorder = "1.5px solid rgba(100,100,100,250);";
        mainBackground = "rgba(150,150,150,200);";
        shadowBackground = "rgba(50,50,50,200);";
        background = "rgba(130,130,130,250);";
        selectedColor = "yellow;";
        textColor = "white;";
        break;
    default:
        break;
    }

    setStyleSheet("QToolButton {"
                  "border:" + buttonBorder +
                  "background-color: rgba(240,240,240,240);"
                  "}"
                  "QToolButton:hover {"
                  "border:" + hoverBorder +
                  "background-color: rgba(255,255,255,255);"
                  "}"
                  "QToolButton[popupMode=\"1\"] {"
                  "padding-right: 15px;"
                  "}"
                  "QToolButton::menu-button {"
                  "border-left: 1px solid gray;"
                  "border-top-right-radius: 10px;"
                  "border-bottom-right-radius: 10px;"
                  "width: 15px;"
                  "}"
                  "QRadioButton {"
                  "padding: 8px 10px 8px 8px;"
                  "color:" + textColor +
                  "}"
                  "QRadioButton::checked {"
                  "font-weight: bold; "
                  "color:" + selectedColor +
                  "}"
                  "QMenu { "
                  "background-color:" + background +
                  "}"
                  "QMenu::item {"
                  "padding: 1px 20px 1px 45px;"
                  "background-color:" + background +
                  "color:" + textColor +
                  "border: none;"
                  "}"
                  "QMenu::item:selected {"
                  "background-color:" + hoverBackground +
                  "color:" + hoverTextColor +
                  "border: 1px solid gray;"
                  "}"
                  "QMenu::icon {"
                  "position: absolute;"
                  "top: 1px;"
                  "right: 3px;"
                  "bottom: 1px;"
                  "left: 4px;"
                  "}"
                  );

    mainFrame->setStyleSheet("background-color:" + mainBackground +
                             "border-radius: 8px;");
    shadowFrame->setStyleSheet("background-color:" + shadowBackground +
                               "border-radius: 10px;");
    currentTheme = theme;
}


/**
 * @brief ToolbarWidget::appendToOpenedMenusList
 */
void ToolbarWidget::appendToOpenMenusList()
{
    ToolbarMenu* menu = qobject_cast<ToolbarMenu*>(QObject::sender());
    openMenus.append(menu);
}


/**
 * @brief ToolbarWidget::removeFromOpenedMenusList
 */
void ToolbarWidget::removeFromOpenMenusList()
{
    ToolbarMenu* menu = qobject_cast<ToolbarMenu*>(QObject::sender());
    openMenus.removeAll(menu);

    QPoint cursorPos = mapFromGlobal(QCursor::pos());
    if (rect().contains(cursorPos)) {
        closeOpenMenus();
    }
}


/**
 * @brief ToolbarWidget::setInstanceID
 * This slot is called when an action from the instancesMenu is hovered over.
 * It updates chosenInstanceID which is used for displaying the selected node's chosen instance.
 */
void ToolbarWidget::setInstanceID()
{
    ToolbarMenuAction* action = qobject_cast<ToolbarMenuAction*>(QObject::sender());
    chosenInstanceID = action->getNodeItemID();
}


/**
 * @brief ToolbarWidget::menuActionHovered
 * @param action
 */
void ToolbarWidget::menuActionHovered(QAction* action)
{
    if (!action) {
        nodeView->highlightOnHover();
        return;
    }

    ToolbarMenuAction* menuAction = qobject_cast<ToolbarMenuAction*>(action);
    if (menuAction) {
        nodeView->highlightOnHover(menuAction->getNodeItemID());
    }
}


/**
 * @brief ToolbarWidget::setupLegalNodesList
 */
void ToolbarWidget::setupLegalNodesList()
{
    if (connectMenuDone) {
        return;
    } else {
        connectMenuDone = true;
    }

    NodeItem* selectedItem = nodeView->getSelectedNodeItem();
    NodeItem* selectedParentItem = 0;
    if (selectedItem) {
        selectedParentItem = selectedItem->getParentNodeItem();
    }

    QList<NodeItem*> topActions, subActions;
    foreach (NodeItem* item, legalNodeItems) {
        NodeItem* parentItem = item->getParentNodeItem();
        if (selectedParentItem == parentItem) {
            topActions.append(item);
        } else {
            subActions.append(item);
        }
    }

    foreach (NodeItem* topItem, topActions) {
        constructMenuAction(topItem, connectMenu);
    }
    if (!topActions.isEmpty() && !subActions.isEmpty()) {
        connectMenu->addSeparator();
    }
    foreach (NodeItem* subItem, subActions) {
        constructSubMenuAction(subItem, connectMenu);
    }
}


/**
 * @brief ToolbarWidget::setupComponentList
 */
void ToolbarWidget::setupComponentList()
{
    ToolbarMenuAction* action = qobject_cast<ToolbarMenuAction*>(QObject::sender());
    if (action == componentImplAction) {
        setupComponentList("impl");
    } else if (action == componentInstAction) {
        setupComponentList("inst");
    } else {
        qWarning() << "ToolbarWidget::setupComponentList - Sender action not handled.";
    }
}


/**
 * @brief ToolbarWidget::setupBlackBoxList
 */
void ToolbarWidget::setupBlackBoxList()
{
    if (blackBoxMenuDone) {
        return;
    } else {
        blackBoxMenuDone = true;
    }

    QList<NodeItem*> blackBoxes = nodeView->getEntityItemsOfKind("BlackBox");
    foreach (NodeItem* blackBox, blackBoxes) {
        constructSubMenuAction(blackBox, blackBoxInstMenu);
    }
}


/**
 * @brief ToolbarWidget::setupEventPortInstanceList
 */
void ToolbarWidget::setupEventPortInstanceList()
{
    ToolbarMenuAction* action = qobject_cast<ToolbarMenuAction*>(QObject::sender());
    if (action == inEventPortDelegateAction) {
        setupEventPortInstanceList("InEventPortInstance");
    } else if (action == outEventPortDelegateAction) {
        setupEventPortInstanceList("OutEventPortInstance");
    } else {
        qWarning() << "ToolbarWidget::setupEventPortInstanceList - Sender action not handled.";
    }
}


/**
 * @brief ToolbarWidget::setupAggregateList
 */
void ToolbarWidget::setupAggregateList()
{
    if (aggregateMenuDone) {
        return;
    } else {
        aggregateMenuDone = true;
    }

    QList<NodeItem*> agrregates = nodeView->getEntityItemsOfKind("Aggregate");
    foreach (NodeItem* aggregate, agrregates) {
        constructSubMenuAction(aggregate, aggregateInstMenu);
    }
}


/**
 * @brief ToolbarWidget::setupVectorList
 */
void ToolbarWidget::setupVectorList()
{
    if (vectorMenuDone) {
        return;
    } else {
        vectorMenuDone = true;
    }

    QList<NodeItem*> vectors = nodeView->getEntityItemsOfKind("Vector");
    foreach (NodeItem* vector, vectors) {
        constructSubMenuAction(vector, vectorInstMenu);
    }
}


/**
 * @brief ToolbarWidget::setupHardwareList
 */
void ToolbarWidget::setupHardwareList()
{
    if (hardwareMenuDone) {
        return;
    } else {
        hardwareMenuDone = true;
    }

    QList<EntityItem*> hardware = nodeView->getHardwareList();

    if (hardware.isEmpty()) {
        return;
    } else {
        hardwareButton->show();
        alterModelButtonsVisible = true;
    }

    QList<NodeItem*> topActions, subActions;

    foreach (EntityItem* item, hardware) {
        if (!item->getParentEntityItem()) {
            topActions.append(item);
        } else {
            subActions.append(item);
        }
    }

    foreach (NodeItem* topItem, topActions) {
        constructMenuAction(topItem, hardwareMenu);
    }
    if (!topActions.isEmpty() && !subActions.isEmpty()) {
        hardwareMenu->addSeparator();
    }
    foreach (NodeItem* subItem, subActions) {
        constructSubMenuAction(subItem, hardwareMenu);
    }
}


/**
 * @brief ToolbarWidget::setupToolBar
 * Initialise and setup the layout and tool buttons and separators.
 */
void ToolbarWidget::setupToolBar()
{
    toolbarLayout = new QHBoxLayout();
    toolbarLayout->setMargin(5);
    toolbarLayout->setAlignment(Qt::AlignTop);
    setLayout(toolbarLayout);

    QSize buttonSize = QSize(39,39);

    // construct tool buttons and separators and add them to the toolbar's layout
    addChildButton = constructToolButton(buttonSize, 0.8, "Plus", "Add Child Entity");
    connectButton = constructToolButton(buttonSize, 0.7, "ConnectTo", "Connect Selection");
    hardwareButton = constructToolButton(buttonSize, 0.7,  "HardwareNode", "Deploy Selection", "Items");
    deleteButton = constructToolButton(buttonSize, 0.65, "Delete", "Delete Selection");
    alignFrame = constructFrameSeparator();
    alignVerticallyButton = constructToolButton(buttonSize, 0.6, "Align_Vertical", "Align Selection Vertically");
    alignHorizontallyButton = constructToolButton(buttonSize, 0.6, "Align_Horizontal", "Align Selection Horizontally");
    expandContractFrame = constructFrameSeparator();
    expandButton = constructToolButton(buttonSize, 0.6, "Expand", "Expand Selection");
    contractButton = constructToolButton(buttonSize, 0.6, "Contract", "Contract Selection");
    snippetFrame = constructFrameSeparator();
    importSnippetButton = constructToolButton(buttonSize, 0.6, "ImportSnippet", "Import GraphML Snippet");
    exportSnippetButton = constructToolButton(buttonSize, 0.6, "ExportSnippet", "Export GraphML Snippet");
    goToFrame = constructFrameSeparator();
    definitionButton = constructToolButton(buttonSize, 0.55, "Definition", "View Definition");
    implementationButton = constructToolButton(buttonSize, 0.6, "Implementation", "View Implementation");
    instancesButton = constructToolButton(buttonSize, 0.6, "Instance", "View Instances");
    alterViewFrame = constructFrameSeparator();
    connectionsButton = constructToolButton(buttonSize, 0.6, "Connections", "View Connections");
    popupNewWindow = constructToolButton(buttonSize, 0.55, "Popup", "View In New Window");
    displayedChildrenOptionButton = constructToolButton(buttonSize, 0.7, "MenuCluster", "Change Displayed Nodes");

    deleteButton->setStyleSheet("padding-right: 3px;");

    /*
     * This is what makes the tool buttons with a split menu button look good on a Mac
     *
    "QToolButton[popupMode=\"1\"] {"
    "padding-right: 12px;"
    "}"
    "QToolButton::menu-button {"
    "border-left: 1px solid gray;"
    "border-top-right-radius: 10px;"
    "border-bottom-right-radius: 10px;"
    "padding-right: 4px;"
    "width: 10px;"
    "}"
    */

    setupMenus();
}


/**
 * @brief ToolbarWidget::setupMenus
 * Setup menus, sub-menus and their parent and default ToolbarWidgetActions.
 */
void ToolbarWidget::setupMenus()
{
    // construct main menus
    addMenu = constructToolButtonMenu(addChildButton);
    connectMenu = constructToolButtonMenu(connectButton, false);
    hardwareMenu = constructToolButtonMenu(hardwareButton);
    definitionMenu = constructToolButtonMenu(definitionButton, false);
    implementationMenu = constructToolButtonMenu(implementationButton, false);
    instancesMenu = constructToolButtonMenu(instancesButton);
    hardwareClusterViewMenu = constructToolButtonMenu(displayedChildrenOptionButton);

    // setup menu actions for the definition and implementation menus
    definitionMenu->addAction(new ToolbarMenuAction("Goto", definitionMenu, "Go to Definition", ":/Actions/"));
    definitionMenu->addAction(new ToolbarMenuAction("Popup", definitionMenu, "Popup Definition", ":/Actions/"));
    implementationMenu->addAction(new ToolbarMenuAction("Goto", implementationMenu, "Go to Implementation", ":/Actions/"));
    implementationMenu->addAction(new ToolbarMenuAction("Popup", implementationMenu, "Popup Implementation", ":/Actions/"));

    // setup widgets for the displayed children option menu for HardwareClusters
    allNodes = new QRadioButton("All", this);
    connectedNodes = new QRadioButton("Connected", this);
    unconnectedNodes = new QRadioButton("Unconnected", this);
    QWidgetAction* a1 = new QWidgetAction(this);
    QWidgetAction* a2 = new QWidgetAction(this);
    QWidgetAction* a3 = new QWidgetAction(this);
    a1->setDefaultWidget(allNodes);
    a2->setDefaultWidget(connectedNodes);
    a3->setDefaultWidget(unconnectedNodes);
    hardwareClusterViewMenu->QMenu::addAction(a1);
    hardwareClusterViewMenu->QMenu::addAction(a2);
    hardwareClusterViewMenu->QMenu::addAction(a3);

    // these actions are not deletable - when their parent menu is cleared, they're only hidden
    componentImplAction = new ToolbarMenuAction("ComponentImpl", this);
    componentInstAction = new ToolbarMenuAction("ComponentInstance", this);
    inEventPortDelegateAction = new ToolbarMenuAction("InEventPortDelegate", this);
    outEventPortDelegateAction = new ToolbarMenuAction("OutEventPortDelegate", this);
    blackBoxInstAction = new ToolbarMenuAction("BlackBoxInstance", this);
    aggregateInstAction = new ToolbarMenuAction("AggregateInstance", this);
    vectorInstAction = new ToolbarMenuAction("VectorInstance", this);

    // default actions to display information for when their parent menus are empty
    componentImplMenuInfoAction = new ToolbarMenuAction("Info", this, "There are no IDL files containing unimplemented Components.", ":/Actions/");
    componentInstMenuInfoAction = new ToolbarMenuAction("Info", this, "There are no IDL files containing Components.", ":/Actions/");
    blackBoxMenuInfoAction = new ToolbarMenuAction("Info", this, "There are no IDL files containing BlackBoxes.", ":/Actions/");
    inEventPortDelegateMenuInfoAction = new ToolbarMenuAction("Info", this, "This Assembly does not contain any InEventPortInstances that has a definition that is connected to an Aggregate.", ":/Actions/");
    outEventPortDelegateMenuInfoAction = new ToolbarMenuAction("Info", this, "This Assembly does not contain any OutEventPortInstances that has a definition that is connected to an Aggregate.", ":/Actions/");
    aggregateInstMenuInfoAction = new ToolbarMenuAction("Info", this, "There are no IDL files containing Aggregates.", ":/Actions/");
    vectorInstMenuInfoAction = new ToolbarMenuAction("Info", this, "There are no IDL files containing Vectors.", ":/Actions/");

    // hidden menus for ComponentInstances, ComponentImpls, In/Out EventPortDelegates and BlackBoxInstances
    componentImplMenu = new ToolbarMenu(this, componentImplMenuInfoAction);
    componentInstMenu = new ToolbarMenu(this, componentInstMenuInfoAction);
    blackBoxInstMenu = new ToolbarMenu(this, blackBoxMenuInfoAction);
    inEventPortDelegateMenu = new ToolbarMenu(this, inEventPortDelegateMenuInfoAction);
    outEventPortDelegateMenu = new ToolbarMenu(this, outEventPortDelegateMenuInfoAction);
    aggregateInstMenu = new ToolbarMenu(this, aggregateInstMenuInfoAction);
    vectorInstMenu = new ToolbarMenu(this, vectorInstMenuInfoAction);

    componentImplAction->setMenu(componentImplMenu);
    componentInstAction->setMenu(componentInstMenu);
    inEventPortDelegateAction->setMenu(inEventPortDelegateMenu);
    outEventPortDelegateAction->setMenu(outEventPortDelegateMenu);
    blackBoxInstAction->setMenu(blackBoxInstMenu);
    aggregateInstAction->setMenu(aggregateInstMenu);
    vectorInstAction->setMenu(vectorInstMenu);

    processAction = new ToolbarMenuAction("Process", this);
    functionsMenuInfoAction = new ToolbarMenuAction("Info", this, "There are no available functions.", ":/Actions/");
    functionsMenu = new ToolbarMenu(this, functionsMenuInfoAction);
    processAction->setMenu(functionsMenu);

    QHash<QString, ToolbarMenu*> classToolbarMenuLookup;

    //ToolbarMenuAction* classAction = new ToolbarMenuAction("Function", functionsMenu, "VectorOperation", ":/Actions/");
    //ToolbarMenu* classMenu = new ToolbarMenu(this);



    //classMenu->addAction(new ToolbarMenuAction("Get", this, "Get", ":/Functions/"));
    //classMenu->addAction(new ToolbarMenuAction("Set", this, "Set", ":/Functions/"));
    //classMenu->addAction(new ToolbarMenuAction("Remove", this, "Remove", ":/Functions/"));
    //classMenu->addAction(new ToolbarMenuAction("Process", this, "Get", ":/Items/"));
    //classMenu->addAction(new ToolbarMenuAction("Process", this, "Set", ":/Items/"));
    //classMenu->addAction(new ToolbarMenuAction("Process", this, "Remove", ":/Items/"));
    //classAction->setMenu(classMenu);
    //functionsMenu->addAction(classAction);
}


/**
 * @brief ToolbarWidget::makeConnections
 * Connect signals and slots.
 */
void ToolbarWidget::makeConnections()
{
    connect(connectButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(alignVerticallyButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(alignHorizontallyButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(expandButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(contractButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(popupNewWindow, SIGNAL(clicked()), this, SLOT(hide()));
    connect(connectionsButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(exportSnippetButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(importSnippetButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(definitionButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(implementationButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(allNodes, SIGNAL(clicked()), hardwareClusterViewMenu, SLOT(hide()));
    connect(connectedNodes, SIGNAL(clicked()), hardwareClusterViewMenu, SLOT(hide()));
    connect(unconnectedNodes, SIGNAL(clicked()), hardwareClusterViewMenu, SLOT(hide()));

    connect(connectButton, SIGNAL(clicked()), nodeView, SLOT(setStateConnect()));
    connect(deleteButton, SIGNAL(clicked()), nodeView, SLOT(deleteSelection()));
    connect(alignVerticallyButton, SIGNAL(clicked()), nodeView, SLOT(alignSelectionVertically()));
    connect(alignHorizontallyButton, SIGNAL(clicked()), nodeView, SLOT(alignSelectionHorizontally()));
    connect(connectionsButton, SIGNAL(clicked()), nodeView, SLOT(showConnectedNodes()));
    connect(exportSnippetButton, SIGNAL(clicked()), nodeView, SLOT(exportSnippet()));
    connect(importSnippetButton, SIGNAL(clicked()), nodeView, SLOT(request_ImportSnippet()));
    connect(expandButton, SIGNAL(clicked()), this, SLOT(expandContractNodes()));
    connect(contractButton, SIGNAL(clicked()), this, SLOT(expandContractNodes()));
    connect(popupNewWindow, SIGNAL(clicked()), this, SLOT(constructNewView()));
    connect(definitionButton, SIGNAL(clicked()), this, SLOT(displayConnectedNode()));
    connect(implementationButton, SIGNAL(clicked()), this, SLOT(displayConnectedNode()));
    connect(allNodes, SIGNAL(clicked()), this, SLOT(updateDisplayedChildren()));
    connect(connectedNodes, SIGNAL(clicked()), this, SLOT(updateDisplayedChildren()));
    connect(unconnectedNodes, SIGNAL(clicked()), this, SLOT(updateDisplayedChildren()));

    connect(addMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(addChildNode(ToolbarMenuAction*)));

    connect(connectMenu, SIGNAL(aboutToShow()), this, SLOT(setupLegalNodesList()));
    connect(connectMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(connectNodes(ToolbarMenuAction*)));

    connect(hardwareMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(connectNodes(ToolbarMenuAction*)));

    connect(definitionMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(displayConnectedNode(ToolbarMenuAction*)));
    connect(implementationMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(displayConnectedNode(ToolbarMenuAction*)));
    connect(instancesMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(displayConnectedNode(ToolbarMenuAction*)));

    connect(componentImplAction, SIGNAL(hovered()), this, SLOT(setupComponentList()));
    connect(componentInstAction, SIGNAL(hovered()), this, SLOT(setupComponentList()));
    connect(inEventPortDelegateAction, SIGNAL(hovered()), this, SLOT(setupEventPortInstanceList()));
    connect(outEventPortDelegateAction, SIGNAL(hovered()), this, SLOT(setupEventPortInstanceList()));
    connect(blackBoxInstAction, SIGNAL(hovered()), this, SLOT(setupBlackBoxList()));
    connect(aggregateInstAction, SIGNAL(hovered()), this, SLOT(setupAggregateList()));
    connect(vectorInstAction, SIGNAL(hovered()), this, SLOT(setupVectorList()));

    connect(componentImplMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(addConnectedNode(ToolbarMenuAction*)));
    connect(componentInstMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(addConnectedNode(ToolbarMenuAction*)));
    connect(inEventPortDelegateMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(addConnectedNode(ToolbarMenuAction*)));
    connect(outEventPortDelegateMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(addConnectedNode(ToolbarMenuAction*)));
    connect(blackBoxInstMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(addConnectedNode(ToolbarMenuAction*)));
    connect(aggregateInstMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(addConnectedNode(ToolbarMenuAction*)));
    connect(vectorInstMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(addConnectedNode(ToolbarMenuAction*)));
}


/**
 * @brief ToolbarWidget::updateButtonsAndMenus
 * Show/hide tool buttons for the currently selected nodeItem(s) and update the menu lists.
 * @param nodeItems
 */
void ToolbarWidget::updateButtonsAndMenus(QList<NodeItem*> nodeItems)
{
    if (nodeItems.isEmpty()) {
        return;
    }

    QList<NodeItem*> legalNodes;
    bool setupLegalNodes = false;
    bool deployable = true;

    // need to clear menus before updating them
    clearMenus();

    if (nodeItems.count() == 1) {

        /*
         * Single NodeItem selected
         */

        nodeItem = nodeItems.at(0);
        if (!nodeItem) {
            return;
        }

        EntityItem* entityItem = 0;
        if (nodeItem->isEntityItem()) {
            entityItem = (EntityItem*)nodeItem;
        }

        // these buttons are only available for a single selection
        popupNewWindow->show();
        definitionButton->setVisible(showDefinitionToolButton);
        implementationButton->setVisible(showImplementationToolButton);
        goToButtonsVisible = showDefinitionToolButton || showImplementationToolButton;

        // the expand and contract buttons are only available for multiple selection
        expandContractButtonsVisible = false;

        // check if the selected node item has other node items connected to it (edges)
        if (nodeItem->getNode()->getEdges(0).count() > 0) {
            connectionsButton->show();
        }

        // only show the displayed children option button if the selected item is a HardwareCluster
        if (entityItem) {
            if (entityItem->isHardwareCluster()) {
                hardwareClusterMenuClicked(entityItem->getHardwareClusterChildrenViewMode());
                displayedChildrenOptionButton->show();
            } else if (!entityItem->isHardwareNode()) {
                setupLegalNodes = true;
            }
        }

        // setup selected item's adoptable nodes menu and sub-menus, and its instances menu
        setupAdoptableNodesList(nodeView->getAdoptableNodeList(nodeItem->getID()));
        setupInstancesList(nodeView->getNodeInstances(nodeItem->getID()));

        legalNodes = nodeView->getConnectableNodeItems(nodeItem->getID());
        deployable = nodeView->isNodeKindDeployable(nodeItem->getNodeKind());
        alterViewButtonsVisible = true;

    } else {

        /*
         * Multiple NodeItems selected
         */

        NodeItem* prevParentItem = 0;
        bool shareParent = true;
        bool allClusters = true;
        bool canBeExpanded = false;
        int viewMode = -1;

        for (int i = 0; i < nodeItems.count(); i++) {

            NodeItem* item_i = nodeItems.at(i);
            NodeItem* parentItem = item_i->getParentNodeItem();

            // check if all the selected items have a shared parent item
            if (prevParentItem && (prevParentItem != parentItem)) {
                shareParent = false;
            }

            prevParentItem = parentItem;

            // if it's not an EntityItem, skip the following checks
            if (!item_i->isEntityItem()){
                deployable = false;
                allClusters = false;
                continue;
            }

            EntityItem* entityItem = (EntityItem*)item_i;

            // only clear the legal nodes list if all selected items are hardware
            if (!setupLegalNodes && (!entityItem->isHardwareNode() || !entityItem->isHardwareCluster())) {
                setupLegalNodes = true;
            }

            // check if all the selected items are HardwareClusters
            if (entityItem->isHardwareCluster()) {
                int currentViewMode = entityItem->getHardwareClusterChildrenViewMode();
                if (viewMode == -1) {
                    // viewMode hasn't been set yet; set it
                    viewMode = currentViewMode;
                } else if (currentViewMode != viewMode) {
                    // if currentMode != viewMode, we don't have a shared viewMode
                    viewMode = -2;
                }
            } else {
                allClusters = false;
            }

            // check if at least one of the selected items can be expanded
            if (!canBeExpanded && entityItem->hasChildren()) {
                canBeExpanded = true;
            }

            // check if all  theselected items are deployable
            if (deployable && !nodeView->isNodeKindDeployable(item_i->getNodeKind())) {
                deployable = false;
            }

            // this allows multiple selection to connect to a shared legal node
            // check if there is any item in item_i's legal nodes list that can connect to all the other items
            foreach (NodeItem* legalNode, nodeView->getConnectableNodeItems(item_i->getID())) {
                bool appendToList = true;
                for (int j = 0; j < nodeItems.count(); j++) {
                    NodeItem* item_j = nodeItems.at(j);
                    Node* itemNode = item_j->getNode();
                    if (!item_j->isEntityItem() || !itemNode->canConnect(legalNode->getNode())) {
                        appendToList = false;
                        break;
                    }
                }
                if (appendToList && !legalNodes.contains(legalNode)) {
                    legalNodes.append(legalNode);
                }
            }
        }

        // these buttons are only available for multiple selected entities
        expandButton->setVisible(canBeExpanded);
        contractButton->setVisible(canBeExpanded);
        expandContractButtonsVisible = canBeExpanded;

        // only show the group alignment buttons if all the selected items have the same parent
        if (shareParent) {
            alignVerticallyButton->show();
            alignHorizontallyButton->show();
            alignButtonsVisible = true;
        }

        // if all selected node items are Hardware clusters, show diplay option button
        if (allClusters) {
            hardwareClusterMenuClicked(viewMode);
            displayedChildrenOptionButton->show();
            alterViewButtonsVisible = true;
        }
    }

    // setup the hardware menu for deployable node items
    if (deployable) {
        setupHardwareList();
    }

    // setup connectable nodes menu for the selected item(s)
    if (setupLegalNodes) {
        setupLegalNodesList(legalNodes);
    }
}


/**
 * @brief ToolbarWidget::updateSeparators
 */
void ToolbarWidget::updateSeparators()
{
    alignFrame->setVisible(alignButtonsVisible && alterModelButtonsVisible);
    snippetFrame->setVisible(snippetButtonsVisible && (alterModelButtonsVisible || alignButtonsVisible));
    goToFrame->setVisible(goToButtonsVisible && (alterModelButtonsVisible || snippetButtonsVisible));
    alterViewFrame->setVisible(alterViewButtonsVisible && (alterModelButtonsVisible || alignButtonsVisible || snippetButtonsVisible || goToButtonsVisible));
    expandContractFrame->setVisible(expandContractButtonsVisible && (alterModelButtonsVisible || alignButtonsVisible));
}


/**
 * @brief ToolbarWidget::hideAllButtons
 */
void ToolbarWidget::hideButtons()
{
    addChildButton->hide();
    connectButton->hide();
    hardwareButton->hide();
    deleteButton->hide();
    alignVerticallyButton->hide();
    alignHorizontallyButton->hide();
    expandButton->hide();
    contractButton->hide();
    exportSnippetButton->hide();
    importSnippetButton->hide();
    definitionButton->hide();
    implementationButton->hide();
    instancesButton->hide();
    connectionsButton->hide();
    popupNewWindow->hide();
    displayedChildrenOptionButton->hide();
}


/**
 * @brief ToolbarWidget::hideSeparators
 */
void ToolbarWidget::hideSeparators()
{
    snippetFrame->hide();
    goToFrame->hide();
    alterViewFrame->hide();
    alignFrame->hide();
    expandContractFrame->hide();
}


/**
 * @brief ToolbarWidget::clearMenus
 * Clear all of the stored menus.
 */
void ToolbarWidget::clearMenus()
{
    addMenu->clearMenu();
    connectMenu->clearMenu();
    hardwareMenu->clearMenu();
    instancesMenu->clearMenu();
    componentImplMenu->clearMenu();
    componentInstMenu->clearMenu();
    inEventPortDelegateMenu->clearMenu();
    outEventPortDelegateMenu->clearMenu();
    blackBoxInstMenu->clearMenu();
    aggregateInstMenu->clearMenu();
    vectorInstMenu->clearMenu();

    // this list needs to be cleared as well - used to populate the connect menu
    legalNodeItems.clear();
}


/**
 * @brief ToolbarWidget::resetButtonGroupFlags
 */
void ToolbarWidget::resetButtonGroupFlags()
{
    alterModelButtonsVisible = false;
    alignButtonsVisible = false;
    expandContractButtonsVisible = false;
    snippetButtonsVisible = false;
    goToButtonsVisible = false;
    alterViewButtonsVisible = false;

    blackBoxMenuDone = false;
    componentImplMenuDone = false;
    componentInstMenuDone = false;
    inEventPortInstanceMenuDone = false;
    outEventPortInstanceMenuDone = false;
    connectMenuDone = false;
    aggregateMenuDone = false;
    vectorMenuDone = false;
    hardwareMenuDone = false;

    chosenInstanceID = -1;
}


/**
 * @brief ToolbarWidget::setupAdoptableNodesList
 * This sets up the menu list of adoptable node kinds for the selected node item.
 * @param nodeKinds
 */
void ToolbarWidget::setupAdoptableNodesList(QStringList nodeKinds)
{
    // if there are no adoptable node kinds, hide the addChild button
    if (nodeKinds.isEmpty()) {
        return;
    } else {
        addChildButton->show();
        alterModelButtonsVisible = true;
    }

    foreach (QString kind, nodeKinds) {

        ToolbarMenuAction* action = 0;

        if (kind == "ComponentImpl") {
            action = componentImplAction;
        } else if (kind == "ComponentInstance") {
            action = componentInstAction;
        } else if (kind == "InEventPortDelegate") {
            action = inEventPortDelegateAction;
        } else if (kind == "OutEventPortDelegate") {
            action = outEventPortDelegateAction;
        } else if (kind == "BlackBoxInstance") {
            action = blackBoxInstAction;
        } else if (kind == "AggregateInstance") {
            action = aggregateInstAction;
        } else if (kind == "VectorInstance") {
            action = vectorInstAction;
        } else if (kind == "Process") {
            action = processAction;
        } else {
            action  = new ToolbarMenuAction(kind, this);
        }

        addMenu->addAction(action);
    }
}


/**
 * @brief ToolbarWidget::setupLegalNodesList
 * This sets up the menu list of nodes the selected node item can connect to.
 * @param nodeList
 */
void ToolbarWidget::setupLegalNodesList(QList<NodeItem*> nodeList)
{
    // skip HardwareClusters and HardwareNodes - they will be listed in the hardwareMenu
    foreach (NodeItem* item, nodeList) {
        if (item->getNodeKind().startsWith("Hardware")) {
            nodeList.removeAll(item);
        }
    }

    // if the selected node can't connect to anything, hide the connect button
    if (nodeList.isEmpty()) {
        return;
    }

    connectButton->show();
    alterModelButtonsVisible = true;
    legalNodeItems = nodeList;
}


/**
 * @brief ToolbarWidget::setupInstancesList
 * This sets up the menu list of instances of the selected node item.
 */
void ToolbarWidget::setupInstancesList(QList<NodeItem*> instances)
{
    // if there are no instances, hide the instance button
    if (instances.isEmpty()) {
        return;
    } else {
        instancesButton->show();
        goToButtonsVisible = true;
    }

    foreach (NodeItem* instance, instances) {
        // construct a display option menu
        ToolbarMenu* menu = new ToolbarMenu(this);
        menu->addAction(new ToolbarMenuAction("Goto", instancesMenu, "Go to Instance", ":/Actions/"));
        menu->addAction(new ToolbarMenuAction("Popup", instancesMenu, "Popup Instance", ":/Actions/"));

        // setup an action for each of the selected entity's instances and attach the display option menu
        ToolbarMenuAction* action = new ToolbarMenuAction(instance, this);
        connect(action, SIGNAL(hovered()), this, SLOT(setInstanceID()));
        action->setMenu(menu);
        instancesMenu->addAction(action);
    }
}


/**
 * @brief ToolbarWidget::setupComponentList
 * @param kind
 */
void ToolbarWidget::setupComponentList(QString actionKind)
{
    ToolbarMenu* actionMenu = 0;

    if (actionKind == "impl") {
        if (componentImplMenuDone) {
            return;
        } else {
            componentImplMenuDone = true;
            actionMenu = componentImplMenu;
        }
    } else if (actionKind == "inst") {
        if (componentInstMenuDone) {
            return;
        } else {
            componentInstMenuDone = true;
            actionMenu = componentInstMenu;
        }
    } else {
        qWarning() << "ToolbarWidget::setupCompList - Action kind not handled.";
        return;
    }

    QList<NodeItem*> components = nodeView->getEntityItemsOfKind("Component");

    foreach (NodeItem* component, components) {
        // if selected node is the BehaviourDefinitions or an undefined ComponentImpl,
        // don't include already implemented Components in the menu
        if (actionKind == "impl" && nodeView->getImplementation(component->getID())) {
            continue;
        }
        // set up an action for each Component definition
        constructSubMenuAction(component, actionMenu);
    }
}


/**
 * @brief ToolbarWidget::setupEventPortInstanceList
 * @param eventPortKind
 */
void ToolbarWidget::setupEventPortInstanceList(QString eventPortKind)
{
    ToolbarMenu* actionMenu = 0;

    if (eventPortKind == "InEventPortInstance") {
        if (inEventPortInstanceMenuDone) {
            return;
        } else {
            inEventPortInstanceMenuDone = true;
            actionMenu = inEventPortDelegateMenu;
        }
    } else if (eventPortKind == "OutEventPortInstance") {
        if (outEventPortInstanceMenuDone) {
            return;
        } else {
            outEventPortInstanceMenuDone = true;
            actionMenu = outEventPortDelegateMenu;
        }
    } else {
        qWarning() << "ToolbarWidget::setupEventPortInstanceList - Event port kind not handled.";
        return;
    }

    // setup combined list of both Component and BlackBox instances for the In/Out EventPortDelegate menus
    QList<NodeItem*> definitionInstances = nodeView->getEntityItemsOfKind("ComponentInstance", nodeItem->getID(), 0);
    definitionInstances.append(nodeView->getEntityItemsOfKind("BlackBoxInstance", nodeItem->getID(), 0));

    foreach (NodeItem* defnInst, definitionInstances) {

        QList<NodeItem*> epInstances = nodeView->getEntityItemsOfKind(eventPortKind, defnInst->getID());

        foreach (NodeItem* epInst, epInstances) {
            // can only connect an EventPort Delegate to an EventPort connected to an Aggregate
            NodeItem* epDefn = nodeView->getDefinition(epInst->getID());
            if (epDefn && !nodeView->getAggregate(epDefn->getID())) {
                continue;
            }
            // set up an action for each In/Out EventPortInstance
            constructSubMenuAction(epInst, actionMenu);
        }
    }
}


/**
 * @brief ToolbarWidget::constructToolButton
 * This constructs a QToolButton with the provided size, icon and tooltip.
 * It adds it to the toolbar's layout and then returns it.
 * @param size - button size
 * @param iconSizeRatio - button icon's size ratio
 * @param iconPng - button icon's png filename without the extension
 * @param tooltip - button tooltip
 * @param iconPath - button icon's png folder
 * @return
 */
QToolButton* ToolbarWidget::constructToolButton(QSize size, double iconSizeRatio, QString iconPng, QString tooltip, QString iconPath)
{
    QToolButton* button = new QToolButton(this);
    button->setFixedSize(size);
    button->setIcon(nodeView->getImage(iconPath, iconPng));
    button->setIconSize(size * iconSizeRatio);
    button->setToolTip(tooltip);
    toolbarLayout->addWidget(button);
    return button;
}


/**
 * @brief ToolbarWidget::constructFrameSeparator
 * This constructs a QFrame used as a separator, adds it to the toolbar's layout and then returns it.
 * @return
 */
QFrame* ToolbarWidget::constructFrameSeparator()
{
    QFrame* frame = new QFrame(this);
    frame->setFrameShape(QFrame::VLine);
    frame->setPalette(QPalette(Qt::darkGray));
    toolbarLayout->addWidget(frame);
    return frame;
}


/**
 * @brief ToolbarWidget::constructToolButtonMenu
 * The connections in here don't need to be applied to all the toolbar menus.
 * Only the top level menus - the ones directly attached to a QToolButton.
 * @param parentButton
 * @return
 */
ToolbarMenu* ToolbarWidget::constructToolButtonMenu(QToolButton* parentButton, bool instantPopup)
{
    if (parentButton) {
        ToolbarMenu* menu = new ToolbarMenu(this);
        connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(hideToolbar(QAction*)));
        connect(menu, SIGNAL(aboutToHide()), this, SLOT(hideToolbar()));
        if (instantPopup) {
            parentButton->setPopupMode(QToolButton::InstantPopup);
        } else {
            parentButton->setPopupMode(QToolButton::MenuButtonPopup);
            parentButton->setFixedWidth(55);
        }
        parentButton->setMenu(menu);
        return menu;
    } else {
        qWarning() << "ToolbarWidget::constructToolButtonMenu - Parent tool button is null.";
        return 0;
    }
}


/**
 * @brief ToolbarWidget::constructMenuAction
 * This method constructs an action for nodeItem and directly adds it to parentMenu.
 * @param nodeItem
 * @param parentMenu
 * @return - the newly constructed action that's been added to parentMenu
 */
ToolbarMenuAction* ToolbarWidget::constructMenuAction(NodeItem* nodeItem, ToolbarMenu* parentMenu)
{
    if (!nodeItem || !parentMenu) {
        qWarning() << "ToolbarWidget::constructMenuAction - Action not constructed.";
        return 0;
    }

    // construct and add nodeItem's action to the parent action's menu
    ToolbarMenuAction* action = new ToolbarMenuAction(nodeItem, this);
    parentMenu->addAction(action);
    return action;
}



/**
 * @brief ToolbarWidget::constructSubMenuAction
 * This method constructs an action for nodeItem, finds its parent action in
 * parentMenu and adds it to its parent action's menu. It constructs the parent
 * action if there isn't already one. The parent action is then added to parentMenu.
 * @param nodeItem
 * @param parentMenu
 * @return - the newly constructed action that's been added to parentMenu
 */
ToolbarMenuAction* ToolbarWidget::constructSubMenuAction(NodeItem* nodeItem, ToolbarMenu* parentMenu)
{
    if (!nodeItem || !parentMenu) {
        qWarning() << "ToolbarWidget::constructSubMenuAction - Action not constructed.";
        return 0;
    }

    NodeItem* parentNodeItem = nodeItem->getParentNodeItem();

    // if nodeItem has a parent, check if there is already an action for it in parentMenu
    // if not, create one and attach a menu to it
    if (parentNodeItem && !parentNodeItem->isModelItem()) {

        ToolbarMenuAction* parentAction = 0;
        ToolbarMenu* parentActionMenu = 0;

        foreach (ToolbarMenuAction* parentNodeAction, parentMenu->getActions(parentNodeItem)) {
            if (parentNodeAction->menu()) {
                parentAction = parentNodeAction;
                break;
            }
        }

        if (parentAction) {
            parentActionMenu = qobject_cast<ToolbarMenu*>(parentAction->menu());
        } else {
            parentAction = new ToolbarMenuAction(parentNodeItem, this);
            parentActionMenu = new ToolbarMenu(this);
            parentAction->setMenu(parentActionMenu);
            parentMenu->addAction(parentAction);
        }

        // construct and add nodeItem's action to the parent action's menu
        if (parentActionMenu) {
            ToolbarMenuAction* action = new ToolbarMenuAction(nodeItem, this);
            parentActionMenu->addAction(action);
            return action;
        }

        /*
        ToolbarMenuAction* parentAction = parentMenu->getAction(parentNodeItem);
        ToolbarMenu* parentActionMenu = 0;

        if (parentAction) {
            parentActionMenu = qobject_cast<ToolbarMenu*>(parentAction->menu());
        } else {
            parentAction = new ToolbarMenuAction(parentNodeItem, this);
            parentActionMenu = new ToolbarMenu(this);
            parentAction->setMenu(parentActionMenu);
            parentMenu->addAction(parentAction);
        }

        // construct and add nodeItem's action to the parent action's menu
        if (parentActionMenu) {
            ToolbarMenuAction* action = new ToolbarMenuAction(nodeItem, this);
            parentActionMenu->addAction(action);
            return action;
        }
        */
    }

    return 0;
}


/**
 * @brief ToolbarWidget::closeOpenMenus
 */
void ToolbarWidget::closeOpenMenus()
{
    foreach (ToolbarMenu* menu, openMenus) {
        menu->close();
    }
}

