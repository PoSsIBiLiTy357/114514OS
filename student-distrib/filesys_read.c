#include <string.h>
#include "filesys_read.h"

#define MAX_FILE_NUM 63
#define NUM_INODE_OFFSET 4
#define NUM_DBLK_OFFSET 8

static int32_t num_entries;
static int32_t num_inodes;
static int32_t num_dataBlocks;

void read_filesys_bootblock(){

    int i;
    num_entries =(int32_t *)bootBlk_addr;
    num_inodes = (int32_t *)(bootBlk_addr + NUM_INODE_OFFSET);
    num_dataBlocks =(int32_t *)(bootBlk_addr + NUM_DBLK_OFFSET);

}

/*  return 0 when successful, the first two calls fill in the dentry t block passed as their second argument with the file name, file
type, and inode number for the file, then 
    return -1 when failure, indicating a non-existent file or invalid index in the case of the first two calls, or an invalid inode number 
in the case of the last routine */


/*
 * int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
 *     DESCRIPTION: When successful, fill in the dentry t block passed as their second argument 
 *                  with the file name, file type, and inode number for the file, then return 0
 *     INPUTS: const uint8_t* fname, dentry_t* dentry
 *     OUTPUTS: none
 *     RETURN VALUE: none
 *     SIDE EFFECTS: initialize PDT idx entry as 4KB entry
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){

    char *i_fname;
    for(i = 0; i < num_entries; i++){
        *i_fname = ((int32_t *)(bootBlk_addr + (i+1)*64));
        if(strcmp(fname, i_fname)){
            
            
            memcpy(void* dest, const void* src, uint32_t n)

            return 0;
        }
    }

    return -1;
}


int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){




}




/*The last routine works much like the read system call, reading up to
length bytes starting from position offset in the file with inode number inode and returning the number of bytes
read and placed in the buffer. A return value of 0 thus indicates that the end of the file has been reached. */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){





    
}