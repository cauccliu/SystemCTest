# pragma once
#include "header.h"
#include "soc.h"
#include <systemc.h>
#include "tlm.h"
#include <thread>
#include <chrono>
#include "tlm_utils/simple_target_socket.h"


class BlockScheduler
: public sc_core::sc_module                   //  SC base class
{
public:
	BlockScheduler ( sc_core::sc_module_name name, SoC * soc)  : sc_core::sc_module  ( name ), soc_(soc)
	{
		 
	}
    int SchedulerMode1(const KernelParam &param, AJDecoder &decoder, ScheduleParam& sp);

    SoC *soc_;
	bool is_block_scheduler_init_ = false;
};

// 需要走总线，soc->总线->cluster
// 单独写一个调度module schedu函数，分发三个mode功能函数
int BlockScheduler::SchedulerMode1(const KernelParam &param, AJDecoder &decoder, ScheduleParam& sp) {
    size_t n_blocks = param.nctaid.x * param.nctaid.y * param.nctaid.z;
	size_t warps_per_block = param.warps_per_block;
	uint64_t kcfg_pc_ = param.init_pc.lo + (static_cast<uint64_t>(param.init_pc.hi) << 32);
	GlobalMem ddr_;
	if (param.ddr_cfgs.size() > 0) {
      	ddr_.DdrInit<32>(param.ddr_cfgs);
    }
	AJTmaUnit tma_;

	auto do_run = [&](int warp_id_in_block, Pe* core, SubCore* subcore, Block *block)
    {
        std::lock_guard<std::mutex> l(subcore->warp_ready_[warp_id_in_block]);
        int res = subcore->warp[warp_id_in_block]->RunThreadBlock(block->blk_ctaid_, warp_id_in_block, kcfg_pc_, param, core->blk_shm_, &ddr_, core->cta_bar_, &tma_);
        block->warps_work_end_[warp_id_in_block] = '1';
        return res;
    };

	Triplet ctaid;

	auto &sr_cfg = param.reg_cfgs.sr_cfg;
    bool single_warp = sr_cfg.count(CTAID_X) || sr_cfg.count(CTAID_Y) ||sr_cfg.count(CTAID_Z) || sr_cfg.count(WARPID);

	if (single_warp){
        #define GET_AND_VERIFY(FIELD, NAME)                                            \
            ctaid.FIELD = sr_cfg.count(CTAID_##NAME) ? sr_cfg.at(CTAID_##NAME) : 0;      \
            if (ctaid.FIELD >= param.nctaid.FIELD) {                                     \
                std::cout << "backdoor cfg ctaid." #FIELD " exceeds nctaid." #FIELD       \
                                ", got: "                                                    \
                            << ctaid.FIELD << ">=" << param.nctaid.FIELD;                   \
                return 1;                                                                  \
            }
            GET_AND_VERIFY(x, X)
            GET_AND_VERIFY(y, Y)
            GET_AND_VERIFY(z, Z)
        #undef GET_AND_VERIFY
        
        // warp_id is obtained from sr_cfg，if sr_cfg no WARPID，warp_id = 0
        size_t warp_id = sr_cfg.count(WARPID) ? sr_cfg.at(WARPID) : 0;
        if (warp_id >= param.warps_per_block) {
            std::cout << "backdoor warp idx exceeds warps per block, got " << warp_id
                    << ">=" << param.warps_per_block;
            return 1;
        }

        Block blocktemp("block");
        Pe* corePtr = soc->cluster[0]->core[0];
        SubCore* subcorePtr = corePtr->subcore[0];

        blocktemp.blk_ctaid_ = ctaid;
        
        if (param.shm_cfgs.size() > 0)
        {
            corePtr->blk_shm_->ShmInit<32>(param.shm_cfgs);
        }

        for (uint32_t warp_idx = 0; warp_idx < warp_per_subcore; ++warp_idx){
            subcorePtr->warp[warp_idx] = AJWarp(&decoder); // need change code
        }

        int err = do_run(warp_id,corePtr,subcorePtr,&blocktemp);
        if (err) {
            std::cout << "block " << ctaid.z << " " << ctaid.y << " " << ctaid.x
                    << " warp " << warp_id << " execution failed";
            return err;
        }
        
    }
	else{
        BS::thread_pool pool(cluster_num); // one thread per cluster, pool size = 16
        std::vector<Block*> blocks;

        while (n_blocks > 0)
        {
            if (is_block_scheduler_init_ == false)
            {
                is_block_scheduler_init_ = true;
                for (size_t clusterIndex = 0; clusterIndex < cluster_num && (n_blocks > 0); ++clusterIndex)
                {
                    Cluster *phyClusterTemp = soc->cluster[clusterIndex];
                    size_t curCoreRunningBlock = 0;
                    for (size_t coreIndex = 0; coreIndex < core_per_cluster && (n_blocks > 0) && (curCoreRunningBlock < cta_num); ++coreIndex)
                    {
                        Pe *phyCoreTemp = phyClusterTemp->core[coreIndex];
                        
                        for (size_t subCoreIndex = 0; subCoreIndex < subcore_per_core && (n_blocks > 0) && (curCoreRunningBlock < cta_num); ++subCoreIndex)
                        {
                            SubCore *phySubCoreTemp = phyCoreTemp->subcore[subCoreIndex];
                            if (warp_per_subcore >= warps_per_block)
                            {
                                // subcore can execute one or more block
                                size_t blockIndex = 0;
								size_t warpNumTemp = warp_per_subcore;
                                while (warpNumTemp >= warps_per_block && (curCoreRunningBlock < cta_num))
                                {
                                    Block block("temp");
                                    block.subcorePtrVec_.emplace_back(phySubCoreTemp);
                                    block.core_ = phyCoreTemp;
									block.blk_ctaid_ = ctaid;
                                    for (size_t warpIndex = 0; warpIndex < warps_per_block; warpIndex++)
                                    {
                                        phySubCoreTemp->warp[blockIndex * warps_per_block + warpIndex] = std::make_unique<AJWarp>(&decoder).get(); // need change
                                        block.warps_work_end_[warpIndex] = '0';
                                        block.warps_result_.push_back(pool.submit(do_run, warpIndex, phyCoreTemp, phySubCoreTemp,&block));
                                    }
                                    blocks.push_back(&block);
                                    ++blockIndex;
                                    warpNumTemp -= warps_per_block;
                                    ++curCoreRunningBlock;

                                    ctaid.x++;
                                    if (ctaid.x == param.nctaid.x)
                                    {
                                        ctaid.x = 0;
                                        ctaid.y++;
                                    }
                                    if (ctaid.y == param.nctaid.y)
                                    {
                                        ctaid.y = 0;
                                        ctaid.z++;
                                    }

                                    if (--n_blocks <= 0)
                                        break;
                                }
                            }
                            else
                            {
                                // block need more than one subcore to work
                                Block block("temp");
                                block.core_ = phyCoreTemp;
								block.blk_ctaid_ = ctaid;

                                size_t remainingWarps = warps_per_block;
                                size_t coreIndex = 0;
                                while (remainingWarps > 0 && (subCoreIndex < subcore_per_core))
                                {
                                    size_t warpsToUse = remainingWarps< warp_per_subcore ? remainingWarps : warp_per_subcore;

                                    for (size_t warpIndex = 0; warpIndex < warpsToUse; warpIndex++)
                                    {
										phySubCoreTemp->warp[warpIndex] = std::make_unique<AJWarp>(&decoder).get(); // need change
                                        block.warps_work_end_[warpIndex + coreIndex * warp_per_subcore] = false;
                                        block.warps_result_.push_back(pool.submit(do_run, warpIndex, phyCoreTemp, phySubCoreTemp, &block));
                                    }
                                    block.subcorePtrVec_.emplace_back(phySubCoreTemp); // 会插入多个subcore
                                    remainingWarps -= warpsToUse;
                                    if (remainingWarps > 0)
                                    {
                                        ++subCoreIndex;
                                        if (subCoreIndex < subcore_per_core)
                                        {
                                            phySubCoreTemp = phyCoreTemp->subcore[subCoreIndex];
                                        }
                                    }
                                    ++coreIndex;
                                }

                                ++curCoreRunningBlock;

                                ctaid.x++;
                                if (ctaid.x == param.nctaid.x)
                                {
                                    ctaid.x = 0;
                                    ctaid.y++;
                                }
                                if (ctaid.y == param.nctaid.y)
                                {
                                    ctaid.y = 0;
                                    ctaid.z++;
                                }

                                if (--n_blocks <= 0)
                                    break;
                            }
                        }
                    }
                }
            }
            else
            {
                while (n_blocks > 0)
                {
                    for (auto b : blocks)
                    {
                        if (b->allWarpsDone(warps_per_block))
                        {
                            for (size_t warpIndex = 0; warpIndex < warps_per_block; warpIndex++)
                            {
                                b->warps_work_end_[warpIndex] = false;
                                b->warps_result_[warpIndex] = pool.submit(do_run, warpIndex, b->core_, b->subcorePtrVec_[warpIndex % warp_per_subcore], b);
                            }
                            --n_blocks;
                        }
                        if (n_blocks <= 0)
                            break;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }// end while
            }// end else
        }// end while (n_blocks > 0)
    } 

	return 0;
}