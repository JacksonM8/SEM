#ifndef TIMELINECHARTVIEW_H
#define TIMELINECHARTVIEW_H

#include "../../../../Controllers/ViewController/viewitem.h"
#include "../../Timeline/Chart/hoverpopup.h"
#include "../Axis/chartlabellist.h"
#include "../Axis/chartlabel.h"
#include "chart.h"

#include "../../Data/Events/portlifecycleevent.h"
#include "../../Data/Events/workloadevent.h"
#include "../../Data/Events/cpuutilisationevent.h"
#include "../../Data/Events/memoryutilisationevent.h"
#include "../../Data/Events/markerevent.h"
#include "../../Data/Events/portevent.h"
#include "../../Data/Events/networkutilisationevent.h"

//#include "../../Data/experimentdata.h"
#include "../../Data/Series/portlifecycleeventseries.h"
#include "../../Data/Series/workloadeventseries.h"
#include "../../Data/Series/cpuutilisationeventseries.h"
#include "../../Data/Series/memoryutilisationeventseries.h"
#include "../../Data/Series/markereventseries.h"
#include "../../Data/Series/porteventseries.h"
#include "../../Data/Series/networkutilisationeventseries.h"
#include "../../Data/experimentrundata.h"

#include <QWidget>
#include <QToolBar>
#include <QPushButton>
#include <QScrollArea>
#include <QMultiHash>

enum class VALUE_TYPE{DOUBLE, DATE_TIME};
enum class TIME_DISPLAY_FORMAT{VALUE, DATE_TIME, ELAPSED_TIME};

class AxisWidget;
class TimelineChartView : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineChartView(QWidget* parent = nullptr);

    bool eventFilter(QObject *watched, QEvent* event) override;

    void addChart(QPointer<const MEDEA::EventSeries> series, const AggServerResponse::ExperimentRun& experiment_run);

    void addChart(QPointer<const MEDEA::EventSeries> series, const MEDEA::ExperimentRunData& exp_run_data);
    //void removeChart(const QString& id, bool clearing_chart_list = false);

    void addPortLifecycleEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<PortLifecycleEvent*>& events);
    void addWorkloadEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<WorkloadEvent*>& events);
    void addCPUUtilisationEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<CPUUtilisationEvent*>& events);
    void addMemoryUtilisationEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<MemoryUtilisationEvent*>& events);
    void addMarkerEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<MarkerEvent*>& events);
    void addPortEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<PortEvent*>& events);
    void addNetworkUtilisationEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<NetworkUtilisationEvent*>& events);

    void updateExperimentRunLastUpdatedTime(quint32 experimentRunID, qint64 time);

    void setTimeDisplayFormat( TIME_DISPLAY_FORMAT format);

    void clearChartList();

signals:
    void seriesLegendHovered(MEDEA::ChartDataKind kind);

public slots:
    void themeChanged();

    void toggledSeriesLegend(bool checked);

    void chartLabelListSizeChanged(QSizeF size);
    void chartHovered(MEDEA::Chart* chart, bool hovered);
    void chartClosed();

    void updateHoverDisplay();

private slots:
    void minSliderMoved(double ratio);
    void maxSliderMoved(double ratio);

    void timelineZoomed(int delta);
    void timelinePanned(double dx, double dy);
    void timelineRubberbandUsed(double left, double right);
    
private:
    void addEvent(MEDEA::ChartDataKind kind, const AggServerResponse::ExperimentRun& experimentRun, MEDEA::Event* event);
    void addedEvents(const AggServerResponse::ExperimentRun& experimentRun);

    MEDEA::EventSeries* getSeriesForEventKind(MEDEA::ChartDataKind kind, const AggServerResponse::ExperimentRun& experimentRun, const QString& eventSeriesID) const;
    MEDEA::EventSeries* constructSeriesForEventKind(MEDEA::ChartDataKind kind, const AggServerResponse::ExperimentRun& experimentRun, const QString& eventSeriesID, const QString& label);

    MEDEA::Chart* constructChartForSeries(MEDEA::EventSeries *series, const QString& ID, const QString& label);
    void removeChart(const QString& ID, bool clearing = false);

    void updateRangeForExperimentRun(quint32 experimentRunID, qint64 startTime, qint64 lastUpdatedTime);
    void removedDataFromExperimentRun(quint32 experimentRunID);
    void updateTimelineRange(bool updateDisplayRange = true);

    const QString& getDateTimeDisplayFormat(const MEDEA::ChartDataKind& kind) const;

    void setupLayout();

    QLabel* emptyLabel_ = nullptr;
    QWidget* mainWidget_ = nullptr;

    MEDEA::ChartList* chartList_ = nullptr;
    MEDEA::ChartLabelList* chartLabelList_ = nullptr;
    AxisWidget* timelineAxis_ = nullptr;
    HoverPopup* hoverDisplay_ = nullptr;

    QToolBar* legendToolbar_ = nullptr;
    QScrollArea* scrollArea_ = nullptr;

    QWidget* topFillerWidget_ = nullptr;
    QWidget* bottomFillerWidget_ = nullptr;
    QWidget* hoverWidget_ = nullptr;

    TIME_DISPLAY_FORMAT timeDisplayFormat_ = TIME_DISPLAY_FORMAT::DATE_TIME;

    QHash<MEDEA::ChartDataKind, QAction*> legendActions_;
    QHash<MEDEA::ChartDataKind, QPushButton*> hoverDisplayButtons_;

    QHash<qint32, int> experimentRunSeriesCount_;
    QHash<quint32, QPair<qint64, qint64>> experimentRunTimeRange_;
    QPair<quint32, qint64> longestExperimentRunDuration_;
    QPair<qint64, qint64> totalTimeRange_;

    // MEDEA::Event related widgets/series
    QHash<QString, MEDEA::ChartLabel*> chartLabels_;
    QHash<QString, MEDEA::Chart*> charts_;
    QMultiHash<QString, MEDEA::EventSeries*> seriesList_;

    QHash<QString, QPointer<const MEDEA::EventSeries>> series_pointers_;
    static const MEDEA::ChartDataKind no_data_kind_;
};

#endif // TIMELINECHARTVIEW_H