#include "axisdisplay.h"
#include "../../../../theme.h"

#include <QDateTime>

#define SPACING 3

/**
 * @brief AxisDisplay::AxisDisplay
 * @param slider
 * @param parent
 * @param type
 */
AxisDisplay::AxisDisplay(AxisSlider* slider, QWidget* parent, VALUE_TYPE type)
    : QWidget(parent)
{
    if (!slider) {
        qWarning("AxisDisplay::AxisDisplay - Axis is null");
        return;
    }

    _slider = slider;
    _orientation = slider->getOrientation();
    _penWidth = slider->getAxisPenWidth();
    _textHeight = fontMetrics().height();
    _tickLength = _textHeight / 4;

    _valueType = type;
    _axisFormat = TIME_DISPLAY_FORMAT::VALUE;

    if (_orientation == Qt::Horizontal) {
        _spacing = SPACING;
        setMinimumHeight(_textHeight + _tickLength + _spacing * 2);
        if (_valueType == VALUE_TYPE::DATE_TIME) {
            setMinimumHeight((_textHeight + _tickLength) * 2 + _spacing * 2);
            setMinimumWidth(fontMetrics().width(QDateTime::fromMSecsSinceEpoch(0).toString(TIME_FORMAT)));
            _axisFormat = TIME_DISPLAY_FORMAT::DATE_TIME;
        }
    } else {
        _spacing = SPACING * 2;
        setMinimumWidth(slider->width());
    }

    switch (_slider->getAlignment()) {
    case Qt::AlignTop:
        _textAlignment = Qt::AlignBottom | Qt::AlignHCenter;
        break;
    case Qt::AlignBottom:
        _textAlignment = Qt::AlignTop | Qt::AlignHCenter;
        break;
    case Qt::AlignLeft:
        _textAlignment = Qt::AlignRight | Qt::AlignVCenter;
        break;
    case Qt::AlignRight:
        _textAlignment = Qt::AlignLeft | Qt::AlignVCenter;
        break;
    default:
        _textAlignment = Qt::AlignTop | Qt::AlignHCenter;
        break;
    }

    connect(slider, &AxisSlider::minRatioChanged, this, &AxisDisplay::updateDisplayedMin);
    connect(slider, &AxisSlider::maxRatioChanged, this, &AxisDisplay::updateDisplayedMax);
    connect(Theme::theme(), &Theme::theme_Changed, this, &AxisDisplay::themeChanged);
    themeChanged();
}


/**
 * @brief AxisDisplay::setAxisMargin
 * @param margin
 */
void AxisDisplay::setAxisMargin(int margin)
{
    _spacing = margin;
    if (_orientation == Qt::Horizontal) {
        setMinimumHeight(_textHeight + _tickLength + _spacing * 2);
    }
    update();
}


/**
 * @brief AxisDisplay::setAxisLineVisible
 * @param visible
 */
void AxisDisplay::setAxisLineVisible(bool visible)
{
    _axisLineVisible = visible;
    update();
}


/**
 * @brief AxisDisplay::setTicksVisible
 * @param visible
 */
void AxisDisplay::setTickVisible(bool visible)
{
    _tickVisible = visible;
    update();
}


/**
 * @brief AxisDisplay::setTickCount
 */
void AxisDisplay::setTickCount(int ticks)
{
    _tickCount = qMax(1, ticks);
    update();
}


/**
 * @brief AxisDisplay::setMax
 * @param max
 */
void AxisDisplay::setMax(double max)
{
    _max = max;
    _range = max - _min;
    update();
}


/**
 * @brief AxisDisplay::setMin
 * @param min
 */
void AxisDisplay::setMin(double min)
{
    _min = min;
    _range = _max - min;
    update();
}


/**
 * @brief AxisDisplay::setRange
 * @param min
 * @param max
 */
void AxisDisplay::setRange(double min, double max)
{
    _min = min;
    _max = max;
    _range = max - min;
    update();
}


/**
 * @brief AxisDisplay::getRange
 * @return
 */
QPair<double, double> AxisDisplay::getRange()
{
    return {_min, _max};
}


