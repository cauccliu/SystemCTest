#include <string>
#include <map>
#include <vector>
#include <systemc.h>
#include "cp.h"
#include "ajqueue.h"
#include "ringbuffer.h"
using namespace sc_core;
using namespace sc_dt;
using namespace std;
using std::map;

extern std::map<uint64_t, uint32_t*> AddrMap;

class Top : public sc_module {
public:
    SC_HAS_PROCESS(Top);
    uint32_t* ptr;  
    uint64_t ring_data = 0xA;
    sc_event wptr_update;

  Top(const sc_module_name& name){

    //IndirectBuffer buffer(5);
    //indirbuf = &buffer;
    ptr = new uint32_t[5];
    for (size_t i = 0; i < 5; ++i) {
        ptr[i] = i + 5;
    }

    AddrMap[ring_data] = ptr;
    //AddrMap[ring_data1] = ptr1;
    RingBuffer rbuf(5);
    rbuf.add(ring_data);

    cp_test = new AJCpModule("cp_test");
    cp_test->rbuf = ring_data;

    for (int i = 0; i < QNUM; ++i) {
        char queue_name[20];
        sprintf(queue_name, "queues_%d", i);
        queues[i] =new AJQueue(queue_name);
        queues[i]->queue_id = i;
        queues[i]->queue_status = 0;
        cp_test->socket_cp2queue[i].bind( queues[i]->socket_cp2queue );
        queues[i]->socket_queue2cp.bind(cp_test->socket_queue2cp[i]);
    }
    //cp_test->initEvent(queues[0]->device_sync) ;
    cp_test->initWptr(wptr_update); 

    SC_THREAD(NotifyWptr);   
    // sensitive <<  wptr_update;    
  }

  void NotifyWptr(){
    while(1){
      wait(3, SC_NS);
      wptr_update.notify();
    }
  }

  ~Top(){
    delete ptr;
    //delete ptr1;
  }

  private:
    AJCpModule *cp_test;
    AJQueue    *queues[QNUM];

};

int sc_main(int argc, char **argv){
  Top top("top");
  sc_start(15,SC_NS);
  return 0;
}

