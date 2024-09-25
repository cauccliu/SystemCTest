#include<iostream>
#include<systemc.h>

/* SC_MODULE(Test) {
    SC_CTOR(Test) {
        SC_THREAD(DoTest);
        sensitive << clk.pos();
        dont_initialize();
    }
 
    void DoTest() {
        while (true) {
            std::cout << ++index << " Test thread" << std::endl;
            wait();
        }
    }
 
public:
    sc_in_clk clk;
    int index{};
};

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 20, SC_NS);
    Test test("test");
    test.clk(clk);
    sc_start(200, SC_NS);
    return 0;
} */

SC_MODULE(Test) {
    SC_CTOR(Test) {
        SC_THREAD(DoTest);
    }
 
    void DoTest() {
        while (true) {
            std::cout << ++index << " Test thread wait 20ns" << std::endl;
            wait(20, SC_NS); // 表示挂起20 ns 后再重新被激活 
        }
    }
 
public:
    int index{};
};
 
int sc_main(int argc, char* argv[]) {
    Test test("test");
    sc_start(200, SC_NS);
    return 0;
}
