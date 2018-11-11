#ifndef _FILESYS_READ_H
#define _FILESYS_READ_H

#include "types.h"
#include "lib.h"
#include "x86_desc.h"

#define MAX_FILE_NUM 63
#define MAX_DATABLOCK_NUM 1023
#define NUM_INODE_OFFSET 4
#define NUM_DBLK_OFFSET 8
#define DENTRY_START_OFFSET 64
#define INODE_START_OFFSET  4096
#define DATABLK_SIZE  4096
#define datablk(i)  (datablk_start + DATABLK_SIZE*i)
#define PROGRAM_MAX_SIZE 1024*4

typedef struct {

    uint8_t fname[32];    //32 byte fname size specified in doc
    uint32_t ftype;
    uint32_t inode;         
    uint8_t reserved[24]; //24 byte reserved size specified in doc

} dentry_t;


typedef struct {
 
    uint32_t length;
    uint32_t data_block[MAX_DATABLOCK_NUM];

} inode_t;

/* file system functions */
void    read_filesys_bootblock();
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

/* file system calls */
//int32_t read_f_by_name(uint8_t *fname, int32_t offset, uint8_t *buf, uint32_t length);    /* compiler error */
int32_t read_f_by_name(uint8_t *fname, uint32_t offset, uint8_t *buf, uint32_t length);
int32_t read_f_by_index(uint32_t index, int32_t offset, uint8_t *buf, uint32_t length);
//int32_t read_f(uint32_t inode, uint8_t *buf);                                             /* compiler error */
int32_t read_f(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length);
int32_t write_f();
int32_t open_f();
int32_t close_f();

/* dir system calls */
int32_t open_dir();
int32_t close_dir();
int32_t read_dir(uint8_t *buf);
int32_t write_dir();

/* test functions */
int print_allfile_test();
int read_file_test(uint8_t *fname);

int read_dir_wrapper(uint32_t inode,uint32_t offset, uint8_t* buf, uint32_t count);
int write_dir_wrapper(uint32_t inode,uint32_t offset, uint8_t* buf, uint32_t count);
int open_dir_wrapper(uint32_t inode,uint32_t offset, uint8_t* buf, uint32_t count);
int close_dir_wrapper(uint32_t inode,uint32_t offset, uint8_t* buf, uint32_t count);
int read_f_wrapper(uint32_t inode,uint32_t offset, uint8_t* buf, uint32_t count);
int write_f_wrapper(uint32_t inode,uint32_t offset, uint8_t* buf, uint32_t count);
int open_f_wrapper(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count);
int close_f_wrapper(uint32_t inode,uint32_t offset, uint8_t* buf, uint32_t count);

#endif  /* _FILESYS_READ_H  */  
////wrappers

