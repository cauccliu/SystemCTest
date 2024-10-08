#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include <systemc.h>
#include <map>
#include <tlm.h>
#include<tlm_utils/simple_initiator_socket.h>
#include<tlm_utils/simple_target_socket.h>
#include <vector>
#include "ringbuffer.h"
extern std::map<uint64_t, uint32_t*> AddrMap;
#define   CP_QUEUE_FREE  0
#define   CP_QUEUE_BUSY  1
#define   CP_DEVICE_SYNC 1
struct Message {
    int queue_id;
    int queue_status;
    int queue_sync;
    uint32_t ib_size;  //ring的前32位值，上次商定为command数量
    uint64_t addr;
};


class AJQueue : public sc_module {
public:
    SC_HAS_PROCESS(AJQueue);
    tlm_utils::simple_target_socket<AJQueue>      socket_cp2queue;
    tlm_utils::simple_initiator_socket<AJQueue>      socket_queue2cp;
    int queue_id;
    int queue_status;

    //sc_event device_sync;
    sc_event received_from_cp;

    AJQueue(const sc_module_name& name) : socket_cp2queue("socket") {
        ib_offset = 0;
        queue_pause = 0;
        socket_cp2queue.register_b_transport(this, &AJQueue::b_transport);
        SC_THREAD(Thread_Queue);
    //dont_initialize();
        sensitive <<  received_from_cp;  
    }

    void Thread_Queue();
    uint32_t* getIbufPtr(uint64_t data);
    virtual void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay);
    void CpIndirectBuffer();//根据indirectbuffer取command，后续可能要添加与global memory交互，考虑溢出,建模最好指针操作，这样不容易错
    int getHeaderBodydws(uint32_t command_header);
    int getHeaderPacketType(uint32_t command_header); //获取packet type  0/1
    int getHeaderDst(uint32_t command_header); //获取包发往的目的地dst
    int getHeaderCmdType(uint32_t command_header); //获取cmd_type
    void CpCommandHeader(uint32_t command_header);
    void CpToCu(); //指向cu的包的处理
    void CpEventSync(int cmd_type); //指向event_sync包的处理
    void CpDeviceSync(); //表示cmd_type=2时，devcie sync的处理
    void CpQueueRelease(); //当前queue内是否还有任务，无的话，释放queue
    void QueueReportCp(int StatusToCp);

private:
    Message mes;
    int queue_sync;
    
    uint32_t* ibuf_ptr;
    uint32_t ib_size;
    int ib_offset ; //记录当前ib的offset，解析完后清零
    int queue_pause ;     
};

