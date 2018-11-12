#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "filesys_read.h"
#include "keyboard.h"
#include "paging.h"
#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}

/* Counter for RTC interrupts declared in RTC.c */
extern int RTC_ctr;

/*****************************************************************************/
/*						 Checkpoint 1 tests									 */
/*****************************************************************************/

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}


/*
* exception_de_test()
*   DESCRIPTION: This is a simple test to trigger a divide by zero
* 		exception (by dividing an integer by 0 of course).
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: int 1 (should not be reached)
*   NOTE: variable declaration line triggers a compiler warning, 
*		  uncomment to use test case.
*/
int exception_de_test(){
	TEST_HEADER;
	int a = 1;			/* Uncomment this to run test */
	int b = 0;
	a = a/b;
	return 1;
}


/*
* exception_test()
*   DESCRIPTION: This is a simple test to trigger an exception.
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: int 1 (should not be reached)
*/
int exception_test(){
    TEST_HEADER;
    asm volatile("int $33");
    return 1;
}


/*
* page_nofault_test()
*   DESCRIPTION: This is a positive test case that should pass. This tests
*		page faults by attempting to dereference valid memory at address
*		0xb8000 (this is where video memory is stored). Page fault should not
*		trigger an exception here.
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: int PASS
*/
int page_nofault_test(){
	TEST_HEADER;
	int *a = (int *)0xB8000;
	int b = *a;
	b++;
	printf("%d\n", *a);
    return PASS;
}


/*
* page_fault_test()
*   DESCRIPTION: This is a negative test case that should fail. This tests
*		page faults by attempting to dereference inaccessible memory at
*		address 0x00000.
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: int FAIL
*	SIDE EFFECTS : triggers page fault
*/
int page_fault_test(){
	TEST_HEADER;
	int *a = (int *)0x00000;
	int b = *a;
	b++;
	printf("%d\n", *a);
    return FAIL;
}


/*
* RTC_test
*   DESCRIPTION: This is a test function called by RTC interrupt handler to simply 
*		print the number of interrupts that have been triggered. This only counts 
*		up to the 15th interrupt.
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: none
*	SIDE EFFECTS : increments an interrupt counter global variable RTC_ctr
*/
void RTC_test() {
	/* Print the test header only once */
	if (RTC_ctr == RTC_HEADER_CHK) { TEST_HEADER; }

	/* Prints count of how many interrupts have been hit and increments ctr */
	if (RTC_ctr < RTC_HEADER_LIM && RTC_ctr >= RTC_START_PRIN) {
		printf("Interrupt no. %d\n", (RTC_ctr - RTC_START_PRIN));
	}

	/* Stops counting after 15 interrupts */
	if (RTC_ctr == RTC_HEADER_LIM) {
		printf("This is the 15th successful interrupt, end of test.");
	}
}


// add more tests here

/*****************************************************************************/
/*						 Checkpoint 2 tests									 */
/*****************************************************************************/

/*
* RTC_freq_test
*   DESCRIPTION: This function tests RTC at varying frequencies. Frequencies are
*		changed by making calls to write() and open()
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: none
*	SIDE EFFECTS : changes RTC frequency
*/
int RTC_freq_test() {
	/* Wait 5 interrupt cycles and clear the screen */
    while (RTC_ctr < TICK_DELAY) {  }
	clear();

	/* Dummy file descriptor */
	int fd;

	while (RTC_ctr < INIT_UPPER_BND) {
		printf("Interrupting at 2Hz. Interrupt number: %d\n", RTC_ctr);
	}

	/* Clear screen and change frequency to 16 Hz */
	clear();

	/* Buffer to store desired frequency */
	uint32_t filename[1] = {VAL_16_HZ};

	rtc_write(fd, filename, NUM_VALID_BYTES);
	while (RTC_ctr < SIXTEEN_UPPER_BND) {
		printf("Interrupt at 16Hz due to write(): %d\n", RTC_ctr);
	}


	/* Clear screen and change frequency to 64 Hz */
	clear();

	/* Buffer to store desired frequency */
	filename[0] = VAL_64_HZ;

	rtc_write(fd, filename, NUM_VALID_BYTES);
	while (RTC_ctr < SIXTYFOUR_UPPER_BND) {
		printf("Interrupt at 64Hz due to write(): %d\n", RTC_ctr);
	}

	/* Clear screen and change frequency to 1024 Hz */
	clear();

	/* Buffer to store desired frequency */
	filename[0] = VAL_512_HZ;

	rtc_write(fd, filename, NUM_VALID_BYTES);
	while (RTC_ctr < FIVETWELVE_UPPER_BND) {
		printf("Interrupt at 512Hz due to write(): %d\n", RTC_ctr);
	}

	/* Clear screen and change frequency to 1024 Hz */
	clear();

	/* Buffer to store desired frequency */
	filename[0] = VAL_1024_HZ;

	rtc_write(fd, filename, NUM_VALID_BYTES);
	while (RTC_ctr < MAX_FREQ_UPPER_BND) {
		printf("Interrupt at 1024Hz due to write(): %d\n", RTC_ctr);
	}

	
	/* Reset frequency to 2Hz with open() */
	clear();

	/* Dummy file */
	const uint8_t * file;

	fd = rtc_open(file);
	while (RTC_ctr < OPEN_UPPER_BND) {
		printf("Interrupting back to 2 Hz after open(): %d\n", RTC_ctr);
	}	

	/* Clear screen once more */
	clear();
	
	TEST_HEADER;
	return PASS;
}


/*
* RTC_valid_size_test
*   DESCRIPTION: This function tests that a valid integer size (4) can be passed
*		into RTC_write() and returns the proper size back (4).
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: none
*	SIDE EFFECTS : calls RTC_write() with buffer size 4
*/
int RTC_valid_size_test() {
	TEST_HEADER;

	/* Dummy valid file descriptor and filename */
	int fd;
	uint32_t filename[1] = {VAL_16_HZ};
	
	/* Check rtc_write return value */
	if (rtc_write(fd, filename, NUM_VALID_BYTES) == sizeof(uint32_t)) 
		return PASS;
	return FAIL;
}


/*
* RTC_invalid_size_test
*   DESCRIPTION: This function tests that an invalid integer size (5) can be passed
*		into RTC_write() and returns with error value -1.
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: none
*	SIDE EFFECTS : calls RTC_write() with buffer size 5
*/
int RTC_invalid_size_test() {
	TEST_HEADER;

	/* Dummy valid file descriptor and filename */
	int fd;
	uint32_t filename[1] = {VAL_16_HZ};

	/* Call rtc_write with invalid nbytes size 5 */
	if (rtc_write(fd, filename, 5) == -1) 
		return PASS;
	return FAIL;
}


/*
* RTC_empty_buf_test
*   DESCRIPTION: This function tests that an empty buffer passed into RTC_write()
*		will return -1
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: none
*	SIDE EFFECTS : calls RTC_write() with empty buffer
*/
int RTC_empty_buf_test() {
	TEST_HEADER;

	/* Dummy valid file descriptor and filename */
	int fd;
	uint32_t * filename = NULL;

	/* Check rtc_write return value */
	if (rtc_write(fd, filename, NUM_VALID_BYTES) == -1) 
		return PASS;
	return FAIL;
}


