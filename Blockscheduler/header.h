
#ifndef HEADER_H_
#define HEADER_H_

#include "tlm.h"

enum{
	cluster_num = 16,
	core_per_cluster = 5,
	subcore_per_core = 4,
	warp_per_subcore = 16,
	thread_per_warp = 32,

	max_warp_num_block = 32,
	core_per_logic_cluster = 2
};


#endif /* HEADER_H_ */
