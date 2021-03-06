#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "lib.h"
#include "rtc.h"
#include "filesys_read.h"
#include "keyboard.h"
#include "paging.h"

#define PCB_SIZE            0x1000
#define KSTACK_BOT          0x7FF000
#define KSTACK_START        0x800000
#define FDESC_SIZE          8
#define MIN_FILES           2   
#define CMD_LIMIT           129
#define MEM_FENCE           0x10

#define GET_PCB(x)          (pcb_t *)(KSTACK_BOT - PCB_SIZE * (x))
#define GET_ESP(x)          (KSTACK_START - (PCB_SIZE * (x)) - MEM_FENCE)

/* Initialization struct for device referenced from: */
/* https://stackoverflow.com/questions/9932212/jump-table-examples-in-c */
typedef struct file_desc_t { 
    int (*open)(uint32_t,uint32_t, uint8_t* , uint32_t);
    int (*close)(uint32_t,uint32_t, uint8_t* , uint32_t);
    int (*read)(uint32_t,uint32_t, uint8_t* , uint32_t);
    int (*write)(uint32_t,uint32_t, uint8_t* , uint32_t);
    int32_t inode, file_pos, flag;
} file_desc_t;

typedef struct pcb_t{
    int32_t pid;
    int32_t p_pid;
    int32_t parent_esp;
    int32_t parent_ebp;
    int32_t current_esp;
    int32_t current_ebp;
    int32_t terminal;
    file_desc_t file_array[FDESC_SIZE];
    int32_t bitmap[FDESC_SIZE];
} pcb_t;


/* System calls */
int32_t halt(uint8_t status);
int32_t execute(const uint8_t * command);
int32_t execute_with_terminal_num(const uint8_t * command,int terminal_num);
int32_t read(int32_t fd, void * buf, int32_t nbytes);
int32_t write(int32_t fd, const void * buf, int32_t nbytes);
int32_t open(const uint8_t * filename);
int32_t close(int32_t fd);
int32_t getargs(uint8_t * buf, int32_t nbytes);
int32_t vidmap(uint8_t ** start_screen);
int32_t set_handler(int32_t signum, void * handler_address);
int32_t sigreturn(void);

/* Helper functions */
int8_t verify_file(const uint8_t * cmd, uint8_t inFile[CMD_LIMIT], uint32_t * v_addr);
int get_pid();

/* Current process ID */
int32_t curr;

#endif  /* _SYSCALL_H  */
