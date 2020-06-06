#ifndef CHART_H
#define CHART_H

#include "../Chart/chartlist.h"
#include "../../Data/Series/eventseries.h"
#include "../../Data/Events/workloadevent.h"
#include "../../Data/Events/portevent.h"
#include "../../../../Controllers/ViewController/viewitem.h"

#include "../../Data/Series/portlifecycleeventseries.h"
#include "../../Data/Series/porteventseries.h"
#include "../../Data/Series/workloadeventseries.h"
#include "../../Data/Series/markereventseries.h"
#include "../../Data/Series/cpuutilisationeventseries.h"
#include "../../Data/Series/memoryutilisationeventseries.h"
#include "../../Data/Series/networkutilisationeventseries.h"

#include <QWidget>
#include <QPen>
#include <QBrush>

namespace MEDEA {

/**
 * @brief The Chart class renders EventSeries based on their kind, which is determined by the kind of Events stored in them.
 * This class can contain multiple EventSeries of different kinds and render them along a shared timeline (date-time) axis.
 * Each kind of series has its own render function and hence, are visualised in different ways.
 */
class Chart : public QWidget
{
    friend class ChartList;
    Q_OBJECT

public:
    explicit Chart(quint32 experimentRunID, qint64 experimentStartTime, QWidget* parent = nullptr);

    quint32 getExperimentRunID() const;

    void addSeries(const QPointer<const MEDEA::EventSeries>& series);
    void removeSeries(ChartDataKind kind);

    QPointer<const EventSeries> getSeries(ChartDataKind kind) const;
    const QHash<ChartDataKind, QPointer<const EventSeries>>& getSeries() const;

    bool isHovered() const;

    QList<ChartDataKind> getHoveredSeriesKinds() const;
    QPair<qint64, qint64> getHoveredTimeRange(ChartDataKind kind) const;

    void updateBinnedData(ChartDataKind kind);
    void updateBinnedData(const QSet<ChartDataKind>& kinds = QSet<ChartDataKind>());

    void updateVerticalMin(double min);
    void updateVerticalMax(double max);
    void updateRange(double startTime, double duration);

    void setRange(double min, double max);

    void setDisplayMinRatio(double ratio);
    void setDisplayMaxRatio(double ratio);
    void setDisplayRangeRatio(double minRatio, double maxRatio);

public slots:
    void setHovered(bool hovered);
    void setHoveredRect(QRectF rect);
    void seriesKindHovered(ChartDataKind kind);

private slots:
    void themeChanged();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    template<class T>
    struct EventSeriesPaintVals {
        //EventSeriesPaintVals(ChartDataKind kind) : series_kind(kind) {}
        ChartDataKind series_kind;
        QHash<T, QPixmap> pixmaps;
        QColor series_color;
    };

    template<class T>
    void paintEventChart(QPainter& painter, const QVector<QList<Event*>>& bins, const EventSeriesPaintVals<T>& paint_vals);
    void paintLineChart();
    void paintBarChart();

    void paintSeries(QPainter& painter, const QPointer<const EventSeries>& series);
    void outlineHoveredData(QPainter& painter);
    void displayDataMinMax(QPainter& painter);

    void paintPortLifecycleSeries(QPainter& painter, const QPointer<const PortLifecycleEventSeries>& series);
    void paintPortEventSeries(QPainter& painter, const QPointer<const PortEventSeries>& series);
    void paintWorkloadEventSeries(QPainter& painter, const QPointer<const WorkloadEventSeries>& series);
    void paintMarkerEventSeries(QPainter& painter, const QPointer<const MarkerEventSeries>& series);
    void paintCPUUtilisationSeries(QPainter& painter, const QPointer<const CPUUtilisationEventSeries>& series);
    void paintMemoryUtilisationSeries(QPainter& painter, const QPointer<const MemoryUtilisationEventSeries>& series);
    void paintNetworkUtilisationSeries(QPainter& painter, const QPointer<const NetworkUtilisationEventSeries>& series);

    QPair<QPair<int, int>, QPair<QList<Event*>::const_iterator, QList<Event*>::const_iterator>>
    getOuterDisplayIterators(const QList<Event*>& events, double target_bin_width) const;

    QVector<QList<Event*>> binEvents(const QList<Event*>& events,
                                     int bin_count,
                                     QList<Event*>::const_iterator first,
                                     QList<Event*>::const_iterator last) const;

