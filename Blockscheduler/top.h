
#ifndef TOP_H_
#define TOP_H_
#include "header.h"
#include "soc.h"

class Top
: public sc_core::sc_module                   //  SC base class
{
public:
	Top ( sc_core::sc_module_name name)  : sc_core::sc_module  ( name )
	{
		soc = new SoC("SoC");
	}

	SoC *soc;
};

#endif /* TOP_H_ */
