
#ifndef PE_H_
#define PE_H_

#include "header.h"
#include "subcore.h"

class Pe
: public sc_core::sc_module
{
public:
	Pe( sc_core::sc_module_name name)  : sc_core::sc_module  ( name  )
	{
		for(int i=0;i<subcore_per_core;i++){
			std::string tmp = "warp_";
			tmp += std::to_string(i);
			subcore[i] = new SubCore(tmp.c_str());
		}
	}

	SubCore *subcore[subcore_per_core];

};

#endif /* PE_H_ */
