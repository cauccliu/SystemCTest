#define SC_INCLUDE_DYNAMIC_PROCESS

#include <systemc>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

class Initiator : public sc_module {
public:
  SC_HAS_PROCESS(Initiator);
  tlm_utils::simple_initiator_socket<Initiator> socket;
  Initiator(const sc_module_name& name) : socket("socket"){
    SC_THREAD(thread_process);
  }
  void thread_process(){
    tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
    sc_time delay = sc_time(10,SC_NS);

    for(int i=32;i<96;i+=4){
      /*
        每个generic payload有一组标准总线属性：
          command：generic payload支持两个命令，read和write。
          address：数据读写的低地址（可理解为开始地址）。
          data：该属性为一个指向Initiator中data buffer的指针
          byte enables：该属性给定数据队列的字节长度（可理解为一次读写的数据大小）。
          streaming width，
          response status。
      */
      tlm::tlm_command cmd = static_cast<tlm::tlm_command> (rand()%2); // 在本例中，命令属性被随机设置为read或者write。
      // 在write命令下，数据会从data array拷贝到target；在read命令下，数据会从target拷贝到data array。
      if(cmd == tlm::TLM_WRITE_COMMAND){
        data = 0xFF000000 | i; 
      }

      trans->set_command(cmd);
      trans->set_address(i); // 该属性是数据读写的低地址（可理解为开始地址），在本例中，address被设置为循环索引
      trans->set_data_ptr(reinterpret_cast<unsigned char*>(&data)); // 该属性为一个指向Initiator中data buffer的指针
      trans->set_data_length(4);
      trans->set_streaming_width(4); // 该属性指明一个流突发请求的宽度
      trans->set_byte_enable_ptr(0); // 当该属性为0时，说明并未使用byte enable。
      trans->set_dmi_allowed(false); // 该方法用于设置DMI hint，每次使用时都应首先初始化为false。DMI hint属性可能由target进行设置，以指明其支持直接内存接口。
      trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE); // 该属性应总是初始化为TLM_INCOMPLETE_RESPONSE


      socket->b_transport(*trans,delay);

      // 在调用b_transport后，Initiator会检查响应状态
      if (trans->is_response_error()) {
        SC_REPORT_ERROR("TLM 2.0", "Response error from b_transport");
      }

      cout << "trans = { " << (cmd ? 'W' : 'R') << ", " << hex << i
           << " } , data = " << hex << data << " at time " << sc_time_stamp()
           << " delay = " << delay << endl;
      wait(delay);
    }
  }

private:
  int data;
};

class Memory : public sc_module {
public:
  SC_HAS_PROCESS(Memory);
  tlm_utils::simple_target_socket<Memory> socket;
  enum {SIZE = 256};

  // 当使用simple_target_socket时，可以通过给target的socket注册一个函数回调，实现函数调用
  Memory(const sc_module_name& name) : socket("socket") {
      socket.register_b_transport(this, &Memory::b_transport); // 接下来target就是需要实现b_transport方法。
      for (int i = 0; i < SIZE; i++) {
        mem[i] = 0xAA000000 | (rand() % SIZE);
      }
  }

  virtual void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay){
    tlm::tlm_command  cmd = trans.get_command();
    sc_dt::uint64     adr = trans.get_address() / 4;
    unsigned char *   ptr = trans.get_data_ptr();
    unsigned int      len = trans.get_data_length();
    unsigned char *   byt = trans.get_byte_enable_ptr();
    unsigned int      wid = trans.get_streaming_width();

    if (adr >= sc_dt::uint64(SIZE) || byt != 0 || len > 4 || wid < len) {
      SC_REPORT_ERROR("TLM 2.0", "Target does not support given generic payload transaction");
    }

    if (cmd == tlm::TLM_READ_COMMAND) {
      memcpy(ptr, &mem[adr], len);
    } else if (cmd == tlm::TLM_WRITE_COMMAND) {
      memcpy(&mem[adr], ptr, len);
    }

    trans.set_response_status(tlm::TLM_OK_RESPONSE);
  }
private:
  int mem[SIZE];
};

class Top : public sc_module {
public:
  SC_HAS_PROCESS(Top);
  Initiator *initiator;
  Memory    *memory;

  Top(const sc_module_name& name){
    initiator = new Initiator("initiator");
    memory    = new Memory("memory");

    initiator->socket.bind( memory->socket); //一个Initiator的socket总是同一个target的socket绑定在一起。
    // 从底层实现上来讲，Initiator的socket是一个sc_port，同时拥有一个sc_export；
    // 对应的一个target的socket是一个sc_export，同时也有一个sc_port，这两者可以直接连接以实现通信。
  }
};

int sc_main(int argc, char **argv){
  Top top("top");
  sc_start();
  return 0;
}