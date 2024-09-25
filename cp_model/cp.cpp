#include "cp.h"

int AJCpModule ::SearchQueue(){ //-----当下只适合启动一条queue的情况，多条queue下要分着给指定的queue，需要在top层给指定queue发包
    for(int i = 0; i < QNUM; ++i) {
        if( !(all_queue_status & (1<<i) )) {
            std::cout << "Queue id = " << i << std::endl;
            queue_id = i;
            queue_status = CP_QUEUE_BUSY;
            all_queue_status=all_queue_status  | (1 << i);
            return i; 
    }      
    }
    cout<< "no free queue" << endl; 
    return -1;
}

void AJCpModule ::thread_process() { //总共三种状态，等待状态，写指针更新后的wptr_update,与queue通讯的device_sync
    tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
    sc_time delay = sc_time(1, SC_NS);
    while(1){
        wait( (device_sync | *wptr_update));
        if(device_sync.triggered()) {                    
            cout << "device_sync at time " << sc_time_stamp()<<endl;
            CPQueueManage(trans,delay);
        }
        if(wptr_update->triggered()) {    
            cout << "wptr_update at time " << sc_time_stamp()<<endl;
            CPQueueStart(trans,delay);
        }
    }
}

void AJCpModule ::CPQueueStart(tlm::tlm_generic_payload* trans,sc_time delay){        
    queue_id = SearchQueue();//当前还没有真正用上search来找空闲queue，全靠指定，后续可能搬到Top

    tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;
    mes.addr = rbuf;
    mes.queue_id = queue_id;
    mes.queue_status = queue_status;
    mes.queue_status = queue_sync;
    mes.ib_size = 5;
    if(all_queue_status == queue_mask){ //后续需判断sync包
        queue_sync = 0;
        mes.queue_sync = queue_sync;
        queue_mask = 0;                        
    }

    trans->set_command(cmd);
    trans->set_address(0);
    trans->set_data_ptr(reinterpret_cast<unsigned char*>(&mes));
    trans->set_data_length(sizeof(mes));
    trans->set_streaming_width(sizeof(mes));
    trans->set_byte_enable_ptr(0);
    trans->set_dmi_allowed(false);
    trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

    socket_cp2queue[queue_id]->b_transport(*trans, delay);

    //wptr_update->cancel();

    if (trans->is_response_error()) {
        SC_REPORT_ERROR("TLM 2.0", "Response error from b_transport");
    }
    cout << "trans = { " << (cmd ? 'W' : 'R') << ", " << hex << 0
    << " } , data = " << hex << mes.addr << " queue_status  " << mes.queue_status <<  "  at time " << sc_time_stamp()
    << " queue_sync = " << mes.queue_sync << "  queue id " << queue_id << "  all_queue_status = " << all_queue_status << endl;
    wait(delay);
    
}

void AJCpModule ::CPQueueManage(tlm::tlm_generic_payload* trans,sc_time delay){
    if((queue_status == CP_QUEUE_BUSY) &(queue_sync ==CP_DEVICE_SYNC)){
        queue_mask = queue_mask & (1<<queue_id) ;//记录下queue id对应在mask里的对应状态
        if(queue_mask == all_queue_status){//如果所有在忙的queue都遇上queue sync
            for(int i = 0;i<QNUM ; ++i){
                if(queue_mask & (1 << i)){
                    mes.queue_id = i;
                    mes.queue_sync = 0; //取消掉sync

                    trans->set_command(tlm::TLM_WRITE_COMMAND);
                    trans->set_data_ptr(reinterpret_cast<unsigned char*>(&mes));
                    trans->set_data_length(sizeof(mes));
                    trans->set_streaming_width(sizeof(mes));
                    socket_cp2queue[i]->b_transport(*trans, delay);
                }
            }
            queue_mask = 0; //条件满足后最后清空mask
        }
    }else if((queue_status == CP_QUEUE_FREE) &(queue_sync ==0)){//queue内任务处理完毕，告诉cp空闲下来
        all_queue_status = all_queue_status & (1<<queue_id) ;
    }

}

void AJCpModule ::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay){
    tlm::tlm_command  cmd = trans.get_command();
    sc_dt::uint64     adr = trans.get_address() / 4;
    unsigned char *   ptr = trans.get_data_ptr();
    unsigned int      len = trans.get_data_length();
    unsigned char *   byt = trans.get_byte_enable_ptr();
    unsigned int      wid = trans.get_streaming_width();
    
    memcpy(&mes, ptr, len);
    queue_id = mes.queue_id;
    queue_status = mes.queue_status;
    queue_sync = mes.queue_sync;

    //trans.set_response_status(tlm::TLM_OK_RESPONSE);  
    cout << "cp read trans  data = " << hex << mes.addr << " queue_status = " << mes.queue_status <<  "  at time " << sc_time_stamp()
    << " queue_sync = " << mes.queue_sync << "  queue id = " << queue_id << "  all_queue_status = " << all_queue_status << endl;     
    

    wait(1, SC_NS);//必须这里加上wait一定时间，让仿真时间推进，才能触发状态机的变化
    device_sync.notify();
    cout<<"22222"<<endl;
    
}