#ifndef WORKERS_DIS_DISWORKER_H
#define WORKERS_DIS_DISWORKER_H

#include <memory>
#include <core/worker.h>
#include <functional>
#include "kdis/KDIS/Extras/PDU_Factory.h"

class Dis_Worker_Impl;
class Dis_Worker : public Worker{
    public:
        Dis_Worker(const BehaviourContainer& container, const std::string& inst_name);
        ~Dis_Worker();
        const std::string& get_version() const override;
        void SetPduCallback(std::function<void (const KDIS::PDU::Header &)> func);
        std::string PDU2String(const KDIS::PDU::Header& header);
        bool Connect();
        void Disconnect();
    protected:
        void HandleConfigure() override;
        void HandleTerminate() override;
    private:
        void Connect_();
        void Disconnect_();
        std::unique_ptr<Dis_Worker_Impl> impl_;

        std::shared_ptr<Attribute> ip_address_;
        std::shared_ptr<Attribute> port_;
};

#endif //WORKERS_DIS_DISWORKER_H
