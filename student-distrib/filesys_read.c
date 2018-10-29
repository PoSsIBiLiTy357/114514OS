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


/*
 * void read_filesys_bootblock(uint32_t bootBlk_addr)
 *     DESCRIPTION: Initialize file system, read in all the metadata in first dentry
 *     INPUTS: uint32_t bootBlk_addr
 *     OUTPUTS: none
 *     RETURN VALUE: none
 */
void read_filesys_bootblock(uint32_t bootBlk_addr){

    num_dentry     = *((int32_t *)bootBlk_addr);
    num_inodes     = *((int32_t *)(bootBlk_addr + NUM_INODE_OFFSET));
    num_dataBlocks = *((int32_t *)(bootBlk_addr + NUM_DBLK_OFFSET));
    dentry_start   = (dentry_t *)(bootBlk_addr + DENTRY_START_OFFSET);
    inode_start    = (inode_t *)(bootBlk_addr + INODE_START_OFFSET);
    datablk_start  = (char *)bootBlk_addr + (num_inodes+1)*DATABLK_SIZE;
    
}


/*
 * int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
 *     DESCRIPTION: When successful, fill in the dentry t block passed as their second argument 
 *                  with the file name, file type, and inode number for the file, then return 0
 *     INPUTS: const uint8_t* fname, dentry_t* dentry
 *     OUTPUTS: return 0  when successful, the first two calls fill in the dentry t block passed as their second argument with the file name, file
 *                  type, and inode number for the file, then 
 *              return -1 when failure, indicating a non-existent file or invalid index in the case of the first two calls, or an invalid inode number 
 *                  in the case of the last routine
 *     RETURN VALUE: success:0 failed:-1
 *     SIDE EFFECTS: initialize PDT idx entry as 4KB entry
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){

    int i;
    char *i_fname;
    for(i = 1; i < num_dentry; i++){
        i_fname = dentry_start[i].fname;
        //if(strlen((uint8_t*)fname) != strlen(i_fname)) continue;
        if(strncmp((uint8_t*)fname, i_fname, strlen((uint8_t*)fname)) == 0){
            
            memcpy( dentry->fname, dentry_start[i].fname, sizeof(dentry_start[i].fname));
            dentry->ftype = dentry_start[i].ftype;
            dentry->inode = dentry_start[i].inode;
            memcpy( dentry->reserved, dentry_start[i].reserved, sizeof(dentry_start[i].reserved));
            
            return 0;
        }
    }

    return -1;
}

/*
 * int32_t read_dentry_by_index (uint32_t index dentry_t* dentry)
 *     DESCRIPTION: When successful, fill in the dentry t block passed as their second argument 
 *                  with the file name, file type, and inode number for the file, then return 0
 *     INPUTS: uint32_t index dentry_t* dentry
 *     OUTPUTS: return 0  when successful, the first two calls fill in the dentry t block passed as their second argument with the file name, file
 *                  type, and inode number for the file, then 
 *              return -1 when failure, indicating a non-existent file or invalid index in the case of the first two calls, or an invalid inode number 
 *                  in the case of the last routine
 *     RETURN VALUE: success:0 failed:-1
 *     SIDE EFFECTS: initialize PDT idx entry as 4KB entry
 */
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

/*
 * int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
 *     DESCRIPTION: The last routine works much like the read system call, reading up to
 *                  length bytes starting from position offset in the file with inode number inode
 *     INPUTS: uint32_t bootBlk_addr
 *     OUTPUTS: return the number of bytes read and placed in the buffer.
 *              return value of 0 thus indicates that the end of the file has been reached
 *              return value of -1 when failed
 *     RETURN VALUE: success:length  reach to end:0  failed:-1
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){

    int i, inode_datablk_offset, datablk_offset;

    if(inode == NULL) return -1;
    if(buf == NULL) return -1;
    if(inode >= num_inodes) return -1;
    if(offset >= inode_start[inode].length) return 0;

    inode_datablk_offset = offset / DATABLK_SIZE;
    datablk_offset = offset % DATABLK_SIZE;

    for(i = 0; i<length; i++){

        if(datablk_offset == DATABLK_SIZE){
            datablk_offset = 0;
            inode_datablk_offset += 1;
        }

        buf[i] = *((uint8_t *)(datablk_start + inode_start[inode].data_block[inode_datablk_offset] * DATABLK_SIZE + datablk_offset));

        datablk_offset += 1;
    }

    return length;
}



int32_t read_f_by_name(uint8_t *fname, int32_t offset, uint8_t *buf, uint32_t length){

    dentry_t dentry;
    if(read_dentry_by_name(fname, &dentry) == -1){
        return -1;
    }
    return read_data(dentry.inode, offset, buf, length);
}


int32_t read_f_by_index(uint32_t index, int32_t offset, uint8_t *buf, uint32_t length){

    dentry_t dentry;
    if(read_dentry_by_index(index, &dentry) == -1){
        return -1;
    }
    return read_data(dentry.inode, offset, buf, length);
}


int32_t read_f(uint8_t *fname, int32_t offset, uint8_t *buf, uint32_t length){

    dentry_t dentry;
    if(read_dentry_by_name(fname, &dentry) == -1){
        return -1;
    }
    return read_data(dentry.inode, offset, buf, length);
}


int32_t write_f(){
    return -1;
}

int32_t open_f(){
    return 0;
}

int32_t close_f(){
    return 0;
}





int32_t open_dir(){
    //current_dir_read_index = 0;
    return 0;
}

int32_t close_dir(){
    //current_dir_read_index = 0;
    return 0;
}

int32_t read_dir(){
    return -1;
}

int32_t write_dir(){
    return -1;
}

/////////////TESTs///////////////

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


int read_file_test(uint8_t *fname){

    int i, j;
    dentry_t d;
    inode_t  inode;
    uint8_t buf[38000];

	clear();

    read_dentry_by_name(fname, &d);
    printf("***read_file_test***: \n");
    printf("d.fname: %s \n", d.fname);
    printf("d.inode: %d \n", d.inode);

    inode = inode_start[d.inode];
    printf("length in B: %d\n",inode.length);
    printf("1st data block #: %d\n",inode.data_block[0]);
    printf("2nd data block #: %d\n",inode.data_block[1]);
    printf("3rd data block #: %d\n",inode.data_block[2]);
/*
    printf("4th data block #: %d\n",inode.data_block[3]);
    printf("5th data block #: %d\n",inode.data_block[4]);
    printf("6th data block #: %d\n",inode.data_block[5]);
    printf("7th data block #: %d\n",inode.data_block[6]);
    printf("8th data block #: %d\n",inode.data_block[7]);
    printf("9th data block #: %d\n",inode.data_block[8]);
    printf("10th data block #: %d\n",inode.data_block[9]);
    printf("11th data block #: %d\n",inode.data_block[10]);
*/
    memset(buf, 0, sizeof(buf));

    read_f_by_name(fname, 0, buf, 38000);
    for(i=0; i<inode.length; i++){
        //if(buf[i] == '\0') break;
        putc(buf[i]);
    }

    putc('\n');
	return 1;
}

