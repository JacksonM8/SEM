#include "timelinechart.h"
#include "../../../../theme.h"
#include "entitychart.h"

#include <QPainter>
#include <QWheelEvent>
#include <QWidgetAction>
#include <QToolButton>

#define BACKGROUND_OPACITY 0.25
#define HOVER_LINE_WIDTH 2.0


/**
 * @brief TimelineChart::TimelineChart
 * @param parent
 */
TimelineChart::TimelineChart(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::WheelFocus);
    setRange(0, 100);

    _layout = new QVBoxLayout(this);
    _layout->setMargin(0);
    _layout->setSpacing(0);

    hoverRect = QRectF(0, 0, HOVER_LINE_WIDTH, height());

    connect(Theme::theme(), &Theme::theme_Changed, this, &TimelineChart::themeChanged);
    themeChanged();
}


/**
 * @brief TimelineChart::setRange
 * @param min
 * @param max
 */
void TimelineChart::setRange(double min, double max)
{
    for (EntityChart* chart : _entityCharts) {
        chart->setRange(min, max);
        chart->setDisplayRangeRatio(0.0, 1.0);
    }
}


/**
 * @brief TimelineChart::setAxisXVisible
 * @param visible
 */
void TimelineChart::setAxisXVisible(bool visible)
{
    axisXVisible = visible;
}


/**
 * @brief TimelineChart::setAxisYVisible
 * @param visible
 */
void TimelineChart::setAxisYVisible(bool visible)
{
    axisYVisible = visible;
}


/**
 * @brief TimelineChart::setAxisWidth
 * @param width
 */
void TimelineChart::setAxisWidth(double width)
{
    axisWidth = width;
    axisLinePen.setWidthF(width);
    setContentsMargins(width, 0, 0, 0);
}


/**
 * @brief TimelineChart::setPointsWidth
 * @param width
 */
void TimelineChart::setPointsWidth(double width)
{
    for (EntityChart* chart : _entityCharts) {
        chart->setPointWidth(width);
    }
    pointsWidth = width;
}


/**
 * @brief TimelineChartWidget::addEntityChart
 * @param chart
 */
void TimelineChart::addEntityChart(EntityChart* chart)
{
    insertEntityChart(-1, chart);
}


/**
 * @brief TimelineChart::insertEntityChart
 * @param index
 * @param chart
 */
void TimelineChart::insertEntityChart(int index, EntityChart* chart)
{
    if (chart) {
        _entityCharts.append(chart);
        _layout->insertWidget(index, chart);
        chart->setPointWidth(pointsWidth);
        chart->installEventFilter(this);
    }
}


/**
 * @brief TimelineChart::removeEntityChart
 * @param chart
 */
void TimelineChart::removeEntityChart(EntityChart* chart)
{
    if (chart) {
        _layout->removeWidget(chart);
        _entityCharts.removeAll(chart);
    }
}


/**
 * @brief TimelineChart::getEntityCharts
 * @return
 */
const QList<EntityChart*>& TimelineChart::getEntityCharts()
{
    return _entityCharts;
}


/**
 * @brief TimelineChart::getHoverRect
 * @return
 */
const QRectF& TimelineChart::getHoverRect()
{
    return hoverRect;
}


/**
 * @brief TimelineChart::isPanning
 * @return
 */
bool TimelineChart::isPanning()
{
    return dragMode == DRAG_MODE::PAN || dragMode == DRAG_MODE::RUBBERBAND;
}


/**
 * @brief TimelineChart::themeChanged
 */
void TimelineChart::themeChanged()
{
    Theme* theme = Theme::theme();
    highlightColor = theme->getHighlightColor();
    hoveredRectColor = theme->getBackgroundColor();

    backgroundColor = theme->getAltBackgroundColor();
    backgroundColor.setAlphaF(BACKGROUND_OPACITY);
    backgroundHighlightColor = theme->getActiveWidgetBorderColor();
    backgroundHighlightColor.setAlphaF(BACKGROUND_OPACITY * 2.0);

    cursorPen = QPen(theme->getTextColor(), 8);
    axisLinePen = QPen(theme->getAltTextColor(), axisWidth);
    topLinePen = QPen(theme->getAltTextColor(), 1.5);
    hoverLinePen = QPen(theme->getTextColor(), HOVER_LINE_WIDTH, Qt::PenStyle::DotLine);
}


