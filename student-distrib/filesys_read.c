#include <string.h>
#include "filesys_read.h"
#include "lib.h"
#include "types.h"

#define DENTRY_START_OFFSET 64



void read_filesys_bootblock(){

    int i;
    num_dentry     = (int32_t *)bootBlk_addr;
    num_inodes     = (int32_t *)(bootBlk_addr + NUM_INODE_OFFSET);
    num_dataBlocks = (int32_t *)(bootBlk_addr + NUM_DBLK_OFFSET);
    dentry_start   = (dentry_t *)(bootBlk_addr + DENTRY_START_OFFSET);

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

    int i;
    char *i_fname;
    for(i = 1; i < num_dentry; i++){
        i_fname = dentry_start[i].fname;
        if(strcmp(fname, i_fname)){
            
            memcpy( dentry->fname, dentry_start[i].fname, sizeof(dentry_start[i].fname));
            dentry->ftype = dentry_start[i].ftype;
            dentry->inode = dentry_start[i].inode;
            memcpy( dentry->reserved, dentry_start[i].reserved, sizeof(dentry_start[i].reserved));
            
            return 0;
        }
    }

    return -1;
}


int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){

    if( (index >= num_dentry) || (index < 0) ) return -1;

    memcpy( dentry->fname, dentry_start[index].fname, sizeof(dentry_start[index].fname));
    dentry->ftype = dentry_start[index].ftype;
    dentry->inode = dentry_start[index].inode;
    memcpy( dentry->reserved, dentry_start[index].reserved, sizeof(dentry_start[index].reserved));

    return 0;

}




/*The last routine works much like the read system call, reading up to
length bytes starting from position offset in the file with inode number inode and returning the number of bytes
read and placed in the buffer. A return value of 0 thus indicates that the end of the file has been reached. */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){

    
    
    
}



