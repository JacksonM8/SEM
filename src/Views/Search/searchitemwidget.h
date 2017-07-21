#ifndef SEARCHITEMWIDGET_H
#define SEARCHITEMWIDGET_H

#include "../../Controllers/ViewController/viewitem.h"
#include "../../Controllers/ViewController/nodeviewitem.h"
#include "../../theme.h"

#include <QFrame>
#include <QWidget>
#include <QToolButton>
#include <QLabel>
#include <QVBoxLayout>

class SearchItemWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SearchItemWidget(ViewItem* item, QWidget *parent = 0);
    ~SearchItemWidget();

    void addDisplayKey(QString key);

    void setAspectFilterKey(int key);
    void setDataFilterKey(int key);
    void setFilterKeys(QList<int> keys);

    void setSelected(bool selected);
    
signals:
    void hoverEnter(int ID);
    void hoverLeave(int ID);
    void itemSelected(int ID);

public slots:
    void themeChanged();
    void expandButtonToggled(bool checked);

    void filterCleared(int filter);
    void filtersChanged(int filter, QList<QVariant> checkedKeys);
    
protected:
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    void setupLayout(QVBoxLayout* layout);
    void constructKeyWidgets();

    void toggleKeyWidgets(QList<QVariant> checkedKeys);

    void updateStyleSheet();
    void updateVisibility(int filter, bool visible);

    ViewItem* viewItem;
    int viewItemID;    
    VIEW_ASPECT viewAspect;
    
    QLabel* iconLabel;
    QSize iconSize;
    QPair<QString, QString> iconPath;

    QLabel* textLabel;
    QToolButton* expandButton;
    QWidget* displayWidget;
    
    QStringList keys;
    QHash<QString, QWidget*> keyWidgetHash;

    QString backgroundColor;

    bool keyWidgetsConstructed;
    bool doubleClicked;
    bool selected;
    bool visible;

    QHash<int, bool> filterVisibility;
    QVariant aspectFilterKey;
    QVariant dataFilterKey;

};

#endif // SEARCHITEMWIDGET_H
