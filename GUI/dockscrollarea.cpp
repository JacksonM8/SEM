#include "dockscrollarea.h"
#include "docktogglebutton.h"
#include "docknodeitem.h"
#include "dockadoptablenodeitem.h"

#include <QFontMetrics>
#include <QDebug>


/**
 * @brief DockScrollArea::DockScrollArea
 * @param title
 * @param parent
 */
DockScrollArea::DockScrollArea(QString label, DockToggleButton *parent) :
    QScrollArea(parent)
{
    layout = new QVBoxLayout(this);
    groupBox = new QGroupBox(0);
    parentButton = parent;
    this->label = label;
    activated = false;

    // attach scroll area and groupbox to their parent button
    parentButton->setContainer(this);

    groupBox->setLayout(layout);
    //groupBox->setTitle(label);
    groupBox->setFixedSize(this->width(), this->height());
    groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    groupBox->setStyleSheet("QGroupBox {"
                            "background-color: rgba(255,255,255,0);"
                            "border: 0px;"
                            "padding: 10px;"
                            "}");

    // find a better way to position the elements in the centre of the layout
    layout->setAlignment(Qt::AlignHCenter);
    layout->setSpacing(5);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    setWidget(groupBox);
    setVisible(false);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("QScrollArea {"
                  "background-color: rgba(255,255,255,180);"
                  "border: 0px;"
                  "border-radius: 10px;"
                  "}");

    // this centers the dock node items but it moves the groupbox title around
    // find a way to keep the groupbox title on the top left
    setAlignment(Qt::AlignHCenter);

    connect(parentButton, SIGNAL(pressed()), this, SLOT(activate()));
}


/**
 * @brief DockScrollArea::~DockScrollArea
 */
DockScrollArea::~DockScrollArea()
{

}


/**
 * @brief DockScrollArea::getParentButton
 * Returns this scroll area's parent button (DockToggleButton)
 * @return
 */
DockToggleButton *DockScrollArea::getParentButton()
{
    return parentButton;
}


/**
 * @brief DockScrollArea::getGroupBox
 * @return
 */
QGroupBox *DockScrollArea::getGroupBox()
{
    return groupBox;
}


/**
 * @brief DockScrollArea::addNode
 * @param buttonName
 * @param nodeName
 */
void DockScrollArea::addDockNode(NodeItem* item)
{
    DockNodeItem *itm = new DockNodeItem(item, this);
    dockNodes.append(itm);
    layout->addWidget(itm);
    connect(itm, SIGNAL(dockNode_addComponentInstance(NodeItem*)),
            this, SLOT(dock_addComponentInstance(NodeItem*)));
}


/**
 * @brief DockScrollArea::addNodes
 * Add adoptable node items to dock.
 * This creates a groupbox for each adoptable node kind
 * which contains the new dock node item and its label.
 * @param nodes
 */
void DockScrollArea::addAdoptableDockNodes(QStringList nodes)
{
    clear();
    nodes.sort();

    for (int i=0; i<nodes.count(); i++) {
        DockAdoptableNodeItem *itm = new DockAdoptableNodeItem(nodes.at(i), this);
        layout->addWidget(itm);
        dockNodes.append(itm);
        connect(itm, SIGNAL(itemPressed(QString)), this, SLOT(buttonPressed(QString)));
    }

    repaint();
}


/**
 * @brief DockScrollArea::paintEvent
 * @param e
 */
void DockScrollArea::paintEvent(QPaintEvent *e)
{
    QScrollArea::paintEvent(e);
}


/**
 * @brief DockScrollArea::buttonPressed
 * @param kind
 */
void DockScrollArea::buttonPressed(QString kind)
{
    emit constructDockNode(kind);
}


/**
 * @brief DockScrollArea::dock_addComponentInstance
 * Tell the view to create and add a ComponentInstance of
 * NodeItem itm into the currently selected node.
 */
void DockScrollArea::dock_addComponentInstance(NodeItem *itm)
{
    emit trigger_addComponentInstance(itm);
}


/**
 * @brief DockScrollArea::activate
 * This shows or hides the scroll area its groupbox.
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
    for (int i=0; i<dockNodes.count(); i++) {
        layout->removeWidget(dockNodes.at(i));
        delete dockNodes.at(i);
    }

    dockNodes.clear();
}
