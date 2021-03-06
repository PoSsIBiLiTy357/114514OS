#include "syscall.h"

#define MAGIC_7F        0x7F
#define MAGIC_E         0x45
#define MAGIC_L         0x4C
#define MAGIC_F         0x46

#define BYTE_LEN           4
#define START_ADDR        24
#define ADDR_OFFSET        8

#define PROGRAM_IMAGE_ADDR  0x8048000
#define PROC_NUM            6
#define ASM                 1
#define RESERV_FILES        2
#define ERR_BUF            26

#define FILE_RTC            0
#define FILE_DIR            1
#define FILE_REG            2

/* Table of active and inactive terminals (active = 1, inactive = 0) */
int32_t t_curr[3] = {-1,-1,-1};

/* Table of active and inactive processes (active = 1, inactive = 0) */
static int proc_state[PROC_NUM] = {0, 0, 0, 0, 0, 0};

/* Buffer for arguments and size storage */
static uint8_t argBuf[CMD_LIMIT];
static uint32_t argSize = 0;


/*
* pcb_init
*   DESCRIPTION: Initializes a new process control block for a process given
*       a process identification number (PID).
*
*   INPUTS: int pid - process identification number for process to be initialized
*   OUTPUTS: none
*   RETURN VALUE: none
*	SIDE EFFECTS : Creates a new PCB for the process with requested PID
*/
void pcb_init(int pid, int terminal_num) {
    int i;

    /* Create PCB for current process and assign PID */
    pcb_t* pcb = GET_PCB(pid);
    pcb_t* p_pcb = GET_PCB(curr);
    pcb->pid = pid;

    /* Fill in file descriptors for reserved file stdin for every new process */
    pcb->file_array[0].read  = terminal_read_wrap;  //init stdin and stdout
    pcb->file_array[0].write = terminal_wrong;
    pcb->file_array[0].open  = terminal_nothing;
    pcb->file_array[0].close = terminal_nothing;
    pcb->file_array[0].flag  = 1;
    pcb->bitmap[0] = 1;

    /* Fill in file descriptors for reserved file stdout for every new process */
    pcb->file_array[1].read  = terminal_wrong;
    pcb->file_array[1].write = terminal_write_wrap;
    pcb->file_array[1].open  = terminal_nothing;
    pcb->file_array[1].close = terminal_nothing;
    pcb->file_array[1].flag  = 1;
    pcb->bitmap[1] =1;

    pcb->current_ebp = GET_ESP(pid);
    pcb->current_esp = GET_ESP(pid);

    /* Set remaining files as unused (flag = 0) for every new process */
    for (i = RESERV_FILES; i < FDESC_SIZE; i++) {
        pcb->file_array[i].flag = 0;
    }


    /* Check if this is the first process, and if it is, set parent ptr to NULL */
    if (t_curr[terminal_num] == -1) {
        pcb->p_pid = pid;
        pcb->parent_esp = GET_ESP(pid);
        pcb->parent_esp = GET_ESP(pid);

    }
    else {
        pcb->p_pid = p_pcb->pid;
        pcb->parent_ebp = p_pcb->current_ebp;
        pcb->parent_esp = p_pcb->current_ebp;

    }
    
    /* Update state variables for what terminal(s) are active and current terminal */
    curr = pid;
    t_curr[terminal_num] = pid;
    pcb->terminal = terminal_num;

}


/*
* get_pid
*   DESCRIPTION: Finds and returns a valid PID based on how many processes
*       are currently active. Returns -1 if max number of processes (six)
*       are already active.
*
*   INPUTS: none
*   OUTPUTS: int - PID (0-5)
*   RETURN VALUE: none
*/
int get_pid() {
    int i;

    /* Look thru process table for an inactive process (marked 0) and return it */
    for(i = 0; i < PROC_NUM; i++){
        if(proc_state[i] == 0){
            return i;
        }
    }

    /* Return -1 max number of processes allowed are already in use */
    return -1;
}