/**
 * @brief AxisDisplay::getDisplayedRange
 * @return
 */
QPair<double, double> AxisDisplay::getDisplayedRange()
{
    return {_displayedMin, _displayedMax};
}


/**
 * @brief AxisDisplay::setDisplayFormat
 * @param format
 */
void AxisDisplay::setDisplayFormat(TIME_DISPLAY_FORMAT format)
{
    switch (format) {
    case TIME_DISPLAY_FORMAT::DATE_TIME:
        setMinimumHeight(minimumHeight() * 2.0);
        break;
    case TIME_DISPLAY_FORMAT::ELAPSED_TIME:
        setMinimumHeight(minimumHeight() / 2.0);
        break;
    default:
        return;
    }
    _axisFormat = format;
    update();
}


/**
 * @brief AxisDisplay::hoverLineUpdated
 * @param visible
 * @param globalPos
 */
void AxisDisplay::hoverLineUpdated(bool visible, QPointF globalPos)
{
    _displayHoverValue = visible;
    if (visible) {
        QPointF pos = mapFromGlobal(globalPos.toPoint());
        if (_orientation == Qt::Horizontal) {
            _hoveredPos = pos.x();
            _hoveredValue = (_hoveredPos / width()) * _displayedRange + _displayedMin;
        } else {
            _hoveredPos = pos.y();
            _hoveredValue = (1.0 - (_hoveredPos / height())) * _displayedRange + _displayedMin;
        }
    }
    update();
}


/**
 * @brief AxisDisplay::themeChanged
 */
void AxisDisplay::themeChanged()
{
    Theme* theme = Theme::theme();
    _labelColor = theme->getTextColor();
    _axisColor = theme->getAltTextColor();

    _hoveredRectColor = theme->getActiveWidgetBorderColor();
    _hoverTextColor = theme->getTextColor();

    update();
}


/**
 * @brief AxisDisplay::updateDisplayedMin
 * This is called when the min slider is moved by the user.
 * @param minRatio
 */
void AxisDisplay::updateDisplayedMin(double minRatio)
{
    displayedMinChanged(minRatio * _range + _min);
}


/**
 * @brief AxisDisplay::updateDisplayedMax
 * This is called when the max slider is moved by the user.
 * @param maxRatio
 */
void AxisDisplay::updateDisplayedMax(double maxRatio)
{
    displayedMaxChanged(maxRatio * _range + _min);
}


/**
 * @brief AxisDisplay::resizeEvent
 * @param event
 */
void AxisDisplay::resizeEvent(QResizeEvent* event)
{
    if ((_orientation == Qt::Horizontal) && (minimumWidth() > 0)) {
        if (_axisFormat == TIME_DISPLAY_FORMAT::ELAPSED_TIME) {
            setTickCount(width() / minimumWidth() / 2.0);
        } else {
            setTickCount(width() / minimumWidth() / 3.0);
        }
    }
    // update tick label locations
    QWidget::resizeEvent(event);
    update();
}


/**
 * @brief AxisDisplay::paintEvent
 * @param event
 */
