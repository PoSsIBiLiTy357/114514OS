#include "syscall.h"

#define MAGIC_7F        0x7F
#define MAGIC_E         0x45
#define MAGIC_L         0x4C
#define MAGIC_F         0x46

#define BYTE_LEN           4
#define START_ADDR        24
#define ADDR_OFFSET        8


#define PROC_NUM 6
#define PCB_SIZE 0x2000
#define KSTACK_BOT 0x800000-0x2000
#define ASM 1

static int curr = 0;
//device_t rtc = { rtc_read, rtc_write, rtc_open, rtc_close };
static int proc_state[PROC_NUM] = {0, 0, 0, 0, 0, 0};


void pcb_init(int pid){
    int i;
    curr = pid;
    
    pcb_t* pcb= (pcb_t *)(KSTACK_BOT - PCB_SIZE * pid);
    pcb->pid=pid;
    pcb->file_array[0].read = terminal_read_wrap;///init stdin and stdout     /* compiler error */
    pcb->file_array[0].write=NULL;
    pcb->file_array[0].open=NULL;
    pcb->file_array[0].close=NULL;
    pcb->file_array[0].flag=1;

    pcb->file_array[1].read=NULL;
    pcb->file_array[1].write= terminal_write_wrap;                           /* compiler error */
    pcb->file_array[1].open=NULL;
    pcb->file_array[1].close=NULL;
    pcb->file_array[1].flag=1;

    for(i=2;i<FDESC_SIZE;i++){
        pcb->file_array[i].flag=0;
    }
    if(pid==0){
        pcb->parent=NULL;
    }
    else{
        pcb->parent = (int32_t *)(KSTACK_BOT - PCB_SIZE * pid + PCB_SIZE);
    }
}

int get_pid(){
    int i;
    for(i=0;i<PROC_NUM;i++){
        if(proc_state[i]==0){
            return i;
        }
    }
    return -1;
}


//device_t rtc = { rtc_read, rtc_write, rtc_open, rtc_close };

int32_t halt(uint8_t status){
    return 0;
}


/*
* execute
*   DESCRIPTION: Executes a user level progam by and hands processor
*           off to the new program until it terminates.
*   NOTE: does not take args into account, need to work on this...
*
*   INPUTS: uint8_t * command - first string is filename followed by args to
*               be interpreted by getargs()
*   OUTPUTS: none
*   RETURN VALUE: 0 on success, -1 on failure
*	SIDE EFFECTS : Switches processor to user mode to run given user program
*/

int32_t execute(const uint8_t * command){
    uint8_t inFile[CMD_LIMIT];  /* name of executable file           */
    uint32_t v_addr;            /* virtual addr of first instruction */
    dentry_t d;
    int pid;
    //u parse/check
    /* Ensure the given command is a valid executable file */

    
    if (verify_file(command, inFile, &v_addr) == -1) { return -1; }

    //fetch a proccess id that is not in use 
    pid=get_pid();
    proc_state[pid]=1;
    //u Paging
    paging_init(pid);

    //u User-level Program Loader


    read_dentry_by_name(inFile, d);
    read_f(d.inode, (uint8_t)0x08048000);
    
    //u Create PCB
    pcb_init(pid);

    //u Context Switch 




    tss.esp0 = KSTACK_BOT - PCB_SIZE * curr - 4;
    pcb_t* pcb = (pcb_t *)(KSTACK_BOT - PCB_SIZE * curr);
  /* tss.esp0 = KSTACK_BOT + PCB_SIZE - PCB_SIZE * curr - 4;
    pcb_t* pcb = (pcb_t *)(KSTACK_BOT - PCB_SIZE * curr);
    __asm__ volatile(
        "cli;"
        "movl %%ebp,%0;"
        "movl %%esp,%1;"
        "movl %2,%%ebp;"
        "movl %2,%%esp;"
       "movw $0x002B,%%ax;"
       "movw %%ax,%%ds;"
       "movw %%ax,%%es;"
       "movw %%ax,%%fs;"
       "movw %%ax,%%gs;"
       "movl %%esp,%%eax;"
       "pushl $0x002B;"
       "pushl $0x083FFFFC;"
       "pushf;"
       "popl %%eax;"
       "orl $0x200,%%eax;"
       "pushl %%eax;"
       "pushl $0x0023;"
       "pushl %3;"
       "iret;"
        :"=r" (pcb->parent_ebp),"=r" (pcb->parent_esp) 
        :"r"  (tss.esp0),"r" (v_addr)
        :"eax"); 
    return 0;
}*/////////////////////////////////////////////  alternate version 
     
     
	/* Saving the current ESP and EBP into the PCB struct */
	asm volatile("			\n\
				movl %%ebp, %%eax 	\n\
				movl %%esp, %%ebx 	\n\
			"
			:"=a"(pcb->parent_ebp), "=b"(pcb->parent_esp));

        asm volatile(
                 "cli;"
                 "mov $0x2B, %%ax;"
                 "mov %%ax, %%ds;"
                 "movl $0x83FFFFC, %%eax;"
                 "pushl $0x2B;"
                 "pushl %%eax;"
                 "pushfl;"
                 "popl %%edx;"
                 "orl $0x200, %%edx;"
                 "pushl %%edx;"
                 "pushl $0x23;"
                 "pushl %0;"
                 "iret;"
                 "RETURN_FROM_IRET:;"
                 "LEAVE;"
                 "RET;"
                 :	/* no outputs */
                 :"r"(v_addr)	/* input */
                 :"%edx","%eax"	/* clobbered register */
                 );

    return 0;
}


