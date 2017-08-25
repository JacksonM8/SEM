#include "qosbrowser.h"

#include <QBoxLayout>
#include <QHeaderView>
#include <QDebug>
#include "qosprofilemodel.h"
#include "../../theme.h"

QOSBrowser::QOSBrowser(ViewController* vc, QWidget *parent) : QFrame(parent)
{
    this->vc = vc;
    qosModel = new QOSProfileModel(this);
    connect(vc, &ViewController::vc_viewItemConstructed, qosModel, &QOSProfileModel::viewItem_Constructed);
    connect(vc, &ViewController::vc_viewItemDestructing, qosModel, &QOSProfileModel::viewItem_Destructed);


    setupLayout();




    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}

void QOSBrowser::themeChanged()
{
    Theme* theme = Theme::theme();

    setStyleSheet("QOSBrowser{padding:0px;margin:0px;background-color: " % theme->getBackgroundColorHex() + ";border:1px solid " % theme->getDisabledBackgroundColorHex() % ";}");

    mainWidget->setStyleSheet("background:" + theme->getBackgroundColorHex() + ";");
    toolbar->setStyleSheet(theme->getToolBarStyleSheet() + "QToolBar{ padding: 0px; }");

    horizontalSplitter->setStyleSheet(theme->getSplitterStyleSheet());
    profileView->setStyleSheet(theme->getAbstractItemViewStyleSheet());
    elementView->setStyleSheet(theme->getAbstractItemViewStyleSheet());
    tableView->setStyleSheet(theme->getAbstractItemViewStyleSheet() +
                             "QAbstractItemView::item {"
                             "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                             "border-width: 0px 0px 1px 0px;"
                             "}");

    profileLabelButton->setStyleSheet("text-align: left; border-radius: 0px; background:" + theme->getAltBackgroundColorHex() + ";");
    policyLabelButton->setStyleSheet("text-align: left; border-radius: 0px; background:" + theme->getAltBackgroundColorHex() + ";");
    attributeLabelButton->setStyleSheet("text-align: left; border-radius: 0px; background:" + theme->getAltBackgroundColorHex() + ";");

    profileLabelButton->setIcon(theme->getImage("Icons", "speedGauge", QSize(16,16), theme->getMenuIconColorHex()));
    policyLabelButton->setIcon(theme->getImage("Icons", "buildingPillared", QSize(16,16), theme->getMenuIconColorHex()));
    attributeLabelButton->setIcon(theme->getImage("Icons", "label", QSize(16,16), theme->getMenuIconColorHex()));
}

void QOSBrowser::profileSelected(QModelIndex index1, QModelIndex)
{
    if(index1.isValid()){
        elementView->setModel(qosModel);
        elementView->setSelectionModel(elementViewSelectionModel);
        elementView->setRootIndex(index1);
    }else{
        elementView->setModel(0);
    }
    tableView->setModel(0);
    elementViewSelectionModel->clear();
    
    removeSelection->setEnabled(index1.isValid());
}

void QOSBrowser::settingSelected(QModelIndex index1, QModelIndex)
{
    tableView->setModel(qosModel->getTableModel(index1));
}

void QOSBrowser::removeSelectedProfile()
{
    QList<int> IDs;
    foreach(QModelIndex index, profileView->selectionModel()->selectedIndexes()){
        int ID = index.data(QOSProfileModel::ID_ROLE).toInt();
        if(ID > 0){
            IDs.append(ID);
        }
    }
    if(!IDs.isEmpty()){
        vc->vc_deleteEntities(IDs);
    }
}

void QOSBrowser::setupLayout()
{
    horizontalSplitter = new QSplitter(this);
    profileView = new QListView(this);
    elementView = new QTreeView(this);
    tableView = new DataTableView(this);

    toolbar = new QToolBar(this);
    toolbar->setIconSize(QSize(20,20));
    toolbar->setContentsMargins(0,0,0,4);

    //profileView->setFocusPolicy(Qt::NoFocus);
    //elementView->setFocusPolicy(Qt::NoFocus);

    profileView->setAttribute(Qt::WA_MacShowFocusRect, false);
    elementView->setAttribute(Qt::WA_MacShowFocusRect, false);

    toolbar->addAction(vc->getActionController()->toolbar_addDDSQOSProfile);
    removeSelection = vc->getActionController()->toolbar_removeDDSQOSProfile;

    toolbar->addAction(removeSelection);
    connect(removeSelection, &QAction::triggered, this, &QOSBrowser::removeSelectedProfile);
    removeSelection->setEnabled(false);

    int labelButtonHeight = elementView->header()->height() - 5;

    profileLabelButton = new QPushButton("Profiles", this);
    profileLabelButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    profileLabelButton->setEnabled(false);
    profileLabelButton->setFixedHeight(labelButtonHeight);

    policyLabelButton = new QPushButton("Policies", this);
    policyLabelButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    policyLabelButton->setEnabled(false);
    policyLabelButton->setFixedHeight(labelButtonHeight);

    attributeLabelButton = new QPushButton("Attributes", this);
    attributeLabelButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    attributeLabelButton->setEnabled(false);
    attributeLabelButton->setFixedHeight(labelButtonHeight);

    QWidget* profileWidget = new QWidget(this);
    profileWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QVBoxLayout* profileLayout = new QVBoxLayout(profileWidget);
    profileLayout->setSpacing(5);
    profileLayout->setMargin(0);
    profileLayout->addWidget(profileLabelButton);
    profileLayout->addWidget(profileView, 1);
    profileLayout->addWidget(toolbar, 0, Qt::AlignRight);

    QWidget* policyWidget = new QWidget(this);
    policyWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QVBoxLayout* policyLayout = new QVBoxLayout(policyWidget);
    policyLayout->setSpacing(5);
    policyLayout->setMargin(0);
    policyLayout->addWidget(policyLabelButton);
    policyLayout->addWidget(elementView, 1);

    QWidget* attributeWidget = new QWidget(this);
    attributeWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QVBoxLayout* attributeLayout = new QVBoxLayout(attributeWidget);
    attributeLayout->setSpacing(5);
    attributeLayout->setMargin(0);
    attributeLayout->addWidget(attributeLabelButton);
    attributeLayout->addWidget(tableView, 1);

    //elementView->header()->setMinimumHeight(25);
    elementView->header()->setVisible(false);

    horizontalSplitter->addWidget(profileWidget);
    horizontalSplitter->addWidget(policyWidget);
    horizontalSplitter->addWidget(attributeWidget);

    mainWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setMargin(5);
    mainLayout->addWidget(horizontalSplitter);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(mainWidget);

    profileView->setModel(qosModel);
    elementView->setModel(0);

    elementViewSelectionModel = new QItemSelectionModel(qosModel);

    profileView->setSelectionBehavior(QAbstractItemView::SelectItems);
    profileView->setSelectionMode(QAbstractItemView::SingleSelection);

    elementView->setSelectionBehavior(QAbstractItemView::SelectItems);
    elementView->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(profileView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(profileSelected(QModelIndex, QModelIndex)));
    connect(elementViewSelectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(settingSelected(QModelIndex, QModelIndex)));
}

