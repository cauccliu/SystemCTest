#include "systemc.h"
#include "tlm.h"
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>

using namespace sc_core;

SC_MODULE(Initiator){
    tlm_utils::simple_initiator_socket<Initiator> initiator_socket;

    SC_CTOR(Initiator):initiator_socket("initiator_socket"){
        SC_THREAD(initiator_thread);
    }
    void initiator_thread(){
        tlm::tlm_generic_payload payload; // tlm::tlm_generic_payload 是 TLM2.0 中的一个类，用于表示一个事务（transaction）的有效载荷（payload）
        payload.set_command(tlm::TLM_READ_COMMAND); //设置了事务的命令为读命令。
        payload.set_address(0x1000); // 设置了事务的目标地址为 0x1000。
        payload.set_data_length(4); // 事务中数据的长度为4。
        sc_core::sc_time time = sc_core::sc_time(10, sc_core::SC_NS); // 事务的执行时间设置为 10 个纳秒。
        initiator_socket->b_transport(payload, time);// 执行事务 b_transport 是 TLM2.0 中的一个函数，用于在模块之间执行同步事务
    }
};

SC_MODULE(Target){
    tlm_utils::simple_target_socket<Target> target_socket;

    SC_CTOR(Target) : target_socket("target_socket") {
        target_socket.register_b_transport(this, &Target::b_transport);
    }
    
    // b_transport是一个回调函数
    void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay) {
        tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64 addr = trans.get_address();
        unsigned char* data_ptr = trans.get_data_ptr();
        unsigned int len = trans.get_data_length();

        std::cout << "Target received command: " << (cmd == tlm::TLM_READ_COMMAND ? "READ" : "WRITE")
                  << ", Address: " << std::hex << addr << ", Data Length: " << std::dec << len << std::endl;

        // Process the transaction (this example doesn't actually do anything with the data)
        delay += sc_time(10, SC_NS); // Simulate some delay
    }
};

int sc_main(int argc, char* argv[]){
    Initiator initiator("initiator");
    Target target("target");

    initiator.initiator_socket.bind(target.target_socket); // socket绑定

    sc_start();
    return 0;
}