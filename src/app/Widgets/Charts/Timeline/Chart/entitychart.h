#ifndef ENTITYCHART_H
#define ENTITYCHART_H

#include "../Chart/timelinechart.h"
#include "../../Series/dataseries.h"
#include "../../Data/Events/workloadevent.h"
//#include "../../Data/Series/workloadeventseries.h"

#include <QWidget>
#include <QPen>
#include <QBrush>

class EntityChart : public QWidget
{
    friend class TimelineChart;
    Q_OBJECT

public:
    explicit EntityChart(ViewItem* item = 0, QWidget* parent = 0);

    ViewItem* getViewItem();
    int getViewItemID();

    void setExperimentRunID(quint32 ID);
    quint32 getExperimentRunID();

    void addEventSeries(MEDEA::EventSeries* series);
    void removeEventSeries(TIMELINE_DATA_KIND kind);

    void addSeries(MEDEA::DataSeries* series);
    void removeSeries(TIMELINE_DATA_KIND seriesKind);

    bool isHovered();

    const QHash<TIMELINE_DATA_KIND, MEDEA::EventSeries*>& getSeries();
    const QList<TIMELINE_DATA_KIND> getHovereSeriesKinds();
    const QPair<qint64, qint64> getHoveredTimeRange(TIMELINE_DATA_KIND kind);

    QPair<double, double> getRangeX();
    QPair<double, double> getRangeY();

    void setRange(double min, double max);

    void setDisplayMinRatio(double ratio);
    void setDisplayMaxRatio(double ratio);
    void setDisplayRangeRatio(double minRatio, double maxRatio);

signals:
    void dataAdded(QList<QPointF> points);

public slots:
    void setHovered(bool visible);
    void setHoveredRect(QRectF rect);
    void seriesKindHovered(TIMELINE_DATA_KIND kind);
    void setSeriesKindVisible(TIMELINE_DATA_KIND kind, bool visible);

private slots:
    void themeChanged();

    void rangeXChanged(double min, double max);
    void rangeYChanged(double min, double max);
    void pointsAdded(QList<QPointF> points);

protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);

private:
    void paintSeries(QPainter& painter, TIMELINE_DATA_KIND kind);
    void paintPortLifecycleEventSeries(QPainter& painter);
    void paintWorkloadEventSeries(QPainter& painter);
    void paintCPUUtilisationEventSeries(QPainter& painter);
    void paintMemoryUtilisationEventSeries(QPainter &painter);
    void paintNotificationSeries(QPainter &painter);
    void paintStateSeries(QPainter &painter);
    void paintBarSeries(QPainter &painter);
    void paintBarData(QPainter &painter, const QRectF &barRect, const QColor &color, const QVector<double> &data);

    bool rectHovered(TIMELINE_DATA_KIND kind, const QRectF& hitRect);
    bool rectHovered(const QRectF& hitRect);

    void clearHoveredLists();

    void setPointWidth(double width);
    double getPointWidth(TIMELINE_DATA_KIND kind);

    void updateSeriesPixmaps();

    qint64 mapPixelToTime(double x);
    double mapTimeToPixel(double time);

    quint32 experimentRunID_;

    bool _useDataRange = false;
    bool _containsYRange = false;
    bool _hovered = false;
    bool _xRangeSet = false;

    double _pointWidth = 12;

    double _displayMin;
    double _displayMax;
    double _dataMinX;
    double _dataMaxX;
    double _dataMinY;
    double _dataMaxY;

    double _minRatio = 0.0;
    double _maxRatio = 1.0;

    ViewItem* _viewItem;

    QPixmap _messagePixmap;
    QPointF _cursorPoint;
    QRectF _hoveredRect;

    QColor _pointColor;
    QColor _pointBorderColor;
    QColor _backgroundColor;
    QColor _backgroundHighlightColor;
    QColor _highlightColor;
    QColor _highlightBrushColor;
    QColor _highlightTextColor;
    QColor _hoveredRectColor;
    QColor _barColor;

    QColor _defaultStateColor;
    QColor _defaultNotificationColor;
    QColor _defaultLineColor;
    QColor _stateColor;
    QColor _notificationColor;
    QColor _lineColor;

    QColor _defaultPortLifecycleColor = Qt::gray;
    QColor _defaultWorkloadColor = Qt::gray;
    QColor _defaultUtilisationColor = Qt::lightGray;
    QColor _defaultMemoryColor = Qt::lightGray;
    QColor _portLifecycleColor = _defaultUtilisationColor;
    QColor _workloadColor = _defaultWorkloadColor;
    QColor _utilisationColor = _defaultUtilisationColor;
    QColor _memoryColor = _defaultMemoryColor;

    int _borderColorDelta;
    int _colorDelta;
    int _color_s_state;
    int _color_s_notification;
    int _color_s_line;
    int _color_v_state;
    int _color_v_notification;
    int _color_v_line;

    QPen _gridPen;
    QPen _pointPen;
    QPen _pointBorderPen;
    QPen _highlightPointPen;
    QPen _hoverLinePen;
    QPen _highlightPen;

    /*
    struct Series{
        MEDEA::DataSeries* series = 0;
        bool is_visible = false;
    }
    QHash<TIMELINE_DATA_KIND, Series> series_;
    */

    QMap<LifecycleType, QPixmap> _lifeCycleTypePixmaps;
    QHash<WorkloadEvent::WorkloadEventType, QPixmap> _workloadEventTypePixmaps;

    QHash<TIMELINE_DATA_KIND, bool> _seriesKindVisible;
    QHash<TIMELINE_DATA_KIND, MEDEA::EventSeries*> _seriesList;
    QHash<TIMELINE_DATA_KIND, QList<QPointF>> _seriesPoints;
    QHash<TIMELINE_DATA_KIND, QList<QPointF>> _mappedPoints;

    QHash<TIMELINE_DATA_KIND, QPair<qint64, qint64>> _hoveredSeriesTimeRange;
    TIMELINE_DATA_KIND _hoveredSeriesKind;
};

#endif // ENTITYCHART_H
