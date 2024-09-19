
#ifndef SOC_H_
#define SOC_H_

#include "header.h"
#include "cluster.h"
#include "block.h"

class SoC
: public sc_core::sc_module
{
public:
	SoC( sc_core::sc_module_name name)  : sc_core::sc_module  ( name  )
	{
		for(int i=0;i<cluster_num;i++){
			std::string tmp = "cluster_";
			tmp += std::to_string(i);
			cluster[i] = new Cluster(tmp.c_str());
		}
	}

	Cluster *cluster[cluster_num];
	std::vector<Block*> blockPtrVec_;
};

#endif /* SOC_H_ */
