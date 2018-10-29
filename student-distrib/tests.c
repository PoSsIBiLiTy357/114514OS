#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

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


/* Checkpoint 1 tests */

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
    asm volatile("int $1");
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

/* Checkpoint 2 tests */

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
    while (RTC_ctr < 5) {  }
	clear();

	/* Dummy file descriptor */
	int fd;

	while (RTC_ctr < 10) {
		printf("Interrupting at 2Hz. Interrupt number: %d\n", RTC_ctr);
	}

	/* Clear screen and change frequency to 16 Hz */
	clear();

	/* Buffer to store desired frequency */
	uint32_t filename[1] = {16};

	rtc_write(fd, filename, 4);
	while (RTC_ctr < 50) {
		printf("Interrupt at 16Hz due to write(): %d\n", RTC_ctr);
	}


	/* Clear screen and change frequency to 64 Hz */
	clear();

	/* Buffer to store desired frequency */
	filename[0] = 64;

	rtc_write(fd, filename, 4);
	while (RTC_ctr < 200) {
		printf("Interrupt at 64Hz due to write(): %d\n", RTC_ctr);
	}

	/* Clear screen and change frequency to 1024 Hz */
	clear();

	/* Buffer to store desired frequency */
	filename[0] = 512;

	rtc_write(fd, filename, 4);
	while (RTC_ctr < 2000) {
		printf("Interrupt at 512Hz due to write(): %d\n", RTC_ctr);
	}

	/* Clear screen and change frequency to 1024 Hz */
	clear();

	/* Buffer to store desired frequency */
	filename[0] = 1024;

	rtc_write(fd, filename, 4);
	while (RTC_ctr < 5000) {
		printf("Interrupt at 1024Hz due to write(): %d\n", RTC_ctr);
	}


	/* Reset frequency to 2Hz with open() */
	clear();
	fd = rtc_open(filename);
	while (RTC_ctr < 5010) {
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
	uint32_t filename[1] = {16};

	if (rtc_write(fd, filename, 4) == sizeof(uint32_t)) 
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
	uint32_t filename[1] = {16};

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
	uint32_t * filename;

	if (rtc_write(fd, filename, 4) == -1) 
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
	uint32_t filename[1] ;
	uint32_t test_val[12] = { 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
	int i;

	for (i = 0; i < 12; i++) {
		filename[0] = test_val[i];
		if ((rtc_write(fd, filename, 4) == -1) && i > 1) {
			return FAIL;
		} else if ((rtc_write(fd, filename, 4) == sizeof(uint32_t)) && i <= 1) {
			return FAIL;
		}
	}
	return PASS;
}


/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("RTC_freq_test", RTC_freq_test());
	TEST_OUTPUT("idt_test", idt_test());
	TEST_OUTPUT("page_nofault_test", page_nofault_test());
	//TEST_OUTPUT("RTC_freq_test", RTC_freq_test());
	//TEST_OUTPUT("RTC_empty_buf_test", RTC_empty_buf_test());
	TEST_OUTPUT("RTC_valid_size_test", RTC_valid_size_test());
	TEST_OUTPUT("RTC_invalid_size_test", RTC_invalid_size_test());
	TEST_OUTPUT("RTC_base_2_test", RTC_base_2_test());
	
	//TEST_OUTPUT("RTC_freq_test", RTC_freq_test());
//	TEST_OUTPUT("page_fault_test", page_fault_test());
	
	//TEST_OUTPUT("exception_de_test", exception_de_test());
	//TEST_OUTPUT("exception_test", exception_test());
	
	// launch your tests here
}
