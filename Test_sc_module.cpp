#include "systemc"

using namespace sc_core;


class SoC: public sc_module{

public:
    SC_HAS_PROCESS(SoC);
    SoC(sc_module_name const &nm);
    ~SoC();
    void test_thread();

private:
    uint32_t cnt;

};

SoC::SoC(sc_module_name const &nm):sc_module(nm),cnt(0){
    std::cout<< "Contruct a SoC module"<<std::endl;
    std::cout<<"This is a demo for sc_module"<<std::endl;
    std::cout<<"Construct SoC done"<<std::endl;
}

SoC::~SoC()
{
    
}

int sc_main(int, char**) {
    SoC soc("soc");
    sc_start(10, SC_NS);
    sc_core::sc_get_curr_simcontext()->reset();
}