/*
* halt
*   DESCRIPTION: Halts a process
*
*   INPUTS: uint8_t status - 
*   OUTPUTS: 0 if successful,
*/
int32_t halt(uint8_t status){
    int32_t i;

    cli();

    pcb_t *cur_pcb = GET_PCB(curr);
    pcb_t *par_pcb = GET_PCB(cur_pcb->p_pid);

    /* Set closing process status to inactive */
    proc_state[cur_pcb->pid] = 0;    

    /* Close any relevant FDs */
    for (i = 0; i < FDESC_SIZE; i++) {

        if (cur_pcb->file_array[i].flag){
            close(i);
        }
        cur_pcb->file_array[i].flag = 0;
    }

    /* Restore active process ID to parent process in active terminal table */
    curr = cur_pcb->p_pid;
    t_curr[cur_pcb->terminal] = cur_pcb->p_pid;

    /* Check if the process to be halted is the bottom-most process and if so, start a new shell */
    if(cur_pcb->pid == cur_pcb->p_pid){
        execute_with_terminal_num((uint8_t *)"shell", cur_pcb->terminal);
    }

    /* Restore parent paging (cr3) */
    pid_page_map(par_pcb->pid);

    /* Update the tss.esp0 */
    tss.esp0 = cur_pcb->parent_esp;   

    sti();

    /* Load in parent process's ebp and esp and save status to eax */
    asm volatile(
        "movl   %0, %%esp   ;"
        "movl   %1, %%ebp   ;"
        "movl   %2, %%eax   ;"
        "jmp RETURN_FROM_IRET;"

        :
        :"r"(cur_pcb->parent_esp), "r"(cur_pcb->parent_ebp), "r"((uint32_t) status)  
        :"%eax"
    );


    return 0;

}


/*
* execute
*   DESCRIPTION: Executes a user level progam by and hands processor
*           off to the new program until it terminates.
*
*   INPUTS: uint8_t * command - first string is filename followed by args to
*               be interpreted by getargs()
*   OUTPUTS: none
*   RETURN VALUE: 0 on success, -1 on failure
*	SIDE EFFECTS : Switches processor to user mode to run given user program
*/
int32_t execute(const uint8_t * command){
    pcb_t *pcb = GET_PCB(curr);
    return execute_with_terminal_num(command, pcb->terminal);
}