void AxisDisplay::paintEvent(QPaintEvent* event)
{
    if (!_slider)
        return;

    QPainter painter(this);
    painter.fillRect(rect(), Qt::transparent);
    painter.setPen(_labelColor);

    QVector<QLineF> tickLines;
    auto paintRect = QRectF(rect()); //getAdjustedRect();

    if (_orientation == Qt::Horizontal) {
        paintHorizontal(painter, tickLines, paintRect);
    } else {
        paintVertical(painter, tickLines, paintRect);
    }

    // display the hovered value
    if (_displayHoverValue) {
        QString hoveredStr = getCovertedString(_hoveredValue);
        double offset = _tickLength + _spacing;
        double x = 0, y = 0, w = 0;

        if (_orientation == Qt::Horizontal) {
            w = qMin(paintRect.width() - 1, fontMetrics().width(hoveredStr) + _spacing * 2.0);
            y = _slider->getAlignment() == Qt::AlignTop ? paintRect.top() : paintRect.top() + offset;
            x = _hoveredPos - w / 2.0;
            if (x < 0) {
                x = 0;
            } else if ((x + w) >= width()) {
                x = width() - w - 1;
            }
        } else {
            hoveredStr.truncate(qMin(hoveredStr.length(), _widestTextLength - 1));
            if (hoveredStr.at(hoveredStr.length() - 1) == ".") {
                hoveredStr.truncate(hoveredStr.length() - 1);
            }
            w = fontMetrics().width(hoveredStr) + _spacing * 2.0;
            x = _slider->getAlignment() == Qt::AlignLeft ? width()- w - offset : offset;
            y = _hoveredPos - _textHeight / 2.0;
            if (y < 0) {
                y = 0;
            } else if ((y + _textHeight) >= height()) {
                y = height() - _textHeight - 1;
            }
        }

        QRectF hoveredRect(x, y, w, _textHeight);
        painter.setBrush(_hoveredRectColor);
        painter.setPen(_hoverTextColor);
        painter.drawRect(hoveredRect);
        painter.drawText(hoveredRect, hoveredStr, QTextOption(Qt::AlignCenter));
    }

    painter.setPen(QPen(_axisColor, _penWidth));
    if (_axisLineVisible)
        painter.drawLine(_axisLine);
    if (_tickVisible)
        painter.drawLines(tickLines);
}


/**
 * @brief AxisDisplay::paintHorizontal
 * @param painter
 * @param tickLines
 * @param rect
 */
void AxisDisplay::paintHorizontal(QPainter &painter, QVector<QLineF> &tickLines, QRectF &rect)
{
    double rectWidth = rect.width() / _tickCount;
    double lineLength = _tickLength;
    double tickY = rect.top() + _penWidth / 2.0;
    double offset = _tickLength + _spacing;
    QSizeF rectSize(rectWidth, rect.height() - offset);
    QPointF startPoint(-rectWidth / 2.0 + rect.left(), rect.top() + offset);
    QPointF dateRectCenter(rect.center().x(), rect.center().y() + rectSize.height() / 2.0);

    if (_slider->getAlignment() == Qt::AlignTop) {
        tickY = rect.bottom() - _penWidth / 2.0;
        lineLength = -_tickLength;
        startPoint.setY(rect.top());
        dateRectCenter.setY(rect.center().y() - rectSize.height() / 2.0);
    }
    _axisLine.setLine(rect.left(), tickY, rect.right(), tickY);

    if (_tickCount == 1) {
        startPoint.setX(0);
        dateRectCenter.setX(rect.center().x());
    }

    bool dateTimeFormat = (_valueType == VALUE_TYPE::DATE_TIME) && (_axisFormat == TIME_DISPLAY_FORMAT::DATE_TIME);
    QRectF textRect;
    QDate prevDate;

    /*
     * TODO - Clean up hovered date/time display and paint methods; move slot outside of this class
     */

    double minTextWidth = 0.0;
    double maxTextWidth = 0.0;

    for (int i = 0; i <= _tickCount; i++) {

        textRect = QRectF(startPoint, rectSize);

        // tickX has to be an int so that the hovered value matches the displayed value
        int tickX = textRect.center().x();
        double value = (double)tickX / width() * _displayedRange + _displayedMin;

        if (_tickCount == 1) {
            tickX = dateRectCenter.x();
            value = _displayedMin + _displayedRange / 2.0;
        } else if (i == 0) {
            tickX = _penWidth / 2.0;
            value = _displayedMin;
            minTextWidth = fontMetrics().width(getCovertedString(value));
            textRect = textRect.adjusted(minTextWidth + _penWidth, 0, 0, 0);
            if (_axisFormat == TIME_DISPLAY_FORMAT::ELAPSED_TIME) {
                startPoint += QPointF(rectWidth, 0);
                continue;
            }
        } else if (i == _tickCount) {
            if (_axisFormat == TIME_DISPLAY_FORMAT::ELAPSED_TIME)
                return;
            tickX -= _penWidth / 2.0;
            value = _displayedMax;
            maxTextWidth = fontMetrics().width(getCovertedString(value));
            textRect = textRect.adjusted(0, 0, -maxTextWidth - _penWidth, 0);
        }

        tickLines.append(QLineF(tickX, tickY, tickX, tickY + lineLength));
        painter.drawText(textRect, getCovertedString(value), QTextOption(_textAlignment));
        startPoint += QPointF(rectWidth, 0);

        if (dateTimeFormat) {
            QDate date = QDateTime::fromMSecsSinceEpoch(value).date();
            if (date == prevDate)
                continue;
            auto dateStr = date.toString(DATE_FORMAT);
            textRect.moveCenter(QPointF(textRect.center().x(), dateRectCenter.y()));
            if (_tickCount != 1) {
                if (i == 0) {
                    auto strLengthDiff = fontMetrics().width(dateStr) - minTextWidth;
                    if (strLengthDiff > 0)
                        textRect.moveRight(textRect.right() + strLengthDiff - 5);
                } else if (i == _tickCount) {
                    auto strLengthDiff = fontMetrics().width(dateStr) - maxTextWidth;
                    if (strLengthDiff > 0)
                        textRect.moveLeft(textRect.left() - strLengthDiff + 5);
                }
            }
            painter.drawText(textRect, dateStr, QTextOption(_textAlignment));
            prevDate = date;
        }
    }
}


