
#ifndef SUBCORE_H_
#define SUBCORE_H_

#include "header.h"
#include "tlm.h"


class Warp
: public sc_core::sc_module
{
public:
	Warp( sc_core::sc_module_name name)  : sc_core::sc_module  ( name  )
	{
		for(int i=0;i<thread_per_warp;i++){
			thread[i].threadId = i;
		}
	}

	typedef struct{
		int threadId;
	}Thread;

	Thread thread[thread_per_warp];
};

class SubCore
: public sc_core::sc_module
{
public:
	SubCore( sc_core::sc_module_name name)  : sc_core::sc_module  ( name  )
	{
		for(int i=0;i<warp_per_subcore;i++){
			std::string tmp = "warp_";
			tmp += std::to_string(i);
			warp[i] = new Warp(tmp.c_str());
		}
	}

	Warp *warp[warp_per_subcore];
};



#endif /* SUBCORE_H_ */
