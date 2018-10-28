#include "filesys_read.h"
#include "lib.h"
#include "types.h"
#include "x86_desc.h"

static int32_t num_dentry;
static int32_t num_inodes;
static int32_t num_dataBlocks;
static dentry_t *dentry_start;
static inode_t *inode_start;
static uint8_t *datablk_start;

void read_filesys_bootblock(uint32_t bootBlk_addr){

    num_dentry     = *((int32_t *)bootBlk_addr);
    num_inodes     = *((int32_t *)(bootBlk_addr + NUM_INODE_OFFSET));
    num_dataBlocks = *((int32_t *)(bootBlk_addr + NUM_DBLK_OFFSET));
    dentry_start   = (dentry_t *)(bootBlk_addr + DENTRY_START_OFFSET);
    inode_start    = (inode_t *)(bootBlk_addr + INODE_START_OFFSET);
    datablk_start  = (uint8_t *)(bootBlk_addr + INODE_START_OFFSET + num_inodes*sizeof(inode_t));
    
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
        if(strlen(fname) != strlen(i_fname)) continue;
        if(strncmp(fname, i_fname, strlen(fname))){
            
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

    //inode_t *target_inode;
    uint8_t *buf_cur;
    int i, inode_datablk_offset, to_end, ret;

    if(inode >= num_inodes) return -1;
    //target_inode = inode_start[inode];
    if(offset >= inode_start[inode].length) return 0;

    buf_cur = buf;

    if(offset + length < inode_start[inode].length){
        //1. offset + length < t_inode.length
        for(i = 0; i < length; i++){

            inode_datablk_offset = (offset + i)/4096;
            memcpy(buf_cur, datablk(inode_start[inode].data_block[inode_datablk_offset]), 1);
            buf_cur += 4;

        }

        ret = length;

    }else{
        //2. offset + length > t_inode.length: 
        to_end = inode_start[inode].length - offset;
        for(i = 0; i < to_end; i++){

            inode_datablk_offset = (offset + i)/4096;
            memcpy(buf_cur, datablk(inode_start[inode].data_block[inode_datablk_offset]), 1);
            buf_cur += 4;

        }

        ret = 0;

    }

    return ret;
    
}







int32_t file_write()
{
    return -1;
}

int32_t file_open()
{
    return 0;
}

int32_t file_close()
{
    return 0;
}

int32_t dir_open()
{
    //current_dir_read_index = 0;
    return 0;
}

int32_t dir_close()
{
    //current_dir_read_index = 0;
    return 0;
}

int32_t dir_write()
{
    return -1;
}



int print_allfile_test(){

    int i;

	clear();
    printf("***print_file_test***: \n");
    printf("num_dentry: %d, num_inodes: %d, num_dataBlocks: %d\n", num_dentry,num_inodes,num_dataBlocks);

    for(i = 0; i < num_dentry; i++){
        
        printf("NO: %d fileName: %s, fileType: %d, #inode: %d \n", i, dentry_start[i].fname, dentry_start[i].ftype, dentry_start[i].inode);

    }

	return 1;
}


int read_file_test(int index){

    int i, j, blk_offset, b_offset;
    dentry_t d;
    inode_t  inode;

	clear();

    read_dentry_by_index(index, &d);
    printf("***read_file_test***: \n");
    printf("d.fname: %s \n", d.fname);
    printf("d.inode: %d \n", d.inode);

    inode = inode_start[d.inode];
    printf("length in B: %d\n",inode.length);
    printf("data block #: %d\n",inode.data_block[0]);
    blk_offset = inode.length/4096;
    for(i = 0; i < blk_offset+1; i++){
        
        for(j = 0; j < inode.length; j++){
            if(i != 0) b_offset = 4096*i;
            putc( datablk(inode.data_block[i])+j - b_offset );
        }

    }

	return 1;
}

