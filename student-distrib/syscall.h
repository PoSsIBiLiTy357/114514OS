#ifndef _SYSCALL_H
#define _SYSCALL_H

#define FDESC_SIZE 8

#include "lib.h"

/*typedef struct func_pointer{
    int (*open)(int32_t,int32_t, int8_t* , int32_t); 
    int (*close)(int32_t,int32_t, int8_t* , int32_t); 
    int (*read)(int32_t,int32_t, int8_t* , int32_t);
    int (*write)(int32_t,int32_t, int8_t* , int32_t);


} func_pointer;
*/


/* Initialization struct for device referenced from: */
/* https://stackoverflow.com/questions/9932212/jump-table-examples-in-c */
typedef struct file_desc_t { 
    //func_pointer * pointer;
    int (*open)(uint32_t,uint32_t, uint8_t* , uint32_t); ///////*********possible bug change to uint ***********//////////////////////////
    int (*close)(uint32_t,uint32_t, uint8_t* , uint32_t); ///////*********possible bug change to uint ***********//////////////////////////
    int (*read)(uint32_t,uint32_t, uint8_t* , uint32_t);///////*********possible bug change to uint ***********//////////////////////////
    int (*write)(uint32_t,uint32_t, uint8_t* , uint32_t);///////*********possible bug change to uint ***********//////////////////////////
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

#endif
