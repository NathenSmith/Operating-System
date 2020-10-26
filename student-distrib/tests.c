#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#include "linkage.h" //REMOVE LATER

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


/* Checkpoint 1 tests */

/* exception_test
 *
 * Tests the exceptions by dereferencing a null pointer.
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */

int exception_test(){
	TEST_HEADER;
	int *res;
	int a = 1;
	res = &a;
	res = NULL;
	int exception = *res;
	return exception;
}


/* divide_test
 *
 * Tests the exceptions by dividing by zero.
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */

int divide_test(){
	TEST_HEADER;
	//return 1 / 0;
	return 0;
}

/* system_call_2
 *
 * Tests the system call using the int 0x80 instruction.
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */

int system_call_2(){
	asm volatile(
          "movl $10, %eax;"
		  "int $0x80"
    );
	return 0;
}

/* Paging Out of Bounds Test
 *
 * Checks if an invalid memory address can be accessed
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Files: paging.h/c
 */

int paging_oob(){
	int * invalidInt = (int*)(0xB8000 - 1);
	int x;
	x = *invalidInt;
	return FAIL;
}

/* Paging In Bounds Test
 *
 * Checks if an valid memory address can be accessed
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Files: paging.h/c
 */

int paging_ib(){
	int * validInt = (int*)(0xB8000 + 1);
	int x;
	x = *validInt;
	return PASS;
}

// add more tests here

/* Checkpoint 2 tests */

int list_files(uint32_t start_addr){
	int i;

  dentry_t result;
	dentry_t * resultPtr = &result;

	boot_block_t * boot_block = (boot_block_t *) start_addr;
	for(i = 0; i < boot_block->dir_count; i++){
		int errorCheck = read_dentry_by_name((uint8_t *)boot_block->direntries[i].filename, resultPtr);
		//errorCheck = read_dir(i, currentFile, FILENAME_LEN);
		if(errorCheck == -1) return FAIL;

		printf(resultPtr->filename);
		// printf(resultPtr->filetype);
		printf("\n");
	}
	return PASS;

}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests(uint32_t input_start_addr){
	uint32_t start_addr = input_start_addr;
	TEST_OUTPUT("List Files", list_files(start_addr));
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("Dereference NULL test", exception_test());
	//TEST_OUTPUT("divide-by-zero test", divide_test());
	//TEST_OUTPUT("system call test 2", system_call_2());
	//TEST_OUTPUT("paging test 1", paging_oob());
	//TEST_OUTPUT("paging test 2", paging_ib());
	// launch your tests here
}
