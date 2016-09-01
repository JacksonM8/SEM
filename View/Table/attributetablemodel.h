#ifndef ATTRIBUTETABLEMODEL_H
#define ATTRIBUTETABLEMODEL_H

#include <QAbstractTableModel>


class ViewItem;
class AttributeTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum ATTRIBUTE_ROLES {
        MULTILINE_ROLE = Qt::UserRole + 1,
        VALID_VALUES_ROLE = Qt::UserRole + 2,
        ID_ROLE = Qt::UserRole + 3
    };
    AttributeTableModel(ViewItem* item);
    ~AttributeTableModel();

signals:
    void req_dataChanged(int ID, QString keyName, QVariant data);

public slots:
    void updatedData(QString keyName);
    void removedData(QString keyName);

    void addData(QString keyName);
    void clearData();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool insertRows(int row, int count, const QModelIndex &parent);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    void sort(int column, Qt::SortOrder order);
private:
    int getIndex(QString keyName) const;
    QString getKey(const QModelIndex &index) const;
    QString getKey(int row) const;
    bool isIndexProtected(const QModelIndex &index) const;
    bool isRowProtected(int row) const;
    bool hasPopupEditor(const QModelIndex &index) const;
    QVariant getData(const QModelIndex &index) const;

    bool isDataProtected(int row) const;
    bool hasData() const;

    void setupDataBinding();

    ViewItem* entity;





    QStringList ignoredKeys;
    QStringList editableKeys;
    QStringList lockedKeys;
    QStringList multiLineKeys;
};

#endif // ATTRIBUTETABLEMODEL_H