/*
* verify_file
*   DESCRIPTION: Simply verifies that the file command name is valid and
*       also checks that it is an executable file by checking for the 
*       ELF magic number.
*
*   INPUTS: uint8_t * command - first string is filename followed by args to
*               be interpreted by getargs()
*           uint8_t inFile[CMD_LIMIt] - buffer to hold the name of the file
*   OUTPUTS: 0 or -1
*   RETURN VALUE: 0 on success, -1 on failure
*	SIDE EFFECTS : places command filename into inFile buffer and starting
                   user address into v_addr
*/
int8_t verify_file(const uint8_t * cmd, uint8_t inFile[CMD_LIMIT], uint32_t * v_addr) {
    int i, j;
    uint8_t addrBuf[BYTE_LEN];

    /* Make sure passed in ptr is not a nullptr */
    if (cmd == NULL) { return -1; }

    /* Retrieve file name */
    for (i = 0; i < strlen((char *)cmd); i++) { 
        if (cmd[i] == ' ') break;
        inFile[i] = cmd[i];
    }
    inFile[i] = '\0';   /* Add sentinel to end of filename */

    /* Look in filesystem to see check if given file exists */
    dentry_t dentry_buf;
    if (read_dentry_by_name(inFile, &dentry_buf) == -1) { return -1; }

    /* Check to make sure file is an executable by checking for magic number */
    char fileBuf[BYTE_LEN]; /* Buffer to hold binary text */
    char magicBuf[BYTE_LEN] = {MAGIC_7F, MAGIC_E, MAGIC_L, MAGIC_F};
    read_f_by_name(inFile, 0, (uint8_t *)fileBuf, BYTE_LEN);
    if (strncmp(fileBuf, magicBuf, BYTE_LEN)) { return -1; }

    /* Retrieve address for first instruction from bytes 24-27 */
    read_f_by_name(inFile, START_ADDR, addrBuf, BYTE_LEN);

    /* Convert from string to integer address */
    v_addr = 0;
    for (j = 0; j < BYTE_LEN; j++) {
        v_addr += (addrBuf[j] << (ADDR_OFFSET * j));
    }

    return 0;
}


int32_t read(int32_t fd, void * buf, int32_t nbytes){
    if (fd>= FDESC_SIZE) return -1;
    pcb_t * temp_pcb = (pcb_t*) KSTACK_BOT-curr*PCB_SIZE;
    int offset = temp_pcb->file_array[fd].file_pos;
    int inode = temp_pcb->file_array[fd].inode;
    int count = temp_pcb->file_array[fd].read(inode,offset,buf,nbytes);
    if (count<0) return -1;
    temp_pcb->file_array[fd].file_pos += count;
    return count;
    
}
int32_t write(int32_t fd, const void * buf, int32_t nbytes){
    
    if (fd >= FDESC_SIZE) return -1;
    pcb_t * temp_pcb = (pcb_t*) KSTACK_BOT-curr*PCB_SIZE;
    int inode = temp_pcb->file_array[fd].inode;
    int count = temp_pcb->file_array[fd].write(inode, 0, (uint8_t *)buf, nbytes);
    if (count <0) return -1;
    return count;
}
int32_t open(const uint8_t * filename){
    
    pcb_t pcb_t;
    dentry_t temp_dentry;
    int i;
    if (read_dentry_by_name(filename, &temp_dentry) ==-1) return -1; // if the filename does not exist, return -1
    for (i = 0; i <FDESC_SIZE;i++){
        if (pcb_t.bitmap[FDESC_SIZE]==0){
            pcb_t.bitmap[FDESC_SIZE]=1;
            break;
        } 
    }
    if (i==FDESC_SIZE) return -1; //if no descriptors are free, return -1
    
    if(temp_dentry.ftype == 0){
        pcb_t.file_array[i].read = rtc_read_wrapper;
        pcb_t.file_array[i].write = rtc_write_wrapper;
        pcb_t.file_array[i].open = rtc_open_wrapper;
        pcb_t.file_array[i].close = rtc_close_wrapper;
    }
    
    if (temp_dentry.ftype == 1){
        pcb_t.file_array[i].read = read_dir_wrapper;
        pcb_t.file_array[i].write = write_dir_wrapper;
        pcb_t.file_array[i].open = open_dir_wrapper;
        pcb_t.file_array[i].close = close_dir_wrapper;        
    }
    if (temp_dentry.ftype ==2){
        pcb_t.file_array[i].read = read_f_wrapper;
        pcb_t.file_array[i].write = write_f_wrapper;
        pcb_t.file_array[i].open = open_f_wrapper;
        pcb_t.file_array[i].close = close_f_wrapper;           
    }
    pcb_t.file_array[i].inode = temp_dentry.inode;
    pcb_t.file_array[i].flag =1; // file in use 
    pcb_t.file_array[i].file_pos = 0;
    return 0;

}

int32_t close(int32_t fd){
    if (fd>=FDESC_SIZE) return -1;
    pcb_t * temp_pcb;
    int temp_pcb_addr = 0x800000-0x2000-curr*0x2000;
    temp_pcb = (pcb_t *) temp_pcb_addr;
    temp_pcb->file_array[fd].flag =0;
    return 0;

}


int32_t getargs(uint8_t * buf, int32_t nbytes){
    // if(){
    //     return -1;
    // }
    // if(){
    //     return-1;
    // }
    return 0;
}
int32_t vidmap(uint8_t ** start_screen){
    return 0;
}
int32_t set_handler(int32_t signum, void * handler_address){
    return 0;
}
int32_t sigreturn(void){
    return 0;
}
