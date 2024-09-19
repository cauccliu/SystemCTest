
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

		blk_shm_ = new SharedMem(shared_memory_size);
		cta_bar_ = new AJBarrier(ajbarrier_size);
	}

	SubCore *subcore[subcore_per_core];
	SharedMem* blk_shm_; // max shared memory per block ：256KB，step 16KB
	AJBarrier* cta_bar_; // 独立出来
};

#endif /* PE_H_ */
