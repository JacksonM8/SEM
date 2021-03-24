//
// Created by Cathlyn Aston on 28/10/20.
//

#ifndef PULSE_MODEL_ENTITY_H
#define PULSE_MODEL_ENTITY_H

#include <QObject>

namespace Pulse::Model {

class Entity : public QObject {
    Q_OBJECT

public:
    virtual ~Entity() = 0;

signals:
    void nameChanged(const QString& name);
};

inline Entity::~Entity() = default;

} // end Pulse::Model namespace
#endif // PULSE_MODEL_ENTITY_H