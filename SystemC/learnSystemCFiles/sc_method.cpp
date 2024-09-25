// SC_METHOD 敏感列表上有事件发生就会被立即调用，调用后立刻返回。
#include<iostream>
#include<systemc.h>

SC_MODULE(Test){
    SC_CTOR(Test){
        SC_METHOD(DoTest);
        sensitive << clk.pos(); // 只在时钟上升沿触发, clk.neg();表示只在下降沿触发。
        dont_initialize(); // 仿真时刻0不执行，之后再执行。
    }
    void DoTest(){
        std::cout << ++index << " Test" << std::endl;
    }
public:
    sc_in_clk clk; 
    // sc_in_clk 是一个输入端口类，用于接收时钟信号。它是 SystemC 中的基本时钟端口类，可以用于接收任意频率的时钟信号。
    // 通常，sc_in_clk 会被连接到一个 sc_clock 实例，以接收时钟信号。
    int index{};
};

int sc_main(int argc, char* argv[]){
    sc_clock clk("clk", 20, SC_NS); //sc_clock 是一个时钟类，用于产生时钟信号。它可以用于产生任意频率的时钟信号，并可以用于驱动模块的行为。
    // 占空比默认是0.5
    // SC_NS纳秒
    // 时钟周期是 20 NS，占空比是 0.5，那么就是高低电平各10NS，如果占空比为 0.9 那么就表示有 18 NS 是高电平，2 NS 是低电平
    Test test("test");
    test.clk(clk);
    sc_start(200,SC_NS);
    return 0;
}