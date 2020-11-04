//
// Created by Cathlyn Aston on 29/10/20.
//

#include "freeformtray.h"
#include "../pulseviewdefaults.h"

#include <stdexcept>

using namespace Pulse::View;

const qreal stack_gap_ = Defaults::layout_padding * 2;

/**
 * @brief FreeFormTray::FreeFormTray
 * @param parent
 */
FreeFormTray::FreeFormTray(QGraphicsItem* parent)
    : QGraphicsWidget(parent) {}

/**
 * @brief FreeFormTray::addItem
 * @param widget
 */
void FreeFormTray::addItem(QGraphicsWidget* widget)
{
    checkPreConditions(widget);
    prepareGeometryChange();

    auto stack_pos = getNextStackPos();
    widget->setPos(stack_pos);
    widget->setParentItem(this);
    widget->setVisible(true);
    contained_items_.push_back(widget);

    // When a child item's geometry has changed, update the tray's geometry and schedule a repaint
    connect(widget, &QGraphicsWidget::geometryChanged, [this]() {
        updateGeometry();
        update();
    });
}

/**
 * @brief FreeFormTray::isEmpty
 * @return
 */
bool FreeFormTray::isEmpty() const
{
    return contained_items_.empty();
}

/**
 * @brief FreeFormTray::boundingRect
 * @return
 */
QRectF FreeFormTray::boundingRect() const
{
    return {QPointF(0,0), getVisibleItemsRect().bottomRight()};
}

/**
 * @brief FreeFormTray::setGeometry
 * @param geom
 */
void FreeFormTray::setGeometry(const QRectF& geom)
{
    // Force this item's geometry to have the same size as the bounding rect
    prepareGeometryChange();
    QRectF adjusted_rect(geom.topLeft(), boundingRect().size());
    QGraphicsWidget::setGeometry(adjusted_rect);
}

/**
 * @brief FreeFormTray::getNextStackPos
 * @return
 */
QPointF FreeFormTray::getNextStackPos() const
{
    const int content_count = contained_items_.size();
    QPointF offset(stack_gap_ * content_count, stack_gap_ * content_count);
    return offset;
}

/**
 * @brief FreeFormTray::getVisibleItemsRect
 * @return
 */
QRectF FreeFormTray::getVisibleItemsRect() const
{
    QRectF visible_rect;
    for (const auto& child_item : childItems()) {
        if (child_item->isVisible()) {
            auto&& child_geom = QRectF(child_item->pos(), child_item->boundingRect().size());
            visible_rect = visible_rect.united(child_geom);
        }
    }
    return visible_rect;
}

/**
 * @brief FreeFormTray::checkPreConditions
 * @param widget
 * @throws std::invalid_argument
 * @throws std::logic_error
 */
void FreeFormTray::checkPreConditions(QGraphicsWidget* widget) const
{
    if (widget == nullptr) {
        throw std::invalid_argument("FreeFormTray - Trying to add a null item");
    }
    if (childItems().contains(widget)) {
        throw std::logic_error("FreeFormTray - Trying to add an item that already exists");
    }
}