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
static int32_t read_dir_index;


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
    datablk_start  = (uint8_t *)bootBlk_addr + (num_inodes+1)*DATABLK_SIZE;
    
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
    uint8_t *i_fname;
    //loop through all dentries
    for(i = 1; i < num_dentry; i++){
        i_fname = dentry_start[i].fname;

        //compare current i_fname with input fname
        if(strncmp((int8_t*)fname, (int8_t *)i_fname, strlen((int8_t*)fname)) == 0){
            
            //copy desire dentry info into input dentry
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

    //invalid input index check
    if( (index >= num_dentry) || (index < 0) ) return -1;

    //copy desire dentry info into input dentry
    memcpy( dentry->fname, dentry_start[index].fname, sizeof(dentry_start[index].fname));
    dentry->ftype = dentry_start[index].ftype;
    dentry->inode = dentry_start[index].inode;
    memcpy( dentry->reserved, dentry_start[index].reserved, sizeof(dentry_start[index].reserved));

    return 0;

}


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

    //invalid input checks
    if(inode == NULL) return -1;
    if(buf == NULL) return -1;
    if(inode >= num_inodes) return -1;
    //return 0 if offset reach end 
    if(offset >= inode_start[inode].length) return 0;

    inode_datablk_offset = offset / DATABLK_SIZE;
    datablk_offset = offset % DATABLK_SIZE;
    //fill buf with desired length
    for(i = 0; i<length; i++){

        if(i >= inode_start[inode].length) return 0;
        //reset index when finish 1 datablock and points to next
        if(datablk_offset == DATABLK_SIZE){
            datablk_offset = 0;
            inode_datablk_offset += 1;
        }

        buf[i] = *((uint8_t *)(datablk_start + inode_start[inode].data_block[inode_datablk_offset] * DATABLK_SIZE + datablk_offset));

        datablk_offset += 1;
    }

    return length;
}


/*
 * int32_t read_f_by_name(uint8_t *fname, int32_t offset, uint8_t *buf, uint32_t length)
 *     DESCRIPTION: read file by name and fill up buf
 *     INPUTS: uint8_t *fname, int32_t offset, uint8_t *buf, uint32_t length
 *     OUTPUTS: -1 when failed, 0 or length when success; fill buf with desire data
 *     RETURN VALUE: 0, -1 , length
 */
int32_t read_f_by_name(uint8_t *fname, int32_t offset, uint8_t *buf, uint32_t length){

    dentry_t dentry;
    if(read_dentry_by_name(fname, &dentry) == -1){
        return -1;
    }
    return read_data(dentry.inode, offset, buf, length);
}

/*
 * int32_t read_f_by_index(uint32_t index, int32_t offset, uint8_t *buf, uint32_t length)
 *     DESCRIPTION: read file by index and full up buf
 *     INPUTS: uint8_t *fname, int32_t offset, uint8_t *buf, uint32_t length
 *     OUTPUTS: -1 when failed, 0 or length when success; fill buf with desire data
 *     RETURN VALUE: 0, -1 , length
 */
int32_t read_f_by_index(uint32_t index, int32_t offset, uint8_t *buf, uint32_t length){

    dentry_t dentry;
    if(read_dentry_by_index(index, &dentry) == -1){
        return -1;
    }
    return read_data(dentry.inode, offset, buf, length);
}

/*
 * int32_t read_f(int32_t inode, int32_t offset, uint8_t *buf, uint32_t length)
 *     DESCRIPTION: read file by given name and fill up buf 
 *     INPUTS: int32_t inode, int32_t offset, uint8_t *buf, uint32_t length
 *     OUTPUTS: -1 when failed, 0 or length when success; fill buf with desire data
 *     RETURN VALUE: 0, -1 , length
 */
int32_t read_f(uint32_t inode, uint8_t *buf){
    int length = PROGRAM_MAX_SIZE;
    return read_data(inode, 0, buf, length);
}

/*
 * int32_t write_f()
 *     DESCRIPTION: write file
 *     INPUTS: none
 *     OUTPUTS: -1, always fail
 *     RETURN VALUE: -1
 */
int32_t write_f(){
    return -1;
}

/*
 * int32_t open_f()
 *     DESCRIPTION: open file
 *     INPUTS: none
 *     OUTPUTS: 0, always success
 *     RETURN VALUE: 0
 */
int32_t open_f(){
    return 0;
}

/*
 * int32_t close_f()
 *     DESCRIPTION: close file
 *     INPUTS: none
 *     OUTPUTS: 0, always success
 *     RETURN VALUE: 0
 */
int32_t close_f(){
    return 0;
}


/////////////Dirs///////////////

/*
 * int32_t open_dir()
 *     DESCRIPTION: open dir
 *     INPUTS: none
 *     OUTPUTS: 0, always success
 *     RETURN VALUE: 0
 */
int32_t open_dir(){
    read_dir_index = 0;
    return 0;
}

/*
 * int32_t close_dir()
 *     DESCRIPTION: close dir
 *     INPUTS: none
 *     OUTPUTS: 0, always success
 *     RETURN VALUE: 0
 */
int32_t close_dir(){
    read_dir_index = 0;
    return 0;
}

/*
 * int32_t read_dir(uint8_t *buf)
 *     DESCRIPTION: read dir and fill up buf with info
 *     INPUTS: uint8_t *buf
 *     OUTPUTS: success:0; fail:-1
 *     RETURN VALUE: 0, -1
 */
int32_t read_dir(uint8_t *buf){
    
    dentry_t dentry;
    if(read_dir_index >= num_dentry) return -1;
    if(read_dentry_by_index(read_dir_index, &dentry) == -1) return -1;
    memcpy(buf, dentry.fname, sizeof(dentry.fname));
    read_dir_index += 1;
    return 0;

}

/*
 * int32_t write_dir()
 *     DESCRIPTION: write dir
 *     INPUTS: none
 *     OUTPUTS: -1, always fail
 *     RETURN VALUE: -1
 */
int32_t write_dir(){
    return -1;
}



//////////////////////////////////////////////////////T   E   S   T s////////////////////////////////////////////////////////////

/*
 * int print_allfile_test()
 *     DESCRIPTION: print all dentries and their length
 *     INPUTS: none
 *     OUTPUTS: 1 always success
 *     RETURN VALUE: 1
 */
int print_allfile_test(){

    int i, j;
	clear();
    printf("***print_file_test***: \n");
    printf("num_dentry: %d, num_inodes: %d, num_dataBlocks: %d\n", num_dentry,num_inodes,num_dataBlocks);

    for(i = 0; i < num_dentry; i++){
        
        //printf("NO: %d fileName: %s, fileType: %d, #inode: %d \n", i, dentry_start[i].fname, dentry_start[i].ftype, dentry_start[i].inode); print :)

        //print one by one enforce 32 name length 
        printf("dentry NO.%d: ", i);
        for(j = 0; j < 32; j++){
            putc(dentry_start[i].fname[j]);
        }
        printf("length: %d", inode_start[dentry_start[i].inode].length);

        putc('\n');
    }

	return 1;
}


/*
 * int read_file_test(uint8_t *fname)
 *     DESCRIPTION: read file by name test
 *     INPUTS: none
 *     OUTPUTS: 1 always success
 *     RETURN VALUE: 1
 */
int read_file_test(uint8_t *fname){

    int i;
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

