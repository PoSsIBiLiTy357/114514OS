#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "lib.h"

/* Initialization struct for device referenced from: */
/* https://stackoverflow.com/questions/9932212/jump-table-examples-in-c */
typedef struct device { 
    int (*open)(const uint8_t *);
    int (*close)(int32_t);
    int (*read)(int32_t, void * , int32_t);
    int (*write)(int32_t, const void * , int32_t);
} device_t;

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