/**
 * @brief TimelineChart::setEntityChartHovered
 * This is called
 * @param chart
 * @param hovered
 */
void TimelineChart::setEntityChartHovered(EntityChart* chart, bool hovered)
{
    if (chart) {
        hoveredChartRect = hovered ? chart->rect() : QRectF();
        hoveredChartRect.moveTo(chart->pos());
        chart->setHovered(hovered);
        update();
    } else {
        hoveredChartRect = QRectF();
    }
}


/**
 * @brief TimelineChart::eventFilter
 * @param watched
 * @param event
 * @return
 */
bool TimelineChart::eventFilter(QObject* watched, QEvent *event)
{
    // the signal below is used to tell the entity axis which chart was hovered
    // it's not sent from the setEntityChartHovered to avoid duplicated signals
    // because the entity axis uses that slot to hover on a chart
    EntityChart* chart = qobject_cast<EntityChart*>(watched);
    if (event->type() == QEvent::Enter) {
        setEntityChartHovered(chart, true);
        emit entityChartHovered(chart, true);
    } else if (event->type() == QEvent::Leave) {
        setEntityChartHovered(chart, false);
        emit entityChartHovered(chart, false);
    }
    return QWidget::eventFilter(watched, event);
}


/**
 * @brief TimelineChart::mousePressEvent
 * @param event
 */
void TimelineChart::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        dragMode = PAN;
        panOrigin = event->pos();
        setCursor(Qt::ClosedHandCursor);
        // if the CTRL key is down, go into rubberband mode
        if (event->modifiers() & Qt::ControlModifier) {
            dragMode = RUBBERBAND;
            setCursor(Qt::CrossCursor);
        }
    }
    QWidget::mousePressEvent(event);
}


/**
 * @brief TimelineChart::mouseReleaseEvent
 * @param event
 */
void TimelineChart::mouseReleaseEvent(QMouseEvent* event)
{
    if (dragMode == RUBBERBAND && !rubberBandRect.isNull()) {
        // send a signal to update the axis' displayed range
        emit rubberbandUsed(rubberBandRect.left(), rubberBandRect.right());
    }

    // this is only here to demo that the hover axis dislay's position can be set manually
    //emit hoverLineUpdated(hoverRect.isValid(), QPointF(0, 0));

    clearDragMode();
    QWidget::mouseReleaseEvent(event);
}


/**
 * @brief TimelineChart::mouseMoveEvent
 * @param event
 */
void TimelineChart::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    cursorPoint = mapFromGlobal(cursor().pos());
    hoverRect.moveCenter(cursorPoint);
    hoverRectUpdated(true);

    switch (dragMode) {
    case PAN: {
        QPointF delta = cursorPoint - panOrigin;
        panOrigin = cursorPoint;
        emit panned(-delta.x(), 0);
        break;
    }
    case RUBBERBAND:
        rubberBandRect = QRectF(panOrigin.x(), 0, event->pos().x() - panOrigin.x(), height());
        break;
    default:
        break;
    }
}


/**
 * @brief TimelineChart::wheelEvent
 * @param event
 */
void TimelineChart::wheelEvent(QWheelEvent *event)
{
    // need to accept the event here so that it doesn't scroll the timeline chart
    emit zoomed(event->delta());
    event->accept();
}


/**
 * @brief TimelineChart::keyReleaseEvent
 * @param event
 */
void TimelineChart::keyReleaseEvent(QKeyEvent *event)
{
    if (dragMode == RUBBERBAND) {
        if (event->key() == Qt::Key_Control) {
            clearDragMode();
        }
    }
    QWidget::keyReleaseEvent(event);
}


