#include <iostream>
#include "systemc.h"
#include "tlm"

SC_MODULE(HelloWorld) {
  SC_CTOR(HelloWorld) {
    SC_THREAD(printHello);
  }

  void printHello() {
    std::cout << "Hello, World!" << std::endl;
  }
};
// systemc entry must be sc_main
int sc_main(int, char**) {

  // model instance
  HelloWorld hw_model("hw_model");

  // start simulation
  sc_start();

  return 0;
}
