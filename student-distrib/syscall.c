#include "syscall.h"
#include "rtc.h"
#define PROC_NUM 6
#define PCB_SIZE 0x2000
#define KSTACK_BOT 0x800000-0x2000

//device_t rtc = { rtc_read, rtc_write, rtc_open, rtc_close };
int proc_state[PROC_NUM]={0,0,0,0,0,0};
static int curr=0;

void pcb_init(int pid){
    int i;
    curr = pid;
    
    pcb_t* pcb=KSTACK_BOT-PCB_SIZE*pid;
    pcb->pid=pid;
    pcb->file_array[0].read=terminal_read_warp;///init stdin and stdout
    pcb->file_array[0].write=NULL;
    pcb->file_array[0].open=NULL;
    pcb->file_array[0].close=NULL;
    pcb->file_array[0].flags=1;

    pcb->file_array[1].read=NULL;
    pcb->file_array[1].write=terminal_write_warp;
    pcb->file_array[1].open=NULL;
    pcb->file_array[1].close=NULL;
    pcb->file_array[1].flags=1;

    for(i=2;i<FDESC_SIZE;i++){
        pcb->file_array[i].flags=0;
    }
    if(pid==0){
        pcb->parent=NULL;
    }
    else{
        pcb->parent=KSTACK_BOT-PCB_SIZE*pid+PCB_SIZE;
    }
}

int get_pid(){
    int i;
    for(i=0;i<PROC_NUM;i++){
        if(proc_state[i]==0){
            return i;
        }
    }
    return -1;
}

int32_t halt(uint8_t status){
    return 0;
}

int32_t execute(const uint8_t * command){
    
    return 0;
}
int32_t read(int32_t fd, void * buf, int32_t nbytes){
    return 0;
}
int32_t write(int32_t fd, const void * buf, int32_t nbytes){
    return 0;
}
int32_t open(const uint8_t * filename){
    
    return 0;
}
int32_t close(int32_t fd){
    return 0;
}
int32_t getargs(uint8_t * buf, int32_t nbytes){
    return 0;
}
int32_t vidmap(uint8_t ** start_screen){
    return 0;
}
int32_t set_handler(int32_t signum, void * handler_address){
    return 0;
}
int32_t sigreturn(void){
    return 0;
}
