#include "searchitem.h"
#include "../medeawindow.h"

#include <QDebug>

#define MIN_WIDTH 500.0
#define MIN_HEIGHT 50.0

#define BUTTON_SIZE 28
#define KEY_LABEL_WIDTH 100

#define LAYOUT_MARGIN 2
#define LAYOUT_SPACING 5
#define MARGIN_OFFSET (LAYOUT_MARGIN + LAYOUT_SPACING)

#define LABEL_RATIO (2.0 / 5.0)
#define ICON_RATIO 0.8
#define ICON_SIZE (MIN_HEIGHT * ICON_RATIO - MARGIN_OFFSET)

#define CLICK_TO_CENTER true
#define DOUBLE_CLICK_TO_EXPAND false


/**
 * @brief SearchItem::SearchItem
 * @param parent
 */
SearchItem::SearchItem(GraphMLItem *item, QWidget *parent) : QLabel(parent)
{
    if (item) {
        graphMLItem = item;
        graphMLItemID = graphMLItem->getID();
    } else {
        qWarning() << "SearchItem:: Cannot create a search item widget with a NULL graphMLItem.";
        return;
    }

    selected = false;
    expanded = true;
    valuesSet = false;

    setupLayout();
    updateColor();
    expandItem();

    if (DOUBLE_CLICK_TO_EXPAND) {
        connect(expandButton, SIGNAL(clicked()), this, SIGNAL(searchItem_clicked()));
    }
    connect(centerOnButton, SIGNAL(clicked()), this, SIGNAL(searchItem_clicked()));
    connect(expandButton, SIGNAL(clicked()), this, SLOT(expandItem()));
    connect(centerOnButton, SIGNAL(clicked()), this, SLOT(centerOnItem()));
}


/**
 * @brief SearchItemButton::connectToWindow
 * This connects the signals and slots to the MEDEA window.
 */
void SearchItem::connectToWindow(QMainWindow* window)
{
    MedeaWindow* medea = dynamic_cast<MedeaWindow*>(window);
    if (medea) {
        connect(this, SIGNAL(searchItem_clicked()), medea, SLOT(searchItemClicked()));
        connect(medea, SIGNAL(window_searchItemClicked(SearchItem*)), this, SLOT(itemClicked(SearchItem*)));
        connect(this, SIGNAL(searchItem_centerOnItem(int)), medea, SLOT(on_searchResultItem_clicked(int)));
    }
}


/**
 * @brief SearchItem::itemClicked
 * @param item
 */
void SearchItem::itemClicked(SearchItem *item)
{
    bool itemSelected = item == this;
    if (itemSelected != selected) {
        selected = itemSelected;
        updateColor();
    }
    if (selected) {
        centerOnItem();
    }
}


/**
 * @brief SearchItem::expandItem
 */
void SearchItem::expandItem()
{
    expanded = !expanded;
    if (expanded && !valuesSet) {
        getDataValues();
        valuesSet = true;
    }
    dataBox->setVisible(expanded);
    if (expanded) {
        setFixedHeight(MIN_HEIGHT + dataBox->sizeHint().height());
        expandButton->setIcon(QIcon(contractPixmap));
    } else {
        setFixedHeight(MIN_HEIGHT);
        expandButton->setIcon(QIcon(expandPixmap));
    }
}


/**
 * @brief SearchItem::centerOnItem
 */
void SearchItem::centerOnItem()
{
    emit searchItem_centerOnItem(graphMLItemID);
}


/**
 * @brief SearchItem::mouseReleaseEvent
 * @param event
 */
void SearchItem::mouseReleaseEvent(QMouseEvent* event)
{
    if (CLICK_TO_CENTER) {
        if (event->button() == Qt::LeftButton) {
            emit searchItem_clicked();
        }
    } else {
        QLabel::mouseReleaseEvent(event);
    }
}


/**
 * @brief SearchItem::mouseDoubleClickEvent
 * @param event
 */
void SearchItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (DOUBLE_CLICK_TO_EXPAND) {
        if (event->button() == Qt::LeftButton) {
            expandItem();
        }
    } else {
        QLabel::mouseDoubleClickEvent(event);
    }
}


/**
 * @brief SearchItem::setupLayout
 * This sets up the layout and widgets within this button.
 */
