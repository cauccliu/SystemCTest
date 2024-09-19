
#include "top.h"

int sc_main(int argc, char* argv[])
{
	Top top("top");        // instantiate a exmaple top module

	sc_core::sc_start();                  // start the simulation
    return 0;
}

