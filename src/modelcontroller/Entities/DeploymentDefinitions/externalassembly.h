#ifndef MEDEA_EXTERNALASSEMBLY_H
#define MEDEA_EXTERNALASSEMBLY_H

#include "eventportassembly.h"
class EntityFactoryRegistryBroker;

namespace MEDEA{
    class ExternalAssembly : public EventPortAssembly
    {
        Q_OBJECT
        public:
            static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
        protected:
            ExternalAssembly(EntityFactoryBroker& factory, bool is_temp_node);
        public:
            bool canAcceptEdge(EDGE_KIND edge_kind, Node *dst);
            bool canAdoptChild(Node* child);
        private:
            void MiddlewareUpdated();
            Node* in_ = 0;
            Node* out_ = 0;
    };
};

#endif // MEDEA_EXTERNALASSEMBLY_H