void SearchItem::setupLayout()
{
    fixedStyleSheet = "QPushButton{"
                      "background-color: rgba(250,250,250,250);"
                      "border-radius: 5px;"
                      "border: 1px solid darkGray;"
                      "}"
                      "QPushButton:hover{"
                      "background-color: rgba(255,255,255,255);"
                      "border: 2px solid rgb(150,150,150);"
                      "}";

    QVBoxLayout* mainLayout = new QVBoxLayout();
    QHBoxLayout* layout = new QHBoxLayout();

    // setup icon label
    QString graphMLKind = graphMLItem->getGraphML()->getDataValue("kind");
    QPixmap pixmap(":/Items/" + graphMLKind + ".png");
    pixmap = pixmap.scaled(ICON_SIZE, ICON_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    iconLabel = new QLabel(this);
    iconLabel->setFixedSize(MIN_HEIGHT - MARGIN_OFFSET, MIN_HEIGHT - MARGIN_OFFSET);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setPixmap(pixmap);

    // setup entity label
    QString graphMLLabel = graphMLItem->getGraphML()->getDataValue("label");
    entityLabel = new QLabel(this);
    entityLabel->setFixedSize(MIN_WIDTH * LABEL_RATIO, iconLabel->height());
    entityLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    entityLabel->setText(graphMLLabel);

    // setup location label
    locationLabel = new QLabel(this);
    locationLabel->setMinimumWidth(MIN_WIDTH - entityLabel->width());
    locationLabel->setFixedHeight(iconLabel->height());
    locationLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    locationLabel->setText(getItemLocation());

    expandPixmap = QPixmap(":/Actions/Arrow_Down");
    contractPixmap = QPixmap(":/Actions/Arrow_Up");
    expandButton = new QPushButton(QIcon(expandPixmap), "", this);
    centerOnButton = new QPushButton(QIcon(":/Actions/Crosshair"), "", this);

    QSize buttonSize(BUTTON_SIZE, BUTTON_SIZE);
    expandButton->setFixedSize(buttonSize);
    centerOnButton->setFixedSize(buttonSize);
    expandButton->setIconSize(buttonSize*0.75);
    centerOnButton->setIconSize(buttonSize*0.75);

    if (CLICK_TO_CENTER) {
        centerOnButton->hide();
    }

    dataBox = new QGroupBox(this);
    QVBoxLayout* boxLayout = new QVBoxLayout();

    //locationLabel = setupDataValueBox("Location", boxLayout, false);
    kindLabel = setupDataValueBox("kind", boxLayout);
    typeLabel = setupDataValueBox("type", boxLayout);
    topicLabel = setupDataValueBox("topicName", boxLayout);
    workerLabel = setupDataValueBox("worker", boxLayout);
    descriptionLabel = setupDataValueBox("description", boxLayout);

    dataBox->setLayout(boxLayout);
    dataBox->setStyleSheet("QGroupBox{ background: rgba(0,0,0,0); }");

    // add labels to layout
    layout->setMargin(LAYOUT_MARGIN);
    layout->setSpacing(LAYOUT_SPACING);
    layout->addWidget(iconLabel);
    layout->addWidget(entityLabel);
    layout->addWidget(locationLabel);
    layout->addStretch();
    layout->addWidget(expandButton);
    layout->addWidget(centerOnButton);
    layout->addSpacing(MARGIN_OFFSET);

    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addLayout(layout);
    mainLayout->addWidget(dataBox);

    setMinimumSize(MIN_WIDTH, MIN_HEIGHT);
    setLayout(mainLayout);
}


/**
 * @brief SearchItem::setupDataValueBox
 * @param key
 * @param layout
 * @return
 */
QLabel* SearchItem::setupDataValueBox(QString key, QLayout *layout, bool storeInHash)
{
    QGroupBox* dataValBox = new QGroupBox(this);
    QLabel* keyLabel = new QLabel(key + ":", this);
    QLabel* valueLabel = new QLabel(this);
    QHBoxLayout* subLayout = new QHBoxLayout();
    keyLabel->setFixedSize(KEY_LABEL_WIDTH, font().pointSize() + MARGIN_OFFSET);
    subLayout->addWidget(keyLabel);
    subLayout->addWidget(valueLabel);
    dataValBox->setLayout(subLayout);
    if (layout) {
        layout->addWidget(dataValBox);
    }
    if (storeInHash) {
        dataValueLabels[key] = valueLabel;
        dataValueBoxes[key] = dataValBox;
    }
    return valueLabel;
}


/**
 * @brief SearchItem::updateColor
 * This updates this button's color depending on its selected state.
 */
void SearchItem::updateColor()
{
    if (selected) {
        setStyleSheet("QLabel{ background: rgb(220,220,220); }"
                      "SearchItem{ border: 2px solid rgb(150,150,150); }"
                      + fixedStyleSheet);
    } else {
        setStyleSheet("QLabel{ background: rgb(240,240,240); }"
                      "SearchItem{ border: 1px solid rgb(180,180,180); }"
                      + fixedStyleSheet);
    }
}


/**
 * @brief SearchItem::getDataValues
 */
void SearchItem::getDataValues()
{
    if (!graphMLItem) {
        return;
    }
    // retrieve the values for the stored data keys
    GraphML* gml = graphMLItem->getGraphML();
    if (gml) {
        foreach (QString key, dataValueBoxes.keys()) {
            QString value = gml->getDataValue(key);
            dataValueLabels[key]->setText(value);
            dataValueBoxes[key]->setVisible(!value.isEmpty());
        }
    }
}


/**
 * @brief SearchItem::getItemLocation
 */
QString SearchItem::getItemLocation()
{
    if (!graphMLItem || !graphMLItem->isNodeItem() || !entityLabel) {
        return "";
    }

    // get NodeItem's location in the model
    NodeItem* nodeItem = (NodeItem*) graphMLItem;
    NodeItem* parentItem = nodeItem->getParentNodeItem();
    QString objectLocation = entityLabel->text();

    while (parentItem && parentItem->getGraphML()) {
        QString parentLabel = parentItem->getGraphML()->getDataValue("label");
        if (parentLabel.isEmpty()) {
            parentLabel = parentItem->getNodeKind();
        }
        objectLocation = parentLabel + " / " + objectLocation;
        parentItem = parentItem->getParentNodeItem();
    }

    return objectLocation;
}
