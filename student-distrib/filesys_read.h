#include "types.h"
#include "lib.h"

#define MAX_FILE_NUM 63
#define MAX_DATABLOCK_NUM 63
#define NUM_INODE_OFFSET 4
#define NUM_DBLK_OFFSET 8

uint32_t bootBlk_addr;
static int32_t num_dentry;
static int32_t num_inodes;
static int32_t num_dataBlocks;
static dentry_t *dentry_start;

typedef struct dentry_t{

    uint8_t fname[32];
    uint32_t ftype;
    uint32_t inode;
    uint8_t reserved[24];

} dentry_t;


typedef struct inode_t{

    uint32_t length;
    uint32_t data_block[MAX_DATABLOCK_NUM];

} inode_t;