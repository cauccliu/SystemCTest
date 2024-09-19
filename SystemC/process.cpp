#include <systemc.h>

SC_MODULE(PROCESS) {
  sc_clock clk; // 声明一个clock对象
  SC_CTOR(PROCESS) : clk("clk", 1, SC_SEC) { // 实例化一个周期为1秒的时钟
    SC_METHOD(method); // method
    SC_THREAD(thread); // thread
    SC_CTHREAD(cthread, clk); // clocked thread
  }
  void method(void) { 
    std::cout << "method triggered @ " << sc_time_stamp() << std::endl;
    next_trigger(sc_time(1, SC_SEC)); // 一秒钟后会再次触发
  }
  void thread() { 
    while (true) { // while循环
      std::cout << "thread triggered @ " << sc_time_stamp() << std::endl;
      wait(1, SC_SEC); // 等待1秒
    }
  }
  void cthread() { 
    while (true) { // while循环
      std::cout << "cthread triggered @ " << sc_time_stamp() << std::endl;
      wait(); // 等待clk事件触发
    }
  }
};

int sc_main(int, char*[]) {
  PROCESS process("process"); 
  std::cout << "execution phase begins @ " << sc_time_stamp() << std::endl;
  sc_start(2, SC_SEC); // 模拟两秒
  std::cout << "execution phase ends @ " << sc_time_stamp() << std::endl;
  return 0;
}