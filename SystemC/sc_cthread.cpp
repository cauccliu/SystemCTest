// 钟控线程进程 SC_CTHREAD
#include<iostream>
#include<systemc.h>

/* SC_MODULE(Test) {
    SC_CTOR(Test) {
        SC_CTHREAD(DoTest, clk.pos());
    }
 
    void DoTest() {
        while (true) {
            std::cout << ++index << " Test sc_cthread" << std::endl;
            wait(2); // 这里的wait是等待的时钟信号，每两个时钟上升沿才会打印一次Test。
        }
    }
 
public:
    int index{};
    sc_in_clk clk;
};
 
int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 20, SC_NS);
    Test test("test");
    test.clk(clk);
    sc_start(200, SC_NS); // 因为每两个时钟上升沿才会打印一次，所以只会打印5次。
    return 0;
} */

// 一个带复位的例子

SC_MODULE(Test) {
    SC_CTOR(Test) {
        SC_CTHREAD(DoTest, clk.pos());
        // 只要当 val = true 就执行复位
        reset_signal_is(val, true);  // 执行信号
    }
 
    void DoTest() {
        int index = 0;
        std::cout << "cthread start" << std::endl;
        while (true) {
            std::cout << ++index << " Test" << std::endl;
            wait(1);
        }
    }
 
public:
    // int index{};
    sc_in_clk clk;
    sc_in<bool> val; // 输入信号
};

SC_MODULE(Driver) {
    SC_CTOR(Driver) {
        SC_METHOD(GenData);
        sensitive << clk.pos();
        dont_initialize();
    }
 
    void GenData() {
        index_++;
        if (index_ % 2 == 0) {
            std::cout << "reset: " << index_ << std::endl;
            valid = !valid;
            val.write(valid);
        }
    }
 
public:
    sc_in_clk clk;
    sc_out<bool> val; // 输出信号
 
    int index_{};
    bool valid{};
};

int sc_main(int argc, char* argv[]){
    sc_clock clk("clk", 20, SC_NS);
    sc_signal<bool> val; // 发射信号的端口
    Test test("test");
    Driver driver("driver");
    test.clk(clk);
    test.val(val);
    driver.clk(clk);
    driver.val(val); // test和driver同时用的一个端口
    sc_start(200, SC_NS);
    return 0;
}