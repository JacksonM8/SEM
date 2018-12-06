#include "barseries.h"

#include <QDebug>

/**
 * @brief MEDEA::BarSeries::BarSeries
 * @param item
 */
MEDEA::BarSeries::BarSeries(ViewItem *item)
    : DataSeries(item, TIMELINE_SERIES_KIND::BAR)
{

}


/**
 * @brief MEDEA::BarSeries::~BarSeries
 */
MEDEA::BarSeries::~BarSeries()
{
}


/**
 * @brief MEDEA::BarSeries::addData
 * @param timeStamp
 * @param data
 * @param windowSize
 */
void MEDEA::BarSeries::addData(QDateTime timeStamp, QVector<double> data, QSize windowSize)
{
    addData(timeStamp.toMSecsSinceEpoch(), data, windowSize);
}


/**
 * @brief MEDEA::BarSeries::addData
 * @param dateTimeMSecsSinceEpoch
 * @param data
 * @param windowSize
 */
void MEDEA::BarSeries::addData(qint64 dateTimeMSecsSinceEpoch, QVector<double> data, QSize windowSize)
{
    if (!data.isEmpty()) {
        auto data_value = new MEDEA::BarData(dateTimeMSecsSinceEpoch, data);
        data_map_.insert(data_value->getTime(), data_value);

        for (double d : data) {
            DataSeries::addPoint(QPointF(dateTimeMSecsSinceEpoch, d));
        }

        /*()
        for (double d : data) {
            DataSeries::addPoint(QPointF(dateTimeMSecsSinceEpoch, d));
        }
        _dataMap.insert(dateTimeMSecsSinceEpoch, data);*/
        //emit dataAdded({data});
        emit dataAdded2();
    }
}


/**
 * @brief MEDEA::BarSeries::getData
 * @return
 */
QMap<qint64, QVector<double> > MEDEA::BarSeries::getData()
{
    return _dataMap;
}


/**
 * @brief MEDEA::BarSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @return
 */
QString MEDEA::BarSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS)
{
    qDebug() << "from: " << QDateTime::fromMSecsSinceEpoch(fromTimeMS).toString("MMMM d, hh:mm:ss:zzzzz");
    qDebug() << "to: " << QDateTime::fromMSecsSinceEpoch(toTimeMS).toString("MMMM d, hh:mm:ss:zzzzz");
    return getHoveredDataString(fromTimeMS, toTimeMS, "MMMM d, hh:mm:ss:zzzzz");
}


/**
 * @brief MEDEA::BarSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @param displayFormat
 * @return
 */
QString MEDEA::BarSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, QString displayFormat)
{
    const auto& data = getConstData2();
    auto current = data.lowerBound(fromTimeMS);
    auto upper = data.upperBound(toTimeMS);

    int count = std::distance(current, upper);
    if (count <= 0) {
        return "";
    } else if (count == 1) {
        hovereData_ = "";
        QTextStream stream(&hovereData_);
        for (;current != upper; current++) {
            stream << QDateTime::fromMSecsSinceEpoch(current.key()).toString(displayFormat) << "\n";
        }
        return hovereData_.trimmed();
    } else {
        return QString::number(count);
    }
}


/**
 * @brief MEDEA::BarSeries::getConstData
 * @return
 */
const QMap<qint64, QVector<double>>& MEDEA::BarSeries::getConstData()
{
    return _dataMap;
}