/**
 * @brief AxisDisplay::paintVertical
 * @param painter
 * @param tickLines
 * @param rect
 */
void AxisDisplay::paintVertical(QPainter &painter, QVector<QLineF> &tickLines, QRectF &rect)
{
    double rectHeight = rect.height() / _tickCount;
    double lineLength = _tickLength;
    double tickX = rect.left();
    double offset = _tickLength + _spacing;
    QPointF startPoint(rect.left() + offset, rect.top() + (_penWidth - rectHeight) / 2.0);

    if (_slider->getAlignment() == Qt::AlignLeft) {
        tickX = rect.right();
        lineLength = -_tickLength;
        startPoint.setX(rect.left());
    }
    _axisLine.setLine(tickX, rect.top(), tickX, rect.bottom());

    QRectF textRect;

    for (int i = 0; i <= _tickCount; i++) {

        textRect = QRectF(startPoint, QSize(rect.width() - offset, rectHeight));

        int tickY = textRect.center().y();
        double value = (1.0 - (double)tickY / height()) * _displayedRange + _displayedMin;
        if (i == 0) {
            tickY = _penWidth / 2.0;
            value = _displayedMax;
            textRect = textRect.adjusted(0, _textHeight, 0, 0);
        } else if (i == _tickCount){
            tickY -= _penWidth / 2.0;
            value = _displayedMin;
            textRect = textRect.adjusted(0, 0, 0, -_textHeight);
        }

        tickLines.append(QLineF(tickX, tickY, tickX + lineLength, tickY));
        painter.drawText(textRect, getCovertedString(value), QTextOption(_textAlignment));
        startPoint += QPointF(0, rectHeight);
    }
}


/**
 * @brief AxisDisplay::displayedMinChanged
 * @param min
 */
void AxisDisplay::displayedMinChanged(double min)
{
    _displayedMin = min;
    _displayedRange = _displayedMax - min;
    rangeChanged();
}


/**
 * @brief AxisDisplay::displayedMaxChanged
 * @param max
 */
void AxisDisplay::displayedMaxChanged(double max)
{
    _displayedMax = max;
    _displayedRange = max - _displayedMin;
    rangeChanged();
}


/**
 * @brief AxisDisplay::rangeChanged
 */
void AxisDisplay::rangeChanged()
{
    // update the vertical axes' width based on the new displayed range
    if (_orientation == Qt::Vertical) {
        double rectHeight = getAdjustedRect().height() / _tickCount;
        int tickY = getAdjustedRect().top();
        int maxLength = 0;
        for (int i = 0; i <= _tickCount; i++) {
            double value = (1.0 - (double)tickY / height()) * _displayedRange + _displayedMin;
            if (i == 0) {
                value = _displayedMax;
            } else if (i == _tickCount){
                value = _displayedMin;
            }
            maxLength = qMax(maxLength, getCovertedString(value).length());
            tickY += rectHeight;
        }

        if (maxLength != _widestTextLength) {
            setFixedWidth(maxLength * fontMetrics().width("0") + _tickLength + _spacing * 2);
            _widestTextLength = maxLength;
        }
    } 
    update();
}


