#ifndef ENTITYITEMNEW_H
#define ENTITYITEMNEW_H

#include "../viewitem.h"
#include <QGraphicsObject>
#include <QGraphicsSceneHoverEvent>
#include <QPen>

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#define ICON_RATIO (4.0 / 6.0)
#define SMALL_ICON_RATIO (1.0 / 6.0)
#define LABEL_RATIO (2.5 / 6.0)
#define DEFAULT_SIZE 72

#define SELECTED_LINE_WIDTH 3
class EntityItemNew: public QGraphicsObject
{
    Q_OBJECT

public:
    enum KIND{EDGE, NODE};
    enum ELEMENT_RECT{ER_PRIMARY_TEXT, ER_SECONDARY_TEXT, ER_MAIN_ICON, ER_MAIN_ICON_OVERLAY, ER_SECONDARY_ICON, ER_EXPANDED_STATE, ER_LOCKED_STATE, ER_STATUS, ER_CONNECT_IN, ER_CONNECT_OUT, ER_INFORMATION, ER_NOTIFICATION, ER_EXPANDCONTRACT, ER_SELECTION, ER_DEPLOYED, ER_QOS, ER_MOVE};
    enum RENDER_STATE{RS_NONE, RS_BLOCK, RS_MINIMAL, RS_REDUCED, RS_FULL};

    EntityItemNew(ViewItem *viewItem, EntityItemNew* parentItem, KIND kind);
    ~EntityItemNew();

    RENDER_STATE getRenderState(qreal lod) const;
    VIEW_STATE getViewState() const;
    EntityItemNew* getParent() const;
    void unsetParent();
    bool isTopLevelItem() const;


    ViewItem* getViewItem() const;

    virtual QPointF getSceneEdgeTermination(bool left) const = 0;


    virtual QColor getBodyColor() const;
    void setBodyColor(QColor color);

    virtual void setPos(const QPointF &pos);
    int getID();

    virtual QRectF getElementRect(ELEMENT_RECT rect) const;
    virtual QPainterPath getElementPath(ELEMENT_RECT rect) const;

    void paintPixmap(QPainter *painter, qreal lod, ELEMENT_RECT pos, QString imageAlias, QString imageName, QColor tintColor=QColor(), bool update=false);
    void paintPixmap(QPainter *painter, qreal lod, QRectF imageRect, QString imageAlias, QString imageName, QColor tintColor=QColor());
    void paintPixmap(QPainter *painter, qreal lod, QRectF imageRect, QPair<QString, QString> image, QColor tintColor=QColor());

    void setTooltip(ELEMENT_RECT rect, QString tooltip, QCursor cursor = Qt::ArrowCursor);

    QRectF translatedBoundingRect() const;
    virtual QRectF sceneBoundingRect() const;
    virtual QRectF boundingRect() const = 0;
    virtual QRectF currentRect() const = 0;
    virtual QRectF viewRect() const;
    QRectF sceneViewRect() const;

    QSize iconSize() const;
    QSize smallIconSize() const;
    QPainterPath shape() const;
    QPainterPath sceneShape() const;

    bool hasBeenMoved() const;
    void adjustPos(QPointF delta);
    virtual QPointF getPos() const;

    virtual QPointF validateAdjustPos(QPointF delta);

    bool intersectsRectInScene(QRectF rectInScene) const;
    bool isDataProtected(QString keyName) const;

    void addRequiredData(QString keyName);
    void removeRequiredData(QString keyName);
    QStringList getRequiredDataKeys();
    void reloadRequiredData();
    QPen getPen();
    void setDefaultPen(QPen pen);
    QPair<QString, QString> getIconPath();

    QPointF getNearestGridPoint();

