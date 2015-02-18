#include "docknodeitem.h"
#include "dockscrollarea.h"

#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
/**
 * @brief DockNodeItem::DockNodeItem
 * @param parent
 */
DockNodeItem::DockNodeItem(NodeItem *node_item, QWidget* parent) :
    QPushButton(parent)
{
    selectedNode = 0;
    nodeItem = node_item;
    kind = nodeItem->getGraphML()->getDataValue("kind");
    label = nodeItem->getGraphML()->getDataValue("label");
    image = new QImage(":/Resources/Icons/" + kind + ".png");
    isSelected = false;

    setFlat(true);
    setFixedSize(100, 100);
    setStyleSheet("padding: 0px;");

    textLabel = new QLabel(label, this);
    QLabel* imageLabel = new QLabel(this);
    QVBoxLayout* vLayout = new QVBoxLayout();
    QImage scaledImage = image->scaled(width(),
                                       height()-textLabel->height(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);
    double offSet = 2.5;

    vLayout->setMargin(0);
    imageLabel->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setFixedSize(width()+offSet, 40);
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setPixmap(QPixmap::fromImage(scaledImage));
    imageLabel->setFixedSize(width(), height()-25);

    vLayout->addWidget(imageLabel);
    vLayout->addStretch();
    vLayout->addWidget(textLabel);
    setLayout(vLayout);

    if (kind == "HardwareNode") {
        selectedColor = "rgba(90, 150, 200, 100);";
    } else if (kind == "Component") {
        selectedColor = "rgba(210, 105, 30, 100);";
    }
}


/**
 * @brief DockNodeItem::paintEvent
 * Change the node image (button icon) depending on the node kind.
 * NOTE: setIcon() calls this and the scroll area's paintEvent() infinitely.
 * @param e
 */
void DockNodeItem::paintEvent(QPaintEvent *e)
{
    if (isSelected) {
        color = selectedColor;
    } else {
        color = "rgba(0,0,0,0);";
    }

    /*
    setStyleSheet("QPushButton{"
                  "background-color:" + color +
                  "border-radius: 10px;"
                  "}");
    */

    // this overrides the call above so when the item is selected, you can
    // only see the change in background color when you hover over it
    setStyleSheet("QPushButton:hover{"
                  "background-color: rgba(0,0,0,0);"
                  "border: 1px solid black;"
                  "border-radius: 5px;"
                  "}");

    QPushButton::paintEvent(e);
}


/**
 * @brief DockNodeItem::buttonPressed
 * Get the selected node and depending on its kind, either try to add a ComponentInstance
 * inside the ComponentAssembly or connect the ComponentInstance to this dock item's Component.
 *
 */
void DockNodeItem::buttonPressed()
{
    emit getSelectedNode();

    if (selectedNode) {
        QString nodeKind = selectedNode->getDataValue("kind");
        if (nodeKind == "ComponentAssembly") {
            emit dockNode_addComponentInstance(selectedNode, nodeItem->getNode());
        } else if (nodeKind == "ComponentInstance") {
            emit dockNode_connectComponentInstance(selectedNode, nodeItem->getNode());
        }
    }
}


/**
 * @brief DockNodeItem::updateData
 * This gets called when the dataTable value for the node item has been changed.
 */
void DockNodeItem::updateData()
{
    label = nodeItem->getGraphML()->getDataValue("label");
    textLabel->setText(label);
    repaint();
}


/**
 * @brief DockNodeItem::deleteLater
 */
void DockNodeItem::deleteLater()
{
    emit removeFromDockNodeList(this);
    /*

    if (parentContainer) {
        parentContainer->checkDockNodesList();
    }
    */

    QObject::deleteLater();
}


/**
 * @brief DockNodeItem::getNodeItem
 * Returns the NodeItem this item is conneted to.
 * @return nodeItem
 */
NodeItem *DockNodeItem::getNodeItem()
{
    return nodeItem;
}


/**
 * @brief DockNodeItem::connectToNodeItem
 */
void DockNodeItem::connectToNodeItem()
{
    connect(this, SIGNAL(clicked()), this , SLOT(buttonPressed()));
    connect(nodeItem, SIGNAL(updateDockNodeItem()), this, SLOT(updateData()));
    connect(nodeItem, SIGNAL(updateOpacity(qreal)), this, SLOT(setOpacity(qreal)));
    connect(nodeItem, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}


/**
 * @brief DockNodeItem::setContainer
 * @param container
 */
void DockNodeItem::setContainer(DockScrollArea *container)
{
    parentContainer = container;
}


/**
 * @brief DockNodeItem::setSelected
 * This gets called everytime a Component Definition or
 * Hardware Node has been selected/deselected.
 * @param _selected
 */
void DockNodeItem::setSelected(bool selected)
{
    isSelected = selected;
    repaint();
}


/**
 * @brief DockNodeItem::setOpacity
 * This disables this button when SHIFT is held down and the currently
 * selected node can't be connected to this button.
 * @param opacity
 */
void DockNodeItem::setOpacity(double opacity)
{
    if (opacity < 1) {
        setEnabled(false);
    } else {
        setEnabled(true);
    }
    repaint();
}


/**
 * @brief DockNodeItem::setSelectedNode
 * @param node
 */
void DockNodeItem::setSelectedNode(Node *node)
{
    selectedNode = node;
}
