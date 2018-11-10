#include "syscall.h"
#include "rtc.h"
#include "filesys_read.h"

static

//device_t rtc = { rtc_read, rtc_write, rtc_open, rtc_close };

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
