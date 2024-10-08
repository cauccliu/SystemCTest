#include "ajqueue.h"
std::map<uint64_t, uint32_t*> AddrMap;

enum dstEnum {
    dst_cu = 0,
    dst_cpu = 1,
    dst_ce = 2,
    dst_event_sync = 3,
    dst_c2c =4 
};

void AJQueue::Thread_Queue(){
    while(1){
        wait(received_from_cp);
        CpIndirectBuffer();
        //CpDeviceSync();//后续应该嵌入到CpIndirectBuffer内，这里为了方便检查拿出来了
        QueueReportCp(1);
    }
}

void AJQueue::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay){
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
    for (const auto& pair : AddrMap) {
        std::cout << "Key1: " << std::hex << pair.first << ", Value: " << *pair.second << std::endl;
    }    

    cout << "mes.addr = " << mes.addr <<endl;
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    received_from_cp.notify();
  }


uint32_t* AJQueue::getIbufPtr(uint64_t data){  //ringbuffer到 ibuf地址map

    // 检查map中是否已经存在对应data的指针
        for (const auto& pair : AddrMap) {
            std::cout << "Key: " << std::hex << pair.first << ", Value: " << *pair.second << std::endl;
            }    

        auto it = AddrMap.find(data);
        cout << "it.first = " << it->first <<endl;
        if (it != AddrMap.end()) {
            return it->second; // 返回对应的指针
        } else {
         //如果map中不存在对应data的指针，则创建一个新指针并添加到map中,可删改为报error
            throw std::runtime_error("No corresponding ib found in map for the given ringbuffer addr.");
        }
    }

void AJQueue::CpIndirectBuffer(){//根据ring所指向的地址（indirectbuffer里）取command，后续可能要添加与global memory交互，考虑溢出,建模最好指针操作，这样不容易错
    
    int header_body_dws = 0;    //command的长度
    uint32_t command_header;

    if(queue_pause == 0){
        ibuf_ptr = getIbufPtr(mes.addr);
        ib_size = mes.ib_size;
        cout << "ibuf_ptr = " << *ibuf_ptr <<endl;
        ib_offset = 0;
    }    
    
    for(; ib_offset<ib_size;){ //------逻辑待理顺--遇见sync包要跳出循环，以及判断是否offset到ib_size

            cout<<"now parsing command The time is  "<<sc_time_stamp()<< "!  command is "<< * (ibuf_ptr + ib_offset ) 
            << " offset is "<< ib_offset << " queue id is " << queue_id <<endl;            
            command_header =* (ibuf_ptr + ib_offset );
            header_body_dws = getHeaderBodydws(command_header);//包头解析处理
            CpCommandHeader(command_header);
            //包身处理           
            ib_offset = ib_offset + header_body_dws  +1;//
            if(queue_sync == CP_DEVICE_SYNC){
                break;
            }
        wait(1, SC_NS);    
    }
    if(ib_offset == ib_size){
        CpQueueRelease();
    }
    
}
int AJQueue::getHeaderBodydws(uint32_t command_header){ //目前是14-23位来表示长度，后续可能会有变化
    uint32_t mask = 0x000FFFC0;
    int header_body_dws = (command_header & mask) >> 6;
    return header_body_dws;
}
int AJQueue::getHeaderPacketType(uint32_t command_header){ //目前是31位来表示type,0/1
    uint32_t mask = 0x80000000;
    int header_packet_type = (command_header & mask) >> 31;
    return header_packet_type;
}
int AJQueue::getHeaderDst(uint32_t command_header){ //目前是28-30位来表示dst
    uint32_t mask = 0x70000000;
    int dst = (command_header & mask) >> 28;
    return dst;
}
int AJQueue::getHeaderCmdType(uint32_t command_header){ //目前是24-27位来表示cmd_type
    uint32_t mask = 0x0F000000;
    int header_cmd_type = (command_header & mask) >> 24;
    return header_cmd_type;
}

void AJQueue:: CpCommandHeader(uint32_t command_header){ //拆分数据包包头，目前只用到dst，后续有其他

    int packet_type = getHeaderPacketType(command_header);
    int dst = getHeaderDst(command_header);
    int cmd_type= getHeaderCmdType(command_header);
    int body_dws = getHeaderBodydws(command_header);
    //command_flag;
    //command_crc = command_head[11:0];
    switch(dst){
        case dst_cu:
            for(int i=1;i<body_dws+1;i++){
                CpToCu(); // command = offset + i
            }
            break;
        case dst_event_sync:
            CpEventSync(cmd_type);
            break;
        default:
            break;

    }
    return ;
};

void AJQueue:: CpToCu(){ //指向cu的包的处理
    //int a = create_cu_packet();
    return ;
};
void AJQueue:: CpEventSync(int cmd_type){//指向event_sync包的处理，三种类型的处理

    if(cmd_type == 2){
        CpDeviceSync();//sync的处理
    }
    return ; 
};
void AJQueue:: CpQueueRelease(){ //当前queue内是否还有任务，无的话，释放queue
    queue_pause = 0;
    queue_status = CP_QUEUE_FREE;
    ib_offset = 0;
    QueueReportCp(0);
    return ;
};

void AJQueue::CpDeviceSync(){  
    //pause 暂停 
    queue_pause = 1;
    queue_sync = 1;
    QueueReportCp(1);
    //wait(SC_ZERO_TIME);
}

void AJQueue::QueueReportCp(int StatusToCp){
    tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
    sc_time delay = sc_time(10, SC_NS);
    
    tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;
    mes.queue_id = queue_id;
    mes.ib_size = 0;
    mes.addr = 0x0;
    if(StatusToCp == 1 ){
        queue_sync = CP_DEVICE_SYNC;
        mes.queue_sync = queue_sync;
        mes.queue_status = queue_status;
    }else if(StatusToCp ==0){ //queue内任务执行完毕，清空
        queue_sync = 0;
        queue_status = CP_QUEUE_FREE;
        mes.queue_sync = queue_sync;
        mes.queue_status = queue_status;       
    }
    trans->set_command(cmd);
    trans->set_address(0);
    trans->set_data_ptr(reinterpret_cast<unsigned char*>(&mes));
    trans->set_data_length(sizeof(mes));
    trans->set_streaming_width(sizeof(mes));
    trans->set_byte_enable_ptr(0);
    trans->set_dmi_allowed(false);
    trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    socket_queue2cp->b_transport(*trans, delay);  

}