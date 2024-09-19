#include <systemc.h>

SC_MODULE(EVENT) {
  sc_event e; // 定义一个 event
  SC_CTOR(EVENT) {
    SC_THREAD(trigger); //注册一个　trigger process
    SC_THREAD(catcher); //注册一个 catcher process
  }
  void trigger() {
    while (true) { 
      e.notify(1, SC_SEC); // 1秒后触发
      if (sc_time_stamp() == sc_time(4, SC_SEC)) {
        e.cancel(); // 第四秒的时候停止触发
      }
      wait(2, SC_SEC); // 等待2秒 
    }
  }
  void catcher() {
    while (true) { 
      wait(e); 
      std::cout << "Event cateched at " << sc_time_stamp() << std::endl; 
    }
  }
};

int sc_main(int, char*[]) {
  EVENT event("event"); 
  sc_start(8, SC_SEC); 
  return 0;
}
