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

/* Counter for RTC interrupts */
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

int exception_de_test(){
	TEST_HEADER;
	// int a = 1/0;			/* Uncomment this to run test */
	return 1;
}

int exception_test(){
    TEST_HEADER;
    asm volatile("int $1");
    return 1;
}

int page_nofault_test(){
    // TEST_HEADER;
	// int *a = 0xb8000;
	// int b = a;
	// printf("%d\n", b);
    // return PASS;
	TEST_HEADER;
	unsigned long *a = (unsigned long*)0xb8000;
	unsigned long b = (unsigned long)a;
	printf("%d\n", b);
    return PASS;
}

int page_fault_test(){
    // TEST_HEADER;
	// int *a = 0x00000;
	// int b = a;
	// printf("%d\n", b);
    // return FAIL;
	TEST_HEADER;
	unsigned long *a = 0x00000;
	unsigned long b = (unsigned long)a;
	printf("%d\n", b);
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
	if (RTC_ctr == 11) { TEST_HEADER; }

	/* Prints count of how many interrupts have been hit and increments ctr */
	if (RTC_ctr < 25 && RTC_ctr >= 10) {
		printf("Interrupt no. %d\n", (RTC_ctr - 10));
	}

	/* Stops counting after 15 interrupts */
	if (RTC_ctr == 25) {
		printf("This is the 15th successful interrupt, end of test.");
	}
}


// add more tests here

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	TEST_OUTPUT("page_nofault_test", page_nofault_test());
	TEST_OUTPUT("page_fault_test", page_fault_test());
	
	//TEST_OUTPUT("exception_de_test", exception_de_test());
	//TEST_OUTPUT("exception_test", exception_test());
	
	// launch your tests here
}
