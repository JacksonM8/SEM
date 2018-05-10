#include "datatablemodel.h"
#include "../../Controllers/ViewController/viewitem.h"
#include "../../theme.h"

#include <QDebug>

DataTableModel::DataTableModel(ViewItem *item)
{
    entity = item;
    //Register the table
    entity->registerObject(this);

    multiline_keys << "processes_to_log" << "code";
    icon_keys << "icon" << "icon_prefix";
    ignoredKeys << "x" << "y" << "width" << "height" << "readOnly";
    //ignoredKeys << "icon" << "icon_prefix";
    setupDataBinding();
}

DataTableModel::~DataTableModel()
{
    entity->unregisterObject(this);
}

void DataTableModel::updatedData(QString keyName)
{
    int row = getIndex(keyName);
    if(row > -1){
        QModelIndex indexA = index(row, 0, QModelIndex());
        QModelIndex indexB = index(row, rowCount(indexA), QModelIndex());
        emit dataChanged(indexA, indexB);
    }
}

void DataTableModel::removedData(QString keyName)
{
    //Get the Index of the data to be removed.
    int row = getIndex(keyName);
    if(row > -1){
        //Initiate the removal of the row.
        beginRemoveRows(QModelIndex(), row, row);

        //Remove it from the HashMap.
        editableKeys.removeAll(keyName);
        lockedKeys.removeAll(keyName);
        endRemoveRows();
    }
}

void DataTableModel::addData(QString keyName)
{
    //If we haven't seen this Data Before.

    if(editableKeys.contains(keyName) || lockedKeys.contains(keyName) || ignoredKeys.contains(keyName)){
        return;
    }

    bool locked = entity->isDataProtected(keyName);

    int insertIndex = 0;
    if(locked){
        insertIndex = lockedKeys.size();
        lockedKeys.append(keyName);
    }else{
        insertIndex = lockedKeys.size() + editableKeys.size();
        editableKeys.append(keyName);
    }

    //Insert Rows.
    beginInsertRows(QModelIndex(), insertIndex, insertIndex);
    endInsertRows();
    sort(0, Qt::AscendingOrder);
}

bool DataTableModel::hasData() const
{
    return rowCount(QModelIndex()) > 0;
}

void DataTableModel::clearData()
{
    beginRemoveRows(QModelIndex(), 0, rowCount(QModelIndex()));
    editableKeys.clear();
    lockedKeys.clear();
    endRemoveRows();
}

int DataTableModel::getIndex(QString keyName) const
{
    int index = -1;
    if(lockedKeys.contains(keyName)){
        index = lockedKeys.indexOf(keyName);
    }else if(editableKeys.contains(keyName)){
        index = lockedKeys.size() + editableKeys.indexOf(keyName);
    }else if(ignoredKeys.contains(keyName)){
        index = -2;
    }
    return index;
}

QString DataTableModel::getKey(const QModelIndex &index) const
{
    return getKey(index.row());
}

QString DataTableModel::getKey(int row) const
{
    QString key;
    if(row < lockedKeys.size()){
        key = lockedKeys[row];
    }else{
        row -= lockedKeys.size();
        if(row < editableKeys.size()){
            key = editableKeys[row];
        }
    }
    return key;
}

bool DataTableModel::isIndexProtected(const QModelIndex &index) const
{
    return isRowProtected(index.row());
}

bool DataTableModel::isRowProtected(int row) const
{
    QString key = getKey(row);
    bool isProtected = true;
    if(entity){
        isProtected = entity->isDataProtected(key);
    }
    return isProtected;
}

bool DataTableModel::hasCodeEditor(const QModelIndex &index) const
{
    QString key_name = getKey(index);
    return multiline_keys.contains(key_name);
}

bool DataTableModel::hasIconEditor(const QModelIndex &index) const
{
    QString key_name = getKey(index);
    return icon_keys.contains(key_name);
}

QVariant DataTableModel::getData(const QModelIndex &index) const
{
    QVariant data;
    if(entity){
        QString key = getKey(index);
        data = entity->getData(key);
    }
    return data;
}




int DataTableModel::rowCount(const QModelIndex&) const
{
    return editableKeys.size() + lockedKeys.size();
}

int DataTableModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant DataTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole) {
        if(hasCodeEditor(index)){
            return QVariant(Qt::AlignLeft | Qt::AlignTop);
        }else{
            return QVariant(Qt::AlignCenter);
        }
    }
    if (role == Qt::DecorationRole) {
        if(hasCodeEditor(index) || hasIconEditor(index)){
            return  Theme::theme()->getImage("Icons", "popOut", QSize(16,16), Theme::theme()->getMenuIconColor());
        }
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
        return getData(index);
    }

    if(role == MULTILINE_ROLE) {
        return hasCodeEditor(index);
    }

    if(role == ICON_ROLE){
        return hasIconEditor(index);
    }

    if(role == VALID_VALUES_ROLE){
        if(entity){
            QString keyName = getKey(index);
            return entity->getValidValuesForKey(keyName);
        }
    }
    if(role == ID_ROLE){
        if(entity){
            return entity->getID();
        }
    }

    return QVariant();
}

QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical){
        auto key = getKey(section);
        if(role == Qt::DisplayRole){
            return key;
        }/* //UNCOMMENT FOR ICONS IN TABLE
        else if(role == Qt::DecorationRole){
            return  Theme::theme()->getImage("Data", key, QSize(16,16), Theme::theme()->getTextColor());
        }*/
    }
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal){
        if(section == 0){
            return "Key";
        }else{
            return "Value";
        }
    }


    if (role == Qt::ToolTipRole) {
        if(isRowProtected(section)){
            return "Data is Protected";
        }
    }
    return QVariant();
}

bool DataTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        int column = index.column();
        int row = index.row();
        if(column == 0 && !isIndexProtected(index)){
            QString key = getKey(row);
            emit req_dataChanged(entity->getID(), key, value);
            return true;
        }
    }
    return false;
}

bool DataTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(row)
    Q_UNUSED(count)
    Q_UNUSED(parent)
    return true;
}


Qt::ItemFlags DataTableModel::flags(const QModelIndex &index) const
{
    if(index.isValid()){
        if(!isIndexProtected(index)){
            //Set it editable.
            return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
    }
    return Qt::NoItemFlags;
}

bool DataTableModel::isDataProtected(int row) const
{
    QString keyName = getKey(row);
    return entity->isDataProtected(keyName);
}


void DataTableModel::setupDataBinding()
{
    if(entity){
        //Attach each data.
        auto key_list = entity->getKeys();
        std::sort(key_list.begin(), key_list.end());
        for(auto key : key_list){
            addData(key);
        }
        connect(entity, &ViewItem::dataAdded, this, &DataTableModel::addData);
        connect(entity, &ViewItem::dataRemoved, this, &DataTableModel::removedData);
        connect(entity, &ViewItem::dataChanged, this, &DataTableModel::updatedData);


        connect(entity, &ViewItem::destructing, this, &DataTableModel::deleteLater);
    }
}



void DataTableModel::sort(int, Qt::SortOrder order)
{
    if(order == Qt::AscendingOrder){
        qSort(lockedKeys.begin(), lockedKeys.end(), qLess<QString>());
        qSort(editableKeys.begin(), editableKeys.end(), qLess<QString>());
    }else{
        qSort(lockedKeys.begin(), lockedKeys.end(), qGreater<QString>());
        qSort(editableKeys.begin(), editableKeys.end(), qGreater<QString>());
    }

    QModelIndex indexA = index(0, 0, QModelIndex());
    QModelIndex indexB = index(columnCount(QModelIndex()), rowCount(indexA), QModelIndex());
    //Do some sorting!
    emit dataChanged(indexA, indexB);
}