    void setFontSize(int fontSize);


private:
    int fontSize;
    QFont textFont;

public:
    //Model State Get/Setters
    void setData(QString keyName, QVariant value);
    QVariant getData(QString keyName) const;
    bool hasData(QString keyName) const;
    bool isDataEditable(QString keyName);

public:
    //State Handlers
    void handleExpand(bool expand);
    void handleHover(bool hovered);
    void handleSelection(bool append);

protected:
    void removeData(QString keyName);
public:
    //Item State Getters
    bool isNodeItem();
    bool isEdgeItem();

public:
    //Feature State Setters
    virtual void setExpanded(bool);
    void setMoveEnabled(bool enabled);
    void setHoverEnabled(bool enabled);
    void setExpandEnabled(bool enabled);
    void setSelectionEnabled(bool enabled);

    virtual void setMoving(bool moving);

public:
    //State Getters
    bool isSelected() const;
    bool isExpanded() const;
    bool isActiveSelected() const;
    bool isHovered() const;
    bool isHighlighted() const;
    bool isMoving() const;

    int getGridSize() const;
    int getMajorGridCount() const;
    virtual QPointF getSceneCenter() const;
    virtual QPointF getCenterOffset() const;

    virtual void setCenter(QPointF center);
    virtual QPointF getCenter() const;


public:
    //Feature State Getters
    bool isSelectionEnabled();
    bool isExpandEnabled();
    bool isMoveEnabled();
    bool isHoverEnabled();

    void setIgnorePosition(bool ignore);
    bool isIgnoringPosition();




signals:
    //Request changes
    void req_adjustPos(QPointF delta);
    void req_adjustingPos(bool adjusting);

    //Request changes
    void req_selected(ViewItem*, bool);
    void req_activeSelected(ViewItem*);

    void req_setData(ViewItem* item, QString keyName, QVariant data);
    void req_removeData(ViewItem* item, QString keyName);
    void req_editData(ViewItem* item, QString keyName);

    void req_triggerAction(QString actionName);

    void req_centerItem(EntityItemNew* entityItem);
    void req_hovered(EntityItemNew*, bool);
    void req_expanded(EntityItemNew*, bool);

    //Inform of changes
    void sizeChanged();
    void positionChanged();
    void scenePosChanged();
private slots:
    virtual void dataChanged(QString keyName, QVariant data) = 0;
    virtual void dataRemoved(QString keyName) = 0;
    void destruct();

public:
    void setHovered(bool isHovered);
    void setHighlighted(bool isHighlight);
    void setSelected(bool selected);
    void setActiveSelected(bool active);

    void renderText(QPainter* painter, qreal lod, QRectF textRect, QString text, int fontSize = -1) const;
private:
    void paintPixmapRect(QPainter* painter, QString imageAlias, QString imageName, QRectF rect);
    void paintPixmap(QPainter* painter, QRectF imageRect, QPixmap pixmap) const;
    QPixmap getPixmap(QString imageAlias, QString imageName, QSize requiredSize=QSize(), QColor tintColor=QColor()) const;
    QSize getPixmapSize(QRectF rect, qreal lod) const;

    void connectViewItem(ViewItem* viewItem);
    void disconnectViewItem();

    EntityItemNew* parentItem;
    ViewItem* viewItem;
    QStringList requiredDataKeys;

    QHash<ELEMENT_RECT, QPixmap> imageMap;
    QHash<ELEMENT_RECT, QString> tooltipMap;
    QHash<ELEMENT_RECT, QCursor> tooltipCursorMap;



    bool selectEnabled;
    bool hoverEnabled;
    bool moveEnabled;
    bool expandEnabled;

    bool _isHovered;
    bool _isHightlighted;
    bool _isSelected;
    bool _isActiveSelected;
    bool _isExpanded;

    bool _hasMoved;

    bool _isMoving;
    bool _isMouseMoving;
    bool _hasMouseMoved;

    QPointF previousMovePoint;

    QPen defaultPen;
    QColor bodyColor;

    KIND kind;
    bool ignorePosition;




protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);


    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // GRAPHMLITEM_H