/**
 * @brief AxisDisplay::getDateTimeString
 * @param value
 * @return
 */
QString AxisDisplay::getDateTimeString(double value)
{
    auto displayFormat = TIME_FORMAT;
    auto maxElapedMS = _displayedMax - _displayedMin;
    auto maxElapsedDays = maxElapedMS / 8.64e7;
    auto maxElapsedHours = maxElapedMS / 3.6e6;
    auto maxElapsedMins = maxElapedMS / 6e4;

    if (maxElapsedDays >= 1 || maxElapsedHours > 1) {
        // if the max displayed elapsed time is a day or more, only show the hours/mins
        displayFormat = "hh:mm";
    } else if (maxElapsedMins >= 1) {
        // if the max displayed elapsed time is a minute or more, show the hours/mins/secs
        displayFormat = "hh:mm:ss";
    } else {
        // if it's anything smaller, show all
        displayFormat = "hh:mm:ss.zzz";
    }

    return QDateTime::fromMSecsSinceEpoch(value).toString(displayFormat);
}


/**
 * @brief AxisDisplay::getElapsedTimeString
 * @param value
 * @return
 */
QString AxisDisplay::getElapsedTimeString(double value)
{
    auto msecs = value - _min;
    if (msecs <= 0) {
        return "0ms";
    } else if (msecs < 1000) {
        return QString::number((int)msecs) + "ms";
    }

    auto maxElapedMS = _displayedMax - _min;
    auto maxElapsedDays = maxElapedMS / 8.64e7;
    auto maxElapsedHours = maxElapedMS / 3.6e6;
    auto maxElapsedMins = maxElapedMS / 6e4;

    int d = 0, h = 0, m = 0, s = 0, ms = 0;

    if (maxElapsedDays >= 1) {
        // if the max displayed elapsed time is a day or more, only show the elapsed days/hours
        d = msecs / 8.64e7;
        if (d >= 100)
            return QString::number(d) + "d";
        h = (msecs / 3.6e6) - ((int)d * 24);
    } else if (maxElapsedHours >= 1) {
        // if the max displayed elapsed time is an hour or more, only show the elapsed hours/mins
        h = msecs / 3.6e6;
        if (h >= 10)
            return QString::number(h) + "h";
        m = (msecs / 6e4) - (h * 60);
    } else if (maxElapsedMins >= 1) {
        // if the max displayed elapsed time is a minute or more, only show the elapsed mins/secs
        m = msecs / 6e4;
        s = (msecs / 1e3) - (m * 60);
    } else {
        // if it's anything smaller, show the elapsed secs/msecs
        s = msecs / 1e3;
        ms = msecs - (s * 1000);
    }

    QString elapsedTime = "";
    if (d > 0)
        elapsedTime = QString::number(d) + "d";
    if (h > 0)
        elapsedTime += QString::number(h) + "h";
    if (m > 0)
        elapsedTime += QString::number(m) + "m";
    if (s > 0) {
        if (ms > 0) {
            elapsedTime += QString::number(s) + "." + QString::number(ms) + "s";
        } else {
            elapsedTime += QString::number(s) + "s";
        }
    }

    return elapsedTime;
}


/**
 * @brief AxisDisplay::getCovertedString
 * @param value
 * @return
 */
QString AxisDisplay::getCovertedString(double value)
{
    switch (_axisFormat) {
    case TIME_DISPLAY_FORMAT::ELAPSED_TIME:
        return getElapsedTimeString(value);
    case TIME_DISPLAY_FORMAT::DATE_TIME:
        return getDateTimeString(value);
    default:
        return QString::number(value);
    }
}


/**
 * @brief AxisDisplay::getAdjustedRect
 * @return
 */
QRectF AxisDisplay::getAdjustedRect()
{
    double halfPenWidth = _penWidth / 2.0;
    return rect().adjusted(halfPenWidth, halfPenWidth, -halfPenWidth, -halfPenWidth);
}
