#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>

#include <google/protobuf/util/time_util.h>
#include <comms/aggregationrequester/aggregationrequester.h>

#include "../../Widgets/Charts/Data/Events/portlifecycleevent.h"
#include "../../Widgets/Charts/Data/Events/workloadevent.h"

class AggregationProxy : public QObject
{
    Q_OBJECT

public:
    explicit AggregationProxy();
    ~AggregationProxy();

    void SetServerEndpoint(QString endpoint);
    void SetRequestExperimentRunID(quint32 experimentRunID);
    void SetRequestEventKinds(QList<TIMELINE_DATA_KIND> kinds);

    void RequestExperiments();
    void ReloadExperiments();

    void RequestExperimentRuns(QString experimentName = "");
    void RequestExperimentState(quint32 experimentRunID);
    void RequestAllEvents();

    void RequestPortLifecycleEvents(PortLifecycleRequest request);
    void RequestWorkloadEvents(WorkloadRequest request);

    static std::unique_ptr<google::protobuf::Timestamp> constructTimestampFromMS(qint64 milliseconds);
    static const QDateTime getQDateTime(const google::protobuf::Timestamp &time);
    static const QString getQString(const std::string &string);

signals:
    void setChartUserInputDialogVisible(bool visible);
    void requestedExperimentRuns(QList<ExperimentRun> runs);
    void requestedExperimentState(QStringList nodeHostname, QStringList componentName, QStringList workerName);

    void receivedPortLifecycleEvent(PortLifecycleEvent* event);
    void receivedPortLifecycleEvents(QList<MEDEA::Event*> events);

    void receivedWorkloadEvent(WorkloadEvent* event);
    void receivedWorkloadEvents(QList<MEDEA::Event*> events);

    void clearPreviousEvents();
    void receivedAllEvents();

private:
    bool GotRequester();
    void ResetRequestFilters();

    void SendRequests();
    void SendPortLifecycleRequest(AggServer::PortLifecycleRequest& request);
    void SendWorkloadRequest(AggServer::WorkloadRequest& request);

    Port convertPort(const AggServer::Port port);
    LifecycleType getLifeCycleType(const AggServer::LifecycleType type);
    Port::Kind getPortKind(const AggServer::Port_Kind kind);

    WorkerInstance convertWorkerInstance(const AggServer::WorkerInstance inst);
    WorkloadEvent::WorkloadEventType getWorkloadEventType(const AggServer::WorkloadEvent_WorkloadEventType type);

    bool hasSelectedExperimentID_ = false;
    quint32 experimentRunID_;
    QString componentName_;
    QString workerName_;

    QList<TIMELINE_DATA_KIND> requestEventKinds_;

    AggServer::Requester* requester_ = 0;

};

#endif // MEDEA_AGGREGATIONPROXY_H
