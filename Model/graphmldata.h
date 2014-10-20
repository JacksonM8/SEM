#ifndef GRAPHMLDATA_H
#define GRAPHMLDATA_H

#include "graphmlkey.h"

#include <QStringList>
#include <QString>

class GraphMLData: public GraphML{
        Q_OBJECT
public:
    GraphMLData(GraphMLKey* key, QString value);
    ~GraphMLData();
    //void setValue(QString value);

    void setValue(QString value);
    void setProtected(bool setProtected);
    bool getProtected();

    void setParentData(GraphMLData* data);
    void unsetParentData();

    void bindData(GraphMLData* data);
    void unbindData(GraphMLData* data);

    QString getValue() const;
    GraphMLKey* getKey();
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();

    QStringList toStringList();
signals:
    void dataChanged(GraphMLData* data);
private:
    GraphMLData* parentData;
    QVector<GraphMLData*> childData;
    QString value;
    bool isProtected;
    GraphMLKey* key;

    // GraphML interface

};

#endif // GRAPHMLDATA_H
