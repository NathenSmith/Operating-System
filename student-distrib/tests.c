#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "terminal.h"

#define PASS 1
#define FAIL 0

#define BLOCK_SIZE 4096

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


/* List Files Test
 *
 * Prints the name, type, and size for every file in the directory
 * Inputs: starting addr of filesystem
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Files: filesystem.h/c
 */

int list_files(uint32_t start_addr) {
	clear();
	int i;

	dentry_t result;
	dentry_t * resultPtr = &result;

	boot_block_t * boot_block = (boot_block_t *) start_addr;
	for(i = 0; i < boot_block->dir_count; i++){
		int errorCheck = read_dentry_by_name((uint8_t *)boot_block->direntries[i].filename, resultPtr);
		if(errorCheck == -1) return FAIL; //make sure that there exists file with that filename

		uint32_t inode_num = resultPtr->inode_num;
		inode_t * inodes = (inode_t *)(start_addr + BLOCK_SIZE);
		inode_t currentInode = inodes[inode_num]; // find inode associated with file
		uint32_t length = currentInode.length;

		int8_t string_to_print[33]; //filename has a size of 32 + 1 for end char
		strncpy(string_to_print, resultPtr->filename, 32);
		string_to_print[32] = '\0';

		printf("file name: "); //print out filename, file type, and file size
		printf(string_to_print);
		printf("     file type: ");
		printf("%d", resultPtr->filetype);
		printf("     file size: ");
		printf("%d", length);
		printf("\n");
	}
	return PASS;
}

int read_dir_test(start_addr) {
	int i;
	//boot_block_t * boot_block = (boot_block_t *) start_addr;
	uint8_t filename[33];
	for(i = 0; i < 100; i++) {
		filename[0] = '\0';
		dir_read(0, filename, 0);
		filename[32] = '\0';
		if(filename[0] != '\0') {
			printf("filename: %s\n", filename);
		}
	}
	return 0;
}
/* RTC test
 *
 * Prints 1's at every possible frequency, from lowest to highest
 * Inputs: none
 * Outputs: PASS
 * Side Effects: Prints to screen
 * Files: rtc.c/h, keyboard.c/h
 */
int rtc_test(){
	TEST_HEADER;
	int32_t i, f = 2;
	int32_t *f_ptr = &f;
	while(f <= 1024){
		rtc_write(0, f_ptr, 4);
		for(i = 0; i < 10; i++){
			rtc_read(0,0,0);
			add_to_kdb_buf('1');
		}
		*f_ptr *= 2;
		add_to_kdb_buf('\n');
		clear();

	}
	return PASS;
}

/* RTC open test
 *
 * Tests open
 * Inputs: none
 * Outputs: PASS
 * Side Effects: Prints to screen
 * Files: rtc.c/h, keyboard.c/h
 */
int rtc_open_test(){
	TEST_HEADER;
	int32_t i, f = 2;
	int32_t *f_ptr = &f;
	while(f <= 1024){
		if(f == 1024){
			printf("rtc open called\n");
			rtc_open(0);
			for(i = 0; i < 10; i++){
				rtc_read(0,0,0);
				add_to_kdb_buf('1');
			}
			break;
		}
		rtc_write(0, f_ptr, 4);
		for(i = 0; i < 10; i++){
			rtc_read(0,0,0);
			add_to_kdb_buf('1');
		}
		*f_ptr *= 2;
		add_to_kdb_buf('\n');
		clear();

	}
	return PASS;
}

/* read_data_from_file Test
 *
 * Prints the contents of the file
 * Inputs: starting addr of filesystem, name of file
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Files: filesystem.h/c
 */

int read_data_from_file(uint32_t start_addr, uint8_t * filename) {
	clear();
	int i;
	uint8_t buf[1000000]; 

	file_open(filename); //call file_open to retrieve necessary file info
	int n_bytes_read = file_read(0, buf, 1000000); //write the file contents into the buffer
	printf("nbytesread: %d\n", n_bytes_read);

	for(i = 0; i < n_bytes_read; i++) {
		putc(buf[i]);
	}

	return PASS;
}

/* Terminal R/W Test
 *
 * Will wait on a newline to be pressed and will write to screen
 * Inputs: none
 * Outputs: PASS, although will never reach there
 * Side Effects: Writes to screen
 * Files: terminal.c, terminal.h, lib.c, lib.h, keyboard.c, keyboard.h
 */
int terminal_test(){
	TEST_HEADER;
	char buf[128];
	int32_t bytes = 0;
	while(1){	
		bytes = terminal_read(0,buf,128);
		printf("terminal write: ");
		terminal_write(0,buf,bytes);
	}
	return PASS;
}
/* Terminal nbytes
 *
 * Tests invalid number of bytes sent to terminal driver
 * Inputs: none
 * Outputs: PASS
 * Side Effects: Writes to screen valid number of chars
 * Files: terminal.c, terminal.h, lib.c, lib.h, keyboard.c, keyboard.h
 */
int nbytes_test(){
	TEST_HEADER;
	char buf[128];
	int32_t i;
	
	for(i = 0; i < 240; i++){
		add_to_kdb_buf('1');
	}
	//add_to_kdb_buf('\n');
	
	terminal_read(0,buf,240);
	printf("terminal write: ");
	terminal_write(0,buf,240);
		
	
	return PASS;
}
/* Checkpoint 3 tests */

int execute_test(){
	return PASS;
}


/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests(uint32_t input_start_addr){
	uint32_t start_addr = input_start_addr;
	//TEST_OUTPUT("Read data from files", read_data_from_file(start_addr, (uint8_t *)"verylargetextwithverylongname.tx"))
	//TEST_OUTPUT("List Files", list_files(start_addr));
	TEST_OUTPUT("Read Dir Test", read_dir_test(start_addr));
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("Dereference NULL test", exception_test());
	//TEST_OUTPUT("divide-by-zero test", divide_test());
	//TEST_OUTPUT("system call test 2", system_call_2());
	//TEST_OUTPUT("paging test 1", paging_oob());
	//TEST_OUTPUT("paging test 2", paging_ib());
	//TEST_OUTPUT("paging test 1", paging_oob());	
	//TEST_OUTPUT("paging test 2", paging_ib());	
	//TEST_OUTPUT("rtc test", rtc_test());
	//TEST_OUTPUT("rtc open test", rtc_open_test());
	//TEST_OUTPUT("terminal r/w", terminal_test());
	//TEST_OUTPUT("Terminal overflow", nbytes_test());
	// launch your tests here
}
