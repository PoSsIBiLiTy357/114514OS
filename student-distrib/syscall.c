#include "syscall.h"
#include "rtc.h"
#include "filesys_read.h"
#include "keyboard.h"
#define PROC_NUM 6
#define PCB_SIZE 0x2000
#define KSTACK_BOT 0x800000-0x2000


static int curr=0;
//device_t rtc = { rtc_read, rtc_write, rtc_open, rtc_close };
int proc_state[PROC_NUM]={0,0,0,0,0,0};


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


//device_t rtc = { rtc_read, rtc_write, rtc_open, rtc_close };

int32_t halt(uint8_t status){
    return 0;
}

int32_t execute(const uint8_t * command){

   

    tss.esp0=KSTACK_BOT+PCB_SIZE-PCB_SIZE*curr-4;
    pcb_t* pcb=KSTACK_BOT-PCB_SIZE*curr;
    asm volatile(
        "cli
         movl %%ebp,%0;
         movl %%esp,%1;
         movl %2,%%ebp;
         movl %2,%%esp;
         movw $USER_DS,%%ax;
         movw %%ax,%%ds;
         movw %%ax,%%es;
         movw %%ax,%%fs;
         movw %%ax,%%gs;
         movl %%esp,%%eax;
         pushl $USER_DS;
         pushl $0x083FFFFC;
         pushf;
         popl %%eax;
         orl $0x200,%%eax;
         pushl %%eax;
         pushl $USER_CS;
         pushl %3;
         iret;"
         :"=r" (pcb->parent_ebp),
          "=r" (pcb->parent_esp)
         :"r"  (tss.esp0),
          "r"  (v_addr)
         :"memory","cc");////////////////////////////////////////////////////////////change here if something went fuck
    return 0;
}
int32_t read(int32_t fd, void * buf, int32_t nbytes){
    return 0;
}
int32_t write(int32_t fd, const void * buf, int32_t nbytes){
    return 0;
}
int32_t open(const uint8_t * filename){
    
    pcb_t pcb_t;
    dentry_t temp_dentry;
    int i;
    if (read_dentry_by_name(filename, &temp_dentry) ==-1) return -1; // if the filename does not exist, return -1
    for (i = 0; i <FDESC_SIZE;i++){
        if (pcb_t.bitmap[FDESC_SIZE]==0){
            pcb_t.bitmap[FDESC_SIZE]=1;
            break;
        } 
    }
    if (i==FDESC_SIZE) return -1; //if no descriptors are free, return -1
    
    if(temp_dentry.ftype == 0){
        pcb_t.file_array[i].read = rtc_read_wrapper;
        pcb_t.file_array[i].write = rtc_write_wrapper;
        pcb_t.file_array[i].open = rtc_open_wrapper;
        pcb_t.file_array[i].close = rtc_close_wrapper;
    }
    
    if (temp_dentry.ftype == 1){
        pcb_t.file_array[i].read = read_dir_wrapper;
        pcb_t.file_array[i].write = write_dir_wrapper;
        pcb_t.file_array[i].open = open_dir_wrapper;
        pcb_t.file_array[i].close = close_dir_wrapper;        
    }
    if (temp_dentry.ftype ==2){
        pcb_t.file_array[i].read = read_f_wrapper;
        pcb_t.file_array[i].write = write_f_wrapper;
        pcb_t.file_array[i].open = open_f_wrapper;
        pcb_t.file_array[i].close = close_f_wrapper;           
    }
    pcb_t.file_array[i].inode = temp_dentry.inode;
    pcb_t.file_array[i].flag =1; // file in use 
    pcb_t.file_array[i].file_pos = 0;
    return 0;

}

int32_t close(int32_t fd){
    if (fd>=FDESC_SIZE) return -1;
    pcb_t * temp_pcb;
    int temp_pcb_addr = 0x800000-0x2000-curr*0x2000;
    temp_pcb = (pct_t *) temp_pcb_addr;
    temp_pcb->file_array[fd].flag =0;
    return 0;

}


int32_t getargs(uint8_t * buf, int32_t nbytes){
    // if(){
    //     return -1;
    // }
    // if(){
    //     return-1;
    // }
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
