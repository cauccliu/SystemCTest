#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include <systemc.h>
#include <tlm.h>
#include<tlm_utils/simple_initiator_socket.h>
#include<tlm_utils/simple_target_socket.h>

#include "ringbuffer.h"
#include "ajqueue.h"
using namespace sc_core;
#define QNUM 32


//static std::map<int, AJQUEUE*> QueueMap;

class AJCpModule : public sc_module {
public:
    SC_HAS_PROCESS(AJCpModule);
    tlm_utils::simple_initiator_socket<AJCpModule> socket_cp2queue[QNUM];
    tlm_utils::simple_target_socket<AJCpModule> socket_queue2cp[QNUM];

    int queue_id;
    int queue_status;
    uint64_t rbuf;

    AJCpModule(const sc_module_name& name) : socket_cp2queue(),socket_queue2cp(){
        queue_id=0;
        queue_status = CP_QUEUE_FREE;
        queue_sync = 0;
        all_queue_status = 0;//记录所有queue的工作状态
        queue_mask=0;//记录32条queue内，收到sync的状态
        //dont_initialize();
        for(int i=0;i<QNUM;++i){
            socket_queue2cp[i].register_b_transport(this, &AJCpModule::b_transport);
        }
    }

    void end_of_elaboration() {//保证启动仿真前，wptr_update被传进来
        SC_THREAD(thread_process);
        sensitive <<  device_sync << *wptr_update;
    }
    
    void initWptr(sc_event& event){
        wptr_update = &event;
    }

    int SearchQueue(); //-----当下只适合启动一条queue的情况，多条queue下要分着给指定的queue，需要在top层给指定queue发包
        
    void thread_process(); //状态机

    void CPQueueStart(tlm::tlm_generic_payload* trans,sc_time delay);//收到来自top的通知后，将资源发给一条queue

   virtual void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay);//queue向cp发送

    void CPQueueManage(tlm::tlm_generic_payload* trans,sc_time delay);
private: 
    Message mes;
    int queue_sync;
    uint32_t all_queue_status;//记录所有queue的工作状态
    uint32_t queue_mask;//记录32条queue内，收到sync的状态
    sc_event device_sync;
    sc_event* wptr_update;
    uint32_t event_status0;//这里用32bit数据来表示32个event id状态，第1位为0，代表event id=0的状态为0，实际须当有1024个event
};

