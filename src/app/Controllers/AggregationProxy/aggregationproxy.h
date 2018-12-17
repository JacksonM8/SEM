#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>

#include <google/protobuf/util/time_util.h>
#include <comms/aggregationrequester/aggregationrequester.h>

#include "../../Widgets/Charts/Data/Events/protomessagestructs.h"

class AggregationProxy : public QObject
{
    Q_OBJECT

public:
    explicit AggregationProxy();
    ~AggregationProxy();
    void SetServerEndpoint(QString endpoint);

    void RequestRunningExperiments();
    void RequestExperimentRuns(QString experimentName = "");
    void RequestExperimentState(quint32 experimentRunID);
    void RequestEvents(quint32 ID, QString componentName);
    void ReloadRunningExperiments();

    static std::unique_ptr<google::protobuf::Timestamp> constructTimestampFromMS(qint64 milliseconds);
    static std::string constructStdStringFromQString(QString s);

    static const QDateTime getQDateTime(const google::protobuf::Timestamp &time);
    static const QString getQString(const std::string &string);

signals:
    void setChartUserInputDialogVisible(bool visible);
    void requstedExperimentRuns(QList<ExperimentRun> runs);
    void requstedComponentNames(QStringList names);

    void clearPreviousEvents();
    void receivedAllEvents();

public slots:
    void setSelectedExperimentRunID(quint32 ID);

private:
    bool hasSelectedExperimentID_ = false;
    quint32 experimentRunID_;
    QStringList componentNames_;


    AggServer::Requester* requester_ = 0;

};

#endif // MEDEA_AGGREGATIONPROXY_H
