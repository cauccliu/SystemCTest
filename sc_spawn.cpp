#include<iostream>
#include<systemc.h>

class Test : public sc_module {
public:
    SC_HAS_PROCESS(Test);
    Test(sc_module_name ins_name) : sc_module(ins_name) {
    }
 
    void CreateProcess() {
        sc_spawn_options opts;
        opts.dont_initialize();
        opts.spawn_method();
        opts.set_sensitivity(&clk.pos());
        auto lmd = [&]() -> void {
            std::cout << sc_time_stamp() << " Print" << std::endl;
        };
        // sc_core::sc_spawn(lmd, "sp1", &opts);
        sc_core::sc_spawn(sc_bind(&Test::Print, this), "sp1", &opts);
    }
 
    void Print() {
        std::cout << sc_time_stamp() << " Print" << std::endl;
    }
 
public:
    sc_in_clk clk;
};
 
int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 20, SC_NS);
    Test test("test");
    test.clk(clk);
    test.CreateProcess();
    sc_start(200, SC_NS);
    return 0;
}