    void drawTextRect(QPainter& painter, const QRectF& rect, const QString& text, const QColor& color, ChartDataKind series_kind);
    void drawLineFromRects(QPainter& painter, const QList<QRectF>& rects, const QColor& color, double opacity, ChartDataKind series_kind);

    bool rectHovered(ChartDataKind kind, const QRectF& hitRect);
    bool rectHovered(const QRectF& hitRect);

    void clearHoveredLists();
    void updateSeriesPixmaps();

    int getBinCount(double target_bin_width) const;
    double getBinWidth(double target_bin_width) const;

    double getDisplayMin() const;
    double getDisplayMax() const;

    qint64 mapPixelToTime(double x);
	double mapTimeToPixel(double time);

	static QColor getContrastingColor(const QColor& color);

    quint32 experimentRunID_;
    qint64 experimentRunStartTime_;

    bool containsYRange_ = false;
    bool hovered_ = false;

    double dataMinX_ = DBL_MAX;
    double dataMaxX_ = DBL_MIN;
    double dataMinY_ = DBL_MAX;
    double dataMaxY_ = DBL_MIN;

    double minRatio_ = 0.0;
    double maxRatio_ = 1.0;

    QRectF hoveredRect_;

    QPen defaultTextPen_;
    QPen defaultRectPen_;
    QPen defaultEllipsePen_;
    QPen highlightPen_;
    QBrush highlightBrush_;

    QColor gridColor_;
    QColor textColor_;
    QColor highlightColor_;
    QColor hoveredRectColor_;

    QColor backgroundColor_;
    QColor backgroundDefaultColor_;
    QColor backgroundHighlightColor_;

    QColor defaultPortLifecycleColor_ = Qt::gray;
    QColor defaultWorkloadColor_ = Qt::gray;
    QColor defaultUtilisationColor_ = Qt::lightGray;
    QColor defaultMemoryColor_ = Qt::lightGray;
    QColor defaultMarkerColor_ = Qt::gray;
    QColor defaultPortEventColor_ = Qt::gray;

    QColor defaultNetworkColor_sent_ = Qt::lightGray;
    QColor defaultNetworkColor_received_ = Qt::lightGray;

    QColor portLifecycleColor_ = defaultUtilisationColor_;
    QColor workloadColor_ = defaultWorkloadColor_;
    QColor utilisationColor_ = defaultUtilisationColor_;
    QColor memoryColor_ = defaultMemoryColor_;
    QColor markerColor_ = defaultMarkerColor_;
    QColor portEventColor_ = defaultPortEventColor_;

    QColor networkColor_sent_ = defaultNetworkColor_sent_;
    QColor networkColor_received_ = defaultNetworkColor_received_;
    QColor networkColor_combined_ = Qt::blue;

    double portLifecycleSeriesOpacity_ = 1.0;
    double workloadSeriesOpacity_ = 1.0;
    double cpuSeriesOpacity_ = 1.0;
    double memorySeriesOpacity_ = 1.0;
    double markerSeriesOpacity_ = 1.0;
    double portEventSeriesOpacity_ = 1.0;
    double networkSeriesOpacity_ = 1.0;

    QHash<ChartDataKind, QPointer<const MEDEA::EventSeries>> series_pointers_;

    QHash<ChartDataKind, QPair<qint64, qint64>> hoveredSeriesTimeRange_;
    ChartDataKind hoveredSeriesKind_ = ChartDataKind::DATA;
    QList<QRectF> hoveredEllipseRects_;
    QList<QRectF> hoveredRects_;

    QHash<AggServerResponse::LifecycleType, QPixmap> lifeCycleTypePixmaps_;
    QHash<WorkloadEvent::WorkloadEventType, QPixmap> workloadEventTypePixmaps_;
    QHash<PortEvent::PortEventType, QPixmap> portEventTypePixmaps_;

    EventSeriesPaintVals<AggServerResponse::LifecycleType> port_lifecycle_paint_vals = {ChartDataKind::PORT_LIFECYCLE};
    EventSeriesPaintVals<WorkloadEvent::WorkloadEventType> workload_event_paint_vals;
    EventSeriesPaintVals<AggServerResponse::LifecycleType> port_event_paint_vals;
};

}

#endif // CHART_H