/*
* execute_with_terminal_num
*   DESCRIPTION: Helper function for execute that allows program to 
*           identify which terminal to run in.
*
*   INPUTS: uint8_t * command - first string is filename followed by args to
*               be interpreted by getargs()
*           int terminal_num - terminal number (0-2)
*   OUTPUTS: none
*   RETURN VALUE: 0 on success, -1 on failure
*	SIDE EFFECTS : Switches processor to user mode to run given user program
*/
int32_t execute_with_terminal_num(const uint8_t * command,int terminal_num){
    uint8_t inFile[CMD_LIMIT];  /* name of executable file           */
    uint32_t v_addr;            /* virtual addr of first instruction */
    dentry_t d;
    int pid;
    char errMsg[ERR_BUF] = "TOO MANY PROCESSES OPEN!\n";

    /* Ensure parameter is valid */
    if (command == NULL) { return -1; }

    cli();
    
    /* Ensure the given command is a valid executable file */   
    if (verify_file(command, inFile, &v_addr) == -1) { return -1; }
    
    /* Fetch a proccess id that is not in use */
    pid = get_pid();

    /* Ensure we don't launch too many processes */
    if (pid == -1) {
        terminal_write(errMsg, ERR_BUF);
        return 0; 
    }

    /* Mark pid as used */
    proc_state[pid] = 1;      
    
    /* Create PCB */
    pcb_init(pid, terminal_num);
    pcb_t* pcb = GET_PCB(pid);

	/* Saving the current ESP and EBP into the PCB struct */
	asm volatile("			\n\
				movl %%ebp, %%eax 	\n\
				movl %%esp, %%ebx 	\n\
			"
			:"=a"(pcb->parent_ebp), "=b"(pcb->parent_esp)
    );


    /* Initialize paging for process */
    pid_page_map(pid);

    /* Update current terminal number and init paging for that terminal */
    set_active_terminal(terminal_num);

    /* User-level Program Loader */
    read_dentry_by_name(inFile, &d);
    read_f(d.inode, (uint8_t *)PROGRAM_IMAGE_ADDR);

    /* Update TSS */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = GET_ESP(pid);
    pcb->current_ebp = tss.esp0;
    pcb->current_esp = tss.esp0;
    
    sti();

    /* IRET setup and context switch */
    asm volatile(       
        "mov $0x2B, %%ax;"
        "mov %%ax, %%ds;"
        "movl $0x83FFFFC, %%eax;"
        "pushl $0x2B;"
        "pushl %%eax;"
        "pushfl;"
        "pushl $0x23;"
        "pushl %0;"
        "iret;"
        "RETURN_FROM_IRET:;"
        "LEAVE;"
        "RET;"
        :	            /* no outputs         */
        :"r"(v_addr)	/* input              */
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
    int i;  /* Loop index */

    /* Make sure passed in ptr is not a nullptr */
    if (cmd == NULL) { return -1; }

    /* Retrieve file name */
    memset(inFile, 0, CMD_LIMIT);
    for (i = 0; i < strlen((char *)cmd); i++) { 
        if (cmd[i] == ' ') break;
        inFile[i] = cmd[i];
    }
    inFile[i] = '\0';   /* Add sentinel to end of filename */

    /* Clear argsize and argBuf and parse arguments for getargs() */
    argSize -= argSize;
    memset(argBuf, 0, CMD_LIMIT);
    while (cmd[i] != '\0') {
        /* Skip spaces */
        if (cmd[i] == ' ') {
            i++;    continue;
        }

        /* Copy arg into argBuf */
        argBuf[argSize++] = cmd[i++];
    }

    /* Look in filesystem to see check if given file exists */
    dentry_t dentry_buf;
    if (read_dentry_by_name(inFile, &dentry_buf) == -1) { return -1; }

    /* Check to make sure file is an executable by checking for magic number */
    char fileBuf[BYTE_LEN]; /* Buffer to hold binary text */
    char magicBuf[BYTE_LEN] = {MAGIC_7F, MAGIC_E, MAGIC_L, MAGIC_F};
    read_f_by_name(inFile, 0, (uint8_t *)fileBuf, BYTE_LEN);
    if (strncmp(fileBuf, magicBuf, BYTE_LEN)) { return -1; }

    /* Retrieve address for first instruction from bytes 24-27 */
    read_f_by_name(inFile, START_ADDR, (uint8_t*)v_addr, BYTE_LEN);

    return 0;
}


/*
* read
*   DESCRIPTION: Reads a given file by creating a temporary PCB to retrieve the
*       starting offset and inode for that specific file and then jump to the
*       respective read handler for the file
*
*   INPUTS: uint32_t fd - file descriptor
*           void * buf - buffer to fill up with chars from file
*           int32_t nbytes - how many characters to read 
*   OUTPUTS: number of characters read
*   RETURN VALUE: int32_t - number of characters read 
*                 -1 on failure (no characters to read)
*/
int32_t read(int32_t fd, void * buf, int32_t nbytes){
    /* Ensure parameter is valid */
    if (buf == NULL) { return -1; }

    /* Check that we haven't exceeded max number of open files */
    if (fd >= FDESC_SIZE || fd < 0) { return -1; }

    /* Create a temporary PCB and initalize offest and inode for file to then jump to read() */
    pcb_t * temp_pcb = GET_PCB(curr);
    
    /* Ensure file to read has been opened */
    if(temp_pcb->file_array[fd].flag == 0) return -1;

    /* Read in file starting offset and inode to pass in when calling read() */
    int offset = temp_pcb->file_array[fd].file_pos;
    int inode = temp_pcb->file_array[fd].inode;
    int count = temp_pcb->file_array[fd].read(inode, offset, buf, nbytes);

    /* Check if read handler returned an error, if so, return -1 */
    if (count < 0) return -1;

    /* Increment the read offset */
    temp_pcb->file_array[fd].file_pos += count;

    return count;
}


/*
* write
*   DESCRIPTION: Writes to a given file by creating a temporary PC to retrive
*       file inode and jump to the respective write handler for file 
*
*   INPUTS: uint32_t fd - file descriptor
*           void * buf - buffer to write to file
*           int32_t nbytes - how many characters were written
*   OUTPUTS: number of characters written
*   RETURN VALUE: int32_t - number of characters written
*                 -1 on failure (no characters to written)
*/
int32_t write(int32_t fd, const void * buf, int32_t nbytes){
    /* Ensure parameter is valid */
    if (buf == NULL) { return -1; }

    /* Check that we haven't exceeded max number of open files */
    if (fd >= FDESC_SIZE || fd < 0 || buf == NULL) return -1;

    /* Create a temporary PCB and initalize inode for file to then jump to write() */
    pcb_t * temp_pcb = GET_PCB(curr);
    
    /* Ensure file to write to has been opened */
    if(temp_pcb->bitmap[fd] == 0) return -1;

    /* Read in inode to pass in when calling write() */
    int inode = temp_pcb->file_array[fd].inode;
    int count = temp_pcb->file_array[fd].write(inode, 0, (uint8_t *)buf, nbytes);

    /* Check if read handler returned an error, if so, return -1 */
    if (count < 0) return -1;

    return count;
}


/*
* open
*   DESCRIPTION: Opens a file and checks for what file type it is to
*       give it its proper initializations
*
*   INPUTS: const uint8_t * filename
*   OUTPUTS: file index
*   RETURN VALUE: file index or -1 on failure
*/
int32_t open(const uint8_t * filename){ 
    pcb_t * pcb;
    dentry_t temp_dentry;
    int i;

    /* Ensure parameter is valid */
    if (filename == NULL) { return -1; }

    /* initialize PCB to the bottom of kernel stack offest by 8MB * curr PID */
    pcb = GET_PCB(curr);

    /* If the filename does not exist, return -1 */
    if (read_dentry_by_name(filename, &temp_dentry) ==-1) { return -1; } 
    
    for (i = 2; i < FDESC_SIZE; i++) {
        if (pcb->bitmap[i] == 0) {
            pcb->bitmap[i] = 1;
            break;
        } 
    }
    if (i == FDESC_SIZE) return -1;     /* if no descriptors are free, return -1 */
    
    /* Check if the file type is an RTC file */
    if (temp_dentry.ftype == FILE_RTC) {
        pcb->file_array[i].read  = rtc_read_wrapper;
        pcb->file_array[i].write = rtc_write_wrapper;
        pcb->file_array[i].open  = rtc_open_wrapper;
        pcb->file_array[i].close = rtc_close_wrapper;
    }
    
    /* Check if file type is a directory file */
    if (temp_dentry.ftype == FILE_DIR) {
        open_dir();
        pcb->file_array[i].read  = read_dir_wrapper;
        pcb->file_array[i].write = write_dir_wrapper;
        pcb->file_array[i].open  = open_dir_wrapper;
        pcb->file_array[i].close = close_dir_wrapper;        
    }

    /* Check if file type is a regular file */
    if (temp_dentry.ftype == FILE_REG) {
        
        pcb->file_array[i].read  = read_f_wrapper;
        pcb->file_array[i].write = write_f_wrapper;
        pcb->file_array[i].open  = open_f_wrapper;
        pcb->file_array[i].close = close_f_wrapper;           
    }

    /* Set the inode for current file and set starting offset to 0 */
    pcb->file_array[i].inode = temp_dentry.inode;
    pcb->file_array[i].flag = 1; // file in use 
    pcb->file_array[i].file_pos = 0;
    
    return i;
}


/*
* close
*   DESCRIPTION: Closes a given file and updates the file array 
*
*   INPUTS: int32_t fd
*   OUTPUTS: 0 on success, -1 on fail
*/
int32_t close(int32_t fd){
    /* Make sure given file descriptor is within range */
    if (fd > FDESC_SIZE || fd < MIN_FILES) { return -1; }
    
    /* Create a temporary PCB and then jump to close() */
    pcb_t *temp_pcb = GET_PCB(curr);

    /* Ensure file to write to has been opened */
    if(temp_pcb->file_array[fd].flag == 0) { return -1; }

    /* Update file array status for current process */
    temp_pcb->file_array[fd].flag = 0;
    temp_pcb->bitmap[fd] = 0;
    
    return 0;
}


/*
*  getargs
*   DESCRIPTION: Returns the last arguments passed into execute 
*
*   INPUTS: uint8_t * buf
*           int32_t nbytes
*   OUTPUTS: 0 on success, -1 on fail
*/
int32_t getargs(uint8_t * buf, int32_t nbytes) {
    int i;

    /* Make sure buffer constraints are fulfilled */
    if (buf == NULL || argSize == 0) { return -1; }

    /* Copy arguments into buf after resetting it */
    memset(buf, 0, strlen((char *)buf));
    for (i = 0; i < strlen((char *)argBuf); i++) { buf[i] = argBuf[i]; }
    buf[i] = '\0';

    return 0;
}


/*
*  vidmap
*   DESCRIPTION: 
*
*   INPUTS: uint8_t ** start_screen
*   OUTPUTS: 
*   RETURN VALUE: 
*/
int32_t vidmap(uint8_t ** start_screen) {   
    if(start_screen == NULL || start_screen == (uint8_t **)_4MB_){
        return -1;
    } 
    int t_display = get_display_terminal();
    vidMem_page_map((int)(132 * _MB_), t_display);

    *start_screen = (uint8_t *)(132 * _MB_);

    return 132 * _MB_;
}




/*
*  set_handler
*   DESCRIPTION: NOT USED
*
*   INPUTS: uint32_t signum
*           void * handler_address
*   OUTPUTS: 
*   RETURN VALUE: 
*/
int32_t set_handler(int32_t signum, void * handler_address){
    return 0;
}


/*
*  sigreturn
*   DESCRIPTION: NOT USED
*
*   INPUTS: none
*   OUTPUTS: 
*   RETURN VALUE: 
*/
int32_t sigreturn(void){
    return 0;
}
