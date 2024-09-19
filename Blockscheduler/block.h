#pragma once

#include <future>
#include "header.h"
#include "pe.h"
#include "subcore.h"

struct Triplet {
  Triplet() : x(0), y(0), z(0) {}
  explicit Triplet(uint32_t _x, uint32_t _y, uint32_t _z)
      : x(_x), y(_y), z(_z) {}
  uint32_t x, y, z;
};

class Block
: public sc_core::sc_module
{
public:
	Block( sc_core::sc_module_name name)  : sc_core::sc_module  ( name  )
	{
		for(int i=0;i<max_warp_num_block;i++){
			warps_work_end_[i] = '0';
		}
	}

    bool allWarpsDone(size_t warpNum){
        for (size_t i = 0; i<warpNum; ++i) {
            if (warps_work_end_[i] == '0') {
                return false;
            }
        }
        return true;
    }

	char warps_work_end_[max_warp_num_block];
    std::vector<std::future<int>> warps_result_;
    Pe* core_;
    std::vector<SubCore*> subcorePtrVec_;
    Triplet blk_ctaid_;
};