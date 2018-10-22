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
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	TEST_OUTPUT("page_nofault_test", page_nofault_test());
//	TEST_OUTPUT("page_fault_test", page_fault_test());
	
	//TEST_OUTPUT("exception_de_test", exception_de_test());
	//TEST_OUTPUT("exception_test", exception_test());
	
	// launch your tests here
}
