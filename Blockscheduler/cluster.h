
#ifndef CLUSTER_H_
#define CLUSTER_H_

#include "header.h"
#include "pe.h"

class Cluster
: public sc_core::sc_module
{
public:
	Cluster( sc_core::sc_module_name name)  : sc_core::sc_module  ( name  )
	{
		for(int i=0;i<core_per_cluster;i++){
			std::string tmp = "core_";
			tmp += std::to_string(i);
			core[i] = new Pe(tmp.c_str());
		}
	}

	Pe *core[core_per_cluster];
};

#endif /* CLUSTER_H_ */
