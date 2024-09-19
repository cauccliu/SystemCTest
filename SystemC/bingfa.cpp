#include <systemc.h>

SC_MODULE(CONCURRENCY) {
  SC_CTOR(CONCURRENCY) { 
    SC_THREAD(thread1); 
    SC_THREAD(thread2); 
  }
  void thread1() {
    while(true) { 
      std::cout << sc_time_stamp() << ": thread1" << std::endl;
      wait(2, SC_SEC); 
    }
  }
  void thread2() {
    while(true) {
      std::cout << "\t" << sc_time_stamp() << ": thread2" << std::endl;
      wait(3, SC_SEC);
    }
  }
};

int sc_main(int, char*[]) {
  CONCURRENCY concur("concur"); 
  sc_start(10, SC_SEC); 
  return 0;
}