/*
* RTC_base_2_test
*   DESCRIPTION: This function tests that only base 2 numbers or valid frequencies
* 		may be passed in.
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: none
*	SIDE EFFECTS : calls RTC_write() with various numbers
*/
int RTC_base_2_test() {
	TEST_HEADER;

	/* Dummy valid file descriptor and filename */
	int fd;
	int i;	/* Loop index */
	int retval;
	uint32_t filename[1];

	/* Array of values to test, all values should pass except the first two (0 and 1) */
	uint32_t test_val[NUM_FREQ_TESTS] = { 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
	
	/* Loop through testing each value and calling RTC_write() */
	for (i = 0; i < NUM_FREQ_TESTS; i++) {
		filename[0] = test_val[i];

		/* Call rtc_write() */
		retval = rtc_write(fd, filename, NUM_VALID_BYTES);

		if ((retval == -1) && i > 1) {
			/* Return fail if any of the valid test values are considered invalid */
			return FAIL;
		} else if ((retval == sizeof(uint32_t)) && i <= 1) {
			/* Return fail if any invalid test values are considered valid */
			return FAIL;
		}
	}

	/* Return PASS otherwise */
	return PASS;
}


/*
* RTC_read_test
*   DESCRIPTION: This function tests the rtc_read function blocks until next
*		interrupt
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: none
*	SIDE EFFECTS : calls RTC_write() with various numbers
*/
int RTC_read_test() {
	TEST_HEADER;

	/* Dummy valid file descriptor and filename */
	int fd;
	uint32_t filename[1] = {VAL_2_HZ};

	/* Reset RTC_ctr */
	RTC_ctr = 0;

	/* Wait 5 clock cycles then clear screen */
	while (RTC_ctr < TICK_DELAY) {}
	clear();

	/* Loop thru counting up for 15 ticks */
	while (RTC_ctr < READ_UPPER_BND) {
		printf("read_test() interrupt count: %d\n", RTC_ctr);

		/* Call read() to block on tick number 10 and 15 */
		if (RTC_ctr == BLOCK_CASE_10 || RTC_ctr == BLOCK_CASE_15) {
			printf("BLOCKING");
			rtc_read(fd, filename, NUM_VALID_BYTES);
		}
	}

	/* Clear screen */
	clear();

	return PASS;
}

void check_terminal_write(){
	terminal_write("hnikjedshfkjdhskjfhdkjshkjfdheriufhisuhifuhdiuhsiuhiuerhiuhfsiuhiueihsufeishufeishfueisfh");
}



/*****************************************************************************/
/*						 Checkpoint 3 tests									 */
/*****************************************************************************/

/*
* exec_valid_file_test
*   DESCRIPTION: This function tests execute() can handle valid files by testing
*		the helper function verify_file() (which execute uses to validate inputs)
*		returns 0 when given valid inputs.
*
*   INPUTS: valid files
*   OUTPUTS: PASS or FAIL
*   RETURN VALUE: PASS if execute returns 0 (we want verify_file to return 0)
*/
int exec_valid_file_test() {
	TEST_HEADER;

	int i;
	char * cmd[2] = {"shell", "testprint"};	/* valid filenames */
	uint8_t dummyBuffer[CMD_LIMIT];
	uint32_t dummy_addr;

	/* Make sure each execute returns 0 for these valid files */
	for (i = 0; i < 2; i++) {
		/* return 0 test */
		if (verify_file((uint8_t *)cmd[i], dummyBuffer, &dummy_addr) != 0) {		
			return FAIL;
		}

		/* Ensure dummyBuffer gets populated with the right file name */
		if (strncmp((char *)dummyBuffer, cmd[i], sizeof(cmd[i])) != 0) {
			return FAIL;
		}
	}

	/* Print starting virtual address from bytes 24-27 */
	printf("starting v_addr: %x\n", dummy_addr);

	return PASS;
}


/*
* exec_invalid_file_test
*   DESCRIPTION: This function tests execute() will return -1 by testing its
*		helper funciton verify_file() (which execute uses to validate inputs)
*		given an invalid input.
*
*   INPUTS: invalid files
*   OUTPUTS: PASS or FAIL
*   RETURN VALUE: PASS if execute returns -1 (we want verify_file to return a fail)
*/
int exec_invalid_file_test() {
	TEST_HEADER;

	uint8_t cmd[10] = "114514OS";	/* Invalid filename */
	uint8_t dummyBuffer[CMD_LIMIT];
	uint32_t dummy_addr;

	/* Make sure verify_file returns -1 since we passed an invalid file */
	if (verify_file(cmd, dummyBuffer, &dummy_addr) == -1) {
		/* Print starting virtual address from bytes 24-27 */
		printf("starting v_addr: %x\n", dummy_addr);

		return PASS;
	}

	return FAIL;
}


/*
* exec_null_file_test
*   DESCRIPTION: This function tests execute() will return -1 by testing its
*		helper funciton verify_file() (which execute uses to validate inputs)
*		given NULL as an argument.
*
*   INPUTS: invalid files
*   OUTPUTS: PASS or FAIL
*   RETURN VALUE: PASS if execute returns -1 (we want verify_file to return a fail)
*/
int exec_null_file_test() {
	TEST_HEADER;

	uint8_t * cmd = NULL;
	uint8_t dummyBuffer[CMD_LIMIT];
	uint32_t dummy_addr;

	/* Make sure verify_file returns -1 since we passed an invalid file */
	if (verify_file(cmd, dummyBuffer, &dummy_addr) == -1) {
		/* Print starting virtual address from bytes 24-27 */
		printf("starting v_addr: %x\n", dummy_addr);
		
		return PASS;
	}

	return FAIL;
}


int page_nofault_test_b5(){
	TEST_HEADER;

	clear();

	int *a = (int *)0x8048000;

	pid_page_map(0);
	*a = 5;

	pid_page_map(1);
	*a = 6;

	pid_page_map(0);
	

	printf("%d\n", *a);
    return PASS;
}


/*
* print_open_file_test
*	Simple test that prints all the PCB contents after opening a file (shell).
*
*   INPUTS: valid file shell
*   OUTPUTS: PASS
*   RETURN VALUE: PASS
*/
int print_open_file_test() {
	TEST_HEADER;
	int i;
	int32_t fd = 0;

	pcb_t * pcb_shell;
	pcb_shell = (pcb_t *)(KSTACK_BOT);

	/* Print PC before open */
	printf("pcb_shell struct before open:\n");
	printf("file_array: {");
	for (i = 0; i < FDESC_SIZE - 1; i++) {
		printf("0x%x, ",pcb_shell->file_array[i]);
	}
	printf("0x%x}\n", pcb_shell->file_array[FDESC_SIZE-1]);
	printf("bitmap: {");
	for (i = 0; i < FDESC_SIZE - 1; i++) {
		printf("%d, ",pcb_shell->bitmap[i]);
	}
	printf("%d}\n", pcb_shell->bitmap[FDESC_SIZE-1]);
	printf("pid: %d\n\n", pcb_shell->pid);

	/* Open the shell file */
	fd = open((uint8_t *)"shell");
	printf("open(shell) returned: %d\n\n", fd);

	/* Print PCB after open */
	printf("pcb_shell struct after open:\n");
	printf("file_array: {");
	for (i = 0; i < FDESC_SIZE - 1; i++) {
		printf("0x%x, ",pcb_shell->file_array[i]);
	}
	printf("0x%x}\n", pcb_shell->file_array[FDESC_SIZE-1]);
	printf("bitmap: {");
	for (i = 0; i < FDESC_SIZE - 1; i++) {
		printf("%d, ",pcb_shell->bitmap[i]);
	}
	printf("%d}\n", pcb_shell->bitmap[FDESC_SIZE-1]);
	printf("pid: %d\n", pcb_shell->pid);

	return PASS;
}


/*
* print_open_file_test
*	Simple test that prints all the PCB contents after opening a file and
* 	then printing PCB contents after closing file to show open and close
*	update the bitmap fields.
*
*   INPUTS: valid file shell
*   OUTPUTS: PASS
*   RETURN VALUE: PASS
*/
int open_then_close_test() {
	TEST_HEADER;

	int i;
	int32_t fd = 0, close_output = 0;

	pcb_t * pcb_shell;
	pcb_shell = (pcb_t *)(KSTACK_BOT);

	/* Open the file shell */
	fd = open((uint8_t *)"shell");
	printf("open(shell) returned: %d\n\n", fd);

	/* Print the PCB struct after opening the file */
	printf("pcb_shell struct after open:\n");
	printf("file_array: {");
	for (i = 0; i < FDESC_SIZE - 1; i++) {
		printf("0x%x, ",pcb_shell->file_array[i]);
	}
	printf("0x%x}\n", pcb_shell->file_array[FDESC_SIZE-1]);
	printf("bitmap: {");
	for (i = 0; i < FDESC_SIZE - 1; i++) {
		printf("%d, ",pcb_shell->bitmap[i]);
	}
	printf("%d}\n", pcb_shell->bitmap[FDESC_SIZE-1]);
	printf("pid: %d\n\n", pcb_shell->pid);

	/* Close the file shell */
	close_output = close(fd);
	printf("close(fd) returned: %d\n\n", close_output);

	/* Print the PCB struct after closing the file */
	printf("pcb_shell struct after close:\n");
	printf("file_array: {");
	for (i = 0; i < FDESC_SIZE - 1; i++) {
		printf("0x%x, ",pcb_shell->file_array[i]);
	}
	printf("0x%x}\n", pcb_shell->file_array[FDESC_SIZE-1]);
	printf("bitmap: {");
	for (i = 0; i < FDESC_SIZE - 1; i++) {
		printf("%d, ",pcb_shell->bitmap[i]);
	}
	printf("%d}\n", pcb_shell->bitmap[FDESC_SIZE-1]);
	printf("pid: %d\n\n", pcb_shell->pid);

	return PASS;
}


/*
* print_open_file_test
*	Simple test that prints all the PCB contents after opening two files and
* 	then printing PCB contents after closing a file to show open and close
*	update the bitmap fields.
*
*   INPUTS: valid file shell
*   OUTPUTS: PASS
*   RETURN VALUE: PASS
*/
int open_open_close_test() {
	TEST_HEADER;

	int i;
	int32_t fd1 = 0, fd2 = 0, close_output = 0;

	pcb_t * pcb_shell;
	pcb_shell = (pcb_t *)(KSTACK_BOT);

	/* Open the file shell */
	fd1 = open((uint8_t *)"shell");
	printf("open(shell) returned: %d\n", fd1);

	/* Open the file fish */
	fd2 = open((uint8_t *)"fish");
	printf("open(fish) returned: %d\n\n", fd2);

	/* Print the PCB struct after opening the file */
	printf("pcb_shell struct after open:\n");
	printf("file_array: {");
	for (i = 0; i < FDESC_SIZE - 1; i++) {
		printf("0x%x, ",pcb_shell->file_array[i]);
	}
	printf("0x%x}\n", pcb_shell->file_array[FDESC_SIZE-1]);

	printf("bitmap: {");
	for (i = 0; i < FDESC_SIZE - 1; i++) {
		printf("%d, ",pcb_shell->bitmap[i]);
	}
	printf("%d}\n", pcb_shell->bitmap[FDESC_SIZE-1]);

	printf("pid: %d\n\n", pcb_shell->pid);

	/* Close the file shell */
	close_output = close(fd1);
	printf("close(fd1) (fd1 = shell's fd) returned: %d\n\n", close_output);

	/* Print the PCB struct after closing the file */
	printf("pcb_shell struct after close:\n");
	printf("file_array: {");
	for (i = 0; i < FDESC_SIZE - 1; i++) {
		printf("0x%x, ",pcb_shell->file_array[i]);
	}
	printf("0x%x}\n", pcb_shell->file_array[FDESC_SIZE-1]);

	printf("bitmap: {");
	for (i = 0; i < FDESC_SIZE - 1; i++) {
		printf("%d, ",pcb_shell->bitmap[i]);
	}
	printf("%d}\n", pcb_shell->bitmap[FDESC_SIZE-1]);

	printf("pid: %d\n\n", pcb_shell->pid);

	return PASS;
}


/*
* open_file_max
*	Makes sure open() exits and returns -1 when file capacity has
*	has already been reached 
*
*   INPUTS: valid file (shell)
*   OUTPUTS: PASS if return -1, FAIL otherwise
*   RETURN VALUE: PASS, FAIL
*/
int open_file_max_test() {
	TEST_HEADER;

	int i;
	int32_t output;

	pcb_t * pcb_shell;
	pcb_shell = (pcb_t *)(KSTACK_BOT);
	
	/* Open max number of files (six) */
	for (i = 0; i < FDESC_SIZE; i++) {
		output = open((uint8_t *)"shell");
	}

	/* Show all files are occupied */
	printf("pcb_shell struct after six opens:\n");
	printf("file_array: {");
	for (i = 0; i < FDESC_SIZE - 1; i++) {
		printf("0x%x, ",pcb_shell->file_array[i]);
	}
	printf("0x%x}\n", pcb_shell->file_array[FDESC_SIZE-1]);

	printf("bitmap: {");
	for (i = 0; i < FDESC_SIZE - 1; i++) {
		printf("%d, ",pcb_shell->bitmap[i]);
	}
	printf("%d}\n", pcb_shell->bitmap[FDESC_SIZE-1]);

	/* Open one more file above max number */
	output = open((uint8_t *)"shell");

	/* Make sure that output returne -1 when trying to open last file */
	if (output == -1) { 
		return PASS; 
	} else {
		return FAIL;
	}
}




// maybe user for later...
//printf("*parent: 0x%x\n", pcb_shell->parent);
// printf("parent_esp: %d\n", pcb_shell->parent_esp);
// printf("parent_ebp: %d\n", pcb_shell->parent_ebp);

/*****************************************************************************/
/*						 Checkpoint 4 tests									 */
/*****************************************************************************/
// coming kinda soon.

/*****************************************************************************/
/*						 Checkpoint 5 tests									 */
/*****************************************************************************/
// bleh..... wo chow




/* Test suite entry point */
void launch_tests(){
	/* Checkpoint 1 tests */
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("page_nofault_test", page_nofault_test());
	//TEST_OUTPUT("page_nofault_test", page_nofault_test_b5());

	//TEST_OUTPUT("page_fault_test", page_fault_test());
	//TEST_OUTPUT("page_fault_test", page_fault_test());
	//TEST_OUTPUT("exception_de_test", exception_de_test());
	//TEST_OUTPUT("exception_test", exception_test());

	/* RTC tests */
	//TEST_OUTPUT("RTC_freq_test", RTC_freq_test());
	// TEST_OUTPUT("RTC_read_test", RTC_read_test());
	// TEST_OUTPUT("RTC_empty_buf_test", RTC_empty_buf_test());
	// TEST_OUTPUT("RTC_valid_size_test", RTC_valid_size_test());
	// TEST_OUTPUT("RTC_invalid_size_test", RTC_invalid_size_test());
	// TEST_OUTPUT("RTC_base_2_test", RTC_base_2_test());
	
	/* File system tests */
	//uint8_t fname[] = "frame0.txt";		/* select file name */
	//TEST_OUTPUT("print_allfile_test", print_allfile_test());
	//TEST_OUTPUT("read_file_test", read_file_test(fname));
	//terminal_write("qwertyuiop[]\asdfghjkl;'zxcvbnm,./bfgdhfdgfdgfdgfdjkgfodjglkfjdlkgkfjkdglkjflkdjklgjklfjlkdjlgjfkdjgofiejdgprokepokpofkld;lgjhfl;djhn;lfmblc;lkfl;dk;lrfkpoerkpogkd;lkfl;gk12323432543267687686786565");

	/* System calls tests */
	//TEST_OUTPUT("exec_valid_file_test()", exec_valid_file_test());
	//TEST_OUTPUT("exec_invalid_file_test()", exec_invalid_file_test());
	//TEST_OUTPUT("exec_null_file_test()", exec_invalid_file_test());
	clear();
	//TEST_OUTPUT("print_open_file_test()", print_open_file_test());
	//TEST_OUTPUT("open_then_close_test()", open_then_close_test());
	//TEST_OUTPUT("open_open_close_test()", open_open_close_test());
	TEST_OUTPUT("open_file_max_test()", open_file_max_test());

	/*terminal test*/
	//check_terminal_write();


	/*execute test*/
	execute((uint8_t *)"shell");

}
