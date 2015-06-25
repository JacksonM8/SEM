#ifndef DOCKNODEITEM_H
#define DOCKNODEITEM_H

#include "../nodeview.h"

#include <QPushButton>
#include <QLabel>

class DockScrollArea;

class DockNodeItem : public QPushButton
{
    Q_OBJECT
public:
    explicit DockNodeItem(QString kind = "", NodeItem* item = 0, QWidget* parent = 0);

    NodeItem* getNodeItem();
    QString getKind();

    void setLabel(QString newLabel);
    QString getLabel();

    void setParentDockNodeItem(DockNodeItem* parentItem);
    DockNodeItem* getParentDockNodeItem();

    void addChildDockItem(DockNodeItem* dockItem);
    void removeChildDockItem(DockNodeItem* dockItem);
    QList<DockNodeItem*> getChildrenDockItems();

    QString getID();
    void setHidden(bool hideItem);
    bool isHidden();

    bool isFileLabel();
    bool isExpanded();

signals:
    void dockItem_clicked();
    void dockItem_fileClicked(bool show);
    void dockItem_relabelled(DockNodeItem* dockItem);
    void dockItem_removeFromDock(DockNodeItem* dockItem);

    void dockItem_hidden();

public slots:
    void labelChanged(QString label);
    void clicked();
    void parentDockItemClicked(bool show);


    void updateData();
    void deleteLater();

    void childDockItemHidden();
    void highlightDockItem(NodeItem* node);

private:
    void setupLayout();
    void updateTextLabel();
    void updateStyleSheet();

    DockScrollArea* parentDock;
    NodeItem* nodeItem;
    DockNodeItem* parentDockItem;
    QList<DockNodeItem*> childrenDockItems;

    QString kind;
    QString label;
    QString ID;

    QLabel* textLabel;
    QLabel* imageLabel;

    QString highlightColor;

    bool fileLabel;
    bool expanded;
    bool hidden;
    bool highlighted;
};

#endif // DOCKNODEITEM_H
