#include "syscall.h"

#define MAGIC_7F        0x7F
#define MAGIC_E         0x45
#define MAGIC_L         0x4C
#define MAGIC_F         0x46

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

    /* Ensure the given command is a valid executable file */
    if (verify_file(command, inFile, &v_addr) == -1) { return -1; }

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
*	SIDE EFFECTS : places command filename into inFile buffer
*/
int8_t verify_file(const uint8_t * cmd, uint8_t inFile[CMD_LIMIT], uint32_t * v_addr) {
    int i, j;
    uint8_t addrBuf[4];

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
    char fileBuf[4]; /* Buffer to hold binary text */
    char magicBuf[4] = {MAGIC_7F, MAGIC_E, MAGIC_L, MAGIC_F};
    read_f_by_name(inFile, 0, (uint8_t *)fileBuf, 4);
    if (strncmp(fileBuf, magicBuf, 4)) { return -1; }

    /* Retrieve address for first instruction from bytes 24-27 */
    read_f_by_name(inFile, 24, addrBuf, 4);

    /* Convert from string to integer address */
    v_addr = 0;
    for (j = 0; j < 4; j++) {
        v_addr += (addrBuf[j] << (8 * j));
    }

    return 0;
}


int32_t read(int32_t fd, void * buf, int32_t nbytes){
    return 0;
}
int32_t write(int32_t fd, const void * buf, int32_t nbytes){
    return 0;
}
int32_t open(const uint8_t * filename){
    
    return 0;
}
int32_t close(int32_t fd){
    return 0;
}
int32_t getargs(uint8_t * buf, int32_t nbytes){
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
