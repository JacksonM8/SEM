#ifndef ENTITYCHART_H
#define ENTITYCHART_H

#include "../Chart/timelinechart.h"
#include "../../Data/Series/eventseries.h"
#include "../../Data/Events/workloadevent.h"
#include "../../../../Controllers/ViewController/viewitem.h"

#include <QWidget>
#include <QPen>
#include <QBrush>

class EntityChart : public QWidget
{
    friend class TimelineChart;
    Q_OBJECT

public:
    explicit EntityChart(quint32 experimentRunID, QWidget* parent = 0);

    quint32 getExperimentRunID();

    void addSeries(MEDEA::EventSeries *series);
    void removeSeries(TIMELINE_DATA_KIND kind);

    bool isHovered();

    const QHash<TIMELINE_DATA_KIND, MEDEA::EventSeries*>& getSeries();
    const QList<TIMELINE_DATA_KIND> getHovereSeriesKinds();
    const QPair<qint64, qint64> getHoveredTimeRange(TIMELINE_DATA_KIND kind);

    void setRange(double min, double max);

    void setDisplayMinRatio(double ratio);
    void setDisplayMaxRatio(double ratio);
    void setDisplayRangeRatio(double minRatio, double maxRatio);

    void updateChartHeight(double height);
    void updateBinnedData(TIMELINE_DATA_KIND kind);
    void updateBinnedData(QSet<TIMELINE_DATA_KIND> kinds);

public slots:
    void setHovered(bool visible);
    void setHoveredRect(QRectF rect);
    void seriesKindHovered(TIMELINE_DATA_KIND kind);
    void setSeriesKindVisible(TIMELINE_DATA_KIND kind, bool visible);

private slots:
    void themeChanged();

protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);

private:
    void paintSeries(QPainter& painter, TIMELINE_DATA_KIND kind);
    void paintPortLifecycleEventSeries(QPainter& painter);
    void paintWorkloadEventSeries(QPainter& painter);
    void paintCPUUtilisationEventSeries(QPainter& painter);
    void paintMemoryUtilisationEventSeries(QPainter &painter);

    void paintPortLifecycleSeries(QPainter& painter);

    bool rectHovered(TIMELINE_DATA_KIND kind, const QRectF& hitRect);
    bool rectHovered(const QRectF& hitRect);

    void clearHoveredLists();

    void updateSeriesPixmaps();

    int getBinIndexForTime(double time);
    QVector<QList<MEDEA::Event*>>& getBinnedData(TIMELINE_DATA_KIND kind);

    qint64 mapPixelToTime(double x);
    double mapTimeToPixel(double time);

    quint32 _experimentRunID;

    bool containsYRange_ = false;
    bool hovered_ = false;

    double displayMin_;
    double displayMax_;
    double dataMinX_;
    double dataMaxX_;
    double dataMinY_;
    double dataMaxY_;
    double dataHeight_ = 0;

    double minRatio_ = 0.0;
    double maxRatio_ = 1.0;

    QPixmap messagePixmap_;
    QRectF hoveredRect_;

    QColor gridColor_;
    QColor textColor_;
    QColor backgroundColor_;
    QColor highlightColor_;
    QColor highlightTextColor_;
    QColor hoveredRectColor_;

    QColor defaultPortLifecycleColor_ = Qt::gray;
    QColor defaultWorkloadColor_ = Qt::gray;
    QColor defaultUtilisationColor_ = Qt::lightGray;
    QColor defaultMemoryColor_ = Qt::lightGray;

    QColor portLifecycleColor_ = defaultUtilisationColor_;
    QColor workloadColor_ = defaultWorkloadColor_;
    QColor utilisationColor_ = defaultUtilisationColor_;
    QColor memoryColor_ = defaultMemoryColor_;

    QHash<TIMELINE_DATA_KIND, bool> seriesKindVisible_;
    QHash<TIMELINE_DATA_KIND, MEDEA::EventSeries*> seriesList_;

    //QHash<TIMELINE_DATA_KIND, QVector< QList<MEDEA::Event*> >> binnedData_;
    QVector<QList<MEDEA::Event*>> portLifecycleBinnedData_;
    QVector<QList<MEDEA::Event*>> workloadBinnedData_;
    QVector<QList<MEDEA::Event*>> cpuUtilisationBinnedData_;
    QVector<QList<MEDEA::Event*>> memoryUtilisationBinnedData_;

    QHash<TIMELINE_DATA_KIND, QPair<qint64, qint64>> hoveredSeriesTimeRange_;
    TIMELINE_DATA_KIND hoveredSeriesKind_;

    QHash<LifecycleType, QPixmap> lifeCycleTypePixmaps_;
    QHash<WorkloadEvent::WorkloadEventType, QPixmap> workloadEventTypePixmaps_;

    /*
    struct Series{
        MEDEA::DataSeries* series = 0;
        bool is_visible = false;
    }
    QHash<TIMELINE_DATA_KIND, Series> series_;
    */

};

#endif // ENTITYCHART_H