/**
 * @brief TimelineChart::enterEvent
 * @param event
 */
void TimelineChart::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);
    setCursor(Qt::BlankCursor);
    hovered = true;

    hoverRect = visibleRegion().boundingRect();
    hoverRect.setWidth(HOVER_LINE_WIDTH);
    hoverRectUpdated();
}


/**
 * @brief TimelineChart::leaveEvent
 * @param event
 */
void TimelineChart::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    hovered = false;
    hoverRect = QRectF();
    hoverRectUpdated();
}


/**
 * @brief TimelineChart::paintEvent
 * @param event
 */
void TimelineChart::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect visibleRect = visibleRegion().boundingRect();
    painter.fillRect(visibleRect, backgroundColor);

    visibleRect = visibleRect.adjusted(axisWidth / 2.0, 0, 0, 0);

    /*visibleRect = visibleRect.adjusted(axisWidth / 2.0, 0, 0, topLinePen.widthF() / 2.0);
    QLineF topLine(visibleRect.topLeft(), visibleRect.topRight());
    painter.setPen(topLinePen);
    painter.drawLine(topLine);*/

    painter.setPen(axisLinePen);
    if (axisXVisible) {
        QLineF axisX(visibleRect.bottomLeft(), visibleRect.bottomRight());
        painter.drawLine(axisX);
    }
    if (axisYVisible) {
        QLineF axisY(visibleRect.topLeft(), visibleRect.bottomLeft());
        painter.drawLine(axisY);
    }

    switch (dragMode) {
    case RUBBERBAND: {
        painter.setOpacity(0.25);
        painter.setPen(QPen(highlightColor.darker(), 2));
        painter.setBrush(highlightColor);
        painter.drawRect(rubberBandRect);
        break;
    }
    default: {
        //painter.fillRect(hoveredChartRect, backgroundHighlightColor);
        if (!hoveredChartRect.isNull()) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(backgroundHighlightColor);
            painter.drawRect(hoveredChartRect);
        }
        if (hovered) {
            painter.setPen(hoverLinePen);
            painter.drawLine(cursorPoint.x(), rect().top(), cursorPoint.x(), rect().bottom());
            painter.setPen(cursorPen);
            painter.drawPoint(cursorPoint);
        }
        break;
    }
    }
}


/**
 * @brief TimelineChart::hoverRectUpdated
 * @param repaintRequired
 */
void TimelineChart::hoverRectUpdated(bool repaintRequired)
{
    if (hoverRect.isNull()) {
        for (EntityChart* chart : _entityCharts) {
            chart->setHoveredRect(hoverRect);
        }
    } else {
        for (EntityChart* chart : _entityCharts) {
            if (!chart->isVisible())
                continue;
            QRect childRect(chart->x(), chart->y(), chart->width(), chart->height());
            if (visibleRegion().contains(childRect)) {
                chart->setHoveredRect(hoverRect);
            }
        }
    }

    if (repaintRequired) {
        // this repaint is required instead of an update whenever there's a moveEvent
        // the hovered series ranges are being calculated in the children charts' paint event
        // and it needs to happen before the signal below is sent to the timeline view
        repaint();
    } else {
        update();
    }

    emit hoverLineUpdated(hoverRect.isValid(), mapToGlobal(hoverRect.center().toPoint()));
}


/**
 * @brief TimelineChart::clearDragMode
 */
void TimelineChart::clearDragMode()
{
    dragMode = NONE;
    rubberBandRect = QRectF();
    setCursor(Qt::BlankCursor);
    update();
}


/**
 * @brief TimelineChart::mapLocalPixelToTime
 * @param pixel
 * @return
 */
double TimelineChart::mapLocalPixelToTime(double pixel)
{
    double ratio = pixel / width();
    return ratio; // * (_displayMax - _displayMin) + _displayMin;
}
