#include "gtest/gtest.h"

// Include the convert functions for the port type
#include "convert.hpp"
#include <middleware/ospl/ineventport.hpp>
#include <middleware/ospl/outeventport.hpp>

//Include the FSM Tester
#include "../../core/activatablefsmtester.h"

void empty_callback(Base::Basic& b){};

bool setup_port(EventPort& port, int domain, std::string topic_name){
    auto d = port.GetAttribute("domain_id").lock();
    auto t = port.GetAttribute("topic_name").lock();
   
    if(d && t){
        d->set_Integer(domain);
        t->set_String(topic_name);
        return true;
    }
    return false;
}

//Define an In/Out Port FSM Tester
class OSPL_InEventPort_FSMTester : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            auto port = new ospl::InEventPort<Base::Basic, Basic>(std::weak_ptr<Component>(),  port_name, empty_callback);
            
            EXPECT_TRUE(setup_port(*port, 0, port_name));

            a = port;
            ASSERT_TRUE(a);
        }
};

class OSPL_OutEventPort_FSMTester : public ActivatableFSMTester{
protected:
    void SetUp(){
        ActivatableFSMTester::SetUp();
        auto port_name = get_long_test_name();
        auto port = new ospl::OutEventPort<Base::Basic, Basic>(std::weak_ptr<Component>(), port_name);
        EXPECT_TRUE(setup_port(*port, 0, port_name));
        a = port;
        ASSERT_TRUE(a);
    }
};


#define TEST_FSM_CLASS OSPL_InEventPort_FSMTester
#include "../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

#define TEST_FSM_CLASS OSPL_OutEventPort_FSMTester
#include "../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

TEST(OSPL_EventportPair, Stable100){
    auto test_name = get_long_test_name();
    auto rx_callback_count = 0;

    
    auto c = std::make_shared<Component>("Test");
    ospl::OutEventPort<Base::Basic, Basic> out_port(c, "tx_" + test_name);
    ospl::InEventPort<Base::Basic, Basic> in_port(c, "rx_" + test_name, [&rx_callback_count](Base::Basic&){
            rx_callback_count ++;
    });

    EXPECT_TRUE(setup_port(out_port, 0, test_name));
    EXPECT_TRUE(setup_port(in_port, 0, test_name));
    

    EXPECT_TRUE(in_port.Configure());
    EXPECT_TRUE(out_port.Configure());
    EXPECT_TRUE(in_port.Activate());
    EXPECT_TRUE(out_port.Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
       Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        out_port.tx(b);
        sleep_ms(1);
    }


    EXPECT_TRUE(in_port.Passivate());
    EXPECT_TRUE(out_port.Passivate());
    EXPECT_TRUE(in_port.Terminate());
    EXPECT_TRUE(out_port.Terminate());

    auto total_rxd = in_port.GetEventsReceieved();
    auto proc_rxd = in_port.GetEventsProcessed();

    auto total_txd = out_port.GetEventsReceived();
    auto total_sent = out_port.GetEventsSent();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(proc_rxd, send_count);
    EXPECT_EQ(rx_callback_count, send_count);
}

//Run a blocking callback which runs for 1 second,
//During that one second, send maximum num
TEST(OSPL_EventportPair, Busy100){
    auto test_name = get_long_test_name();
    auto rx_callback_count = 0;

    auto c = std::make_shared<Component>("Test");
    ospl::OutEventPort<Base::Basic, Basic> out_port(c, "tx_" + test_name);
    ospl::InEventPort<Base::Basic, Basic> in_port(c, "rx_" + test_name, [&rx_callback_count, &out_port](Base::Basic&){
            rx_callback_count ++;
            sleep_ms(1000);
    });
    
    EXPECT_TRUE(setup_port(out_port, 0, test_name));
    EXPECT_TRUE(setup_port(in_port, 0, test_name));


    EXPECT_TRUE(in_port.Configure());
    EXPECT_TRUE(out_port.Configure());
    EXPECT_TRUE(in_port.Activate());
    EXPECT_TRUE(out_port.Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        out_port.tx(b);
    }

    //Sleep for a reasonable time (Bigger than the callback work)
    sleep_ms(500);

    EXPECT_TRUE(out_port.Passivate());
    EXPECT_TRUE(in_port.Passivate());

    EXPECT_TRUE(out_port.Terminate());
    EXPECT_TRUE(in_port.Terminate());

    auto total_rxd = in_port.GetEventsReceieved();
    auto proc_rxd = in_port.GetEventsProcessed();

    auto total_txd = out_port.GetEventsReceived();
    auto total_sent = out_port.GetEventsSent();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(proc_rxd, 1);
    EXPECT_EQ(rx_callback_count, 1);
}

int main(int ac, char* av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}