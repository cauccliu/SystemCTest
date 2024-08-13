#include <iostream>
#include "systemc"
#include "tlm"

// define a class hello_world , and this model print some information only
class hello_world : public sc_core::sc_module {
 public:
  SC_HAS_PROCESS(hello_world);

  // ctor
  hello_world(sc_core::sc_module_name name) : sc_core::sc_module(name) {

    // declare a thread, performing only one time for "SC_METHOD"  
    SC_METHOD(main_thread);
  }

  // dtor
  ~hello_world() {}

  // definition of a thread
  void main_thread() {
    std::cout <<this->name() <<"@ " << sc_core::sc_time_stamp()<< " hello world\n";
    std::cout <<this->name() <<"@ " << sc_core::sc_time_stamp()<< " hello systemc + tlm2.0\n";
    std::cout <<this->name() <<"@ " << sc_core::sc_time_stamp()<< " wellcome to the IP modeling world\n";

  }
}; // end of hello_world

// systemc entry must be sc_main
int sc_main(int, char**) {

  // model instance
  hello_world hw_model("hw_model");

  // start simulation
  sc_core::sc_start();

  return 0;
}

/* #include <iostream>
#include "Test.h"
using namespace std;
int main(){
  int a = 1;
  int b = 1;
  int c = sumA(a,b);
  return 0;
} */