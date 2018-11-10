#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "lib.h"
#include "rtc.h"
#include "filesys_read.h"

#define FDESC_SIZE          8
#define CMD_LIMIT         129

/* Initialization struct for device referenced from: */
/* https://stackoverflow.com/questions/9932212/jump-table-examples-in-c */
typedef struct file_desc_t { 
    int (*open)(int32_t,int32_t, int8_t* , int32_t); ///////*********possible bug change to uint ***********//////////////////////////
    int (*close)(int32_t,int32_t, int8_t* , int32_t); ///////*********possible bug change to uint ***********//////////////////////////
    int (*read)(int32_t,int32_t, int8_t* , int32_t);///////*********possible bug change to uint ***********//////////////////////////
    int (*write)(int32_t,int32_t, int8_t* , int32_t);///////*********possible bug change to uint ***********//////////////////////////
    int32_t inode,file_pos,flags;
} file_desc_t;

typedef struct pcb_t{
    int32_t *parent;
    int32_t parent_esp;
    int32_t parent_ebp;
    file_desc_t file_array[FDESC_SIZE];
    char bitmap[FDESC_SIZE];
    int32_t pid;
} pcb_t;



/* System calls */
int32_t halt(uint8_t status);
int32_t execute(const uint8_t * command);
int32_t read(int32_t fd, void * buf, int32_t nbytes);
int32_t write(int32_t fd, const void * buf, int32_t nbytes);
int32_t open(const uint8_t * filename);
int32_t close(int32_t fd);
int32_t getargs(uint8_t * buf, int32_t nbytes);
int32_t vidmap(uint8_t ** start_screen);
int32_t set_handler(int32_t signum, void * handler_address);
int32_t sigreturn(void);

/* Helper functions */
int8_t verify_file(const uint8_t * command, uint8_t inFile[CMD_LIMIT]);

#endif  /* _SYSCALL_H  */
