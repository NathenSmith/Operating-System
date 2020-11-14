#ifndef _EXECUTE_H
#define _EXECUTE_H
#include "lib.h"


#define TASK_VIRTUAL_LOCATION 0x8000000 //128 MB
#define START_OF_USER_PROGRAM 0x08048000
#define SIZE_OF_KERNEL_STACK 0x2000 //8 KB
#define START_OF_KERNEL_STACKS 0x800000 //8 MB
#define MEMORY_SIZE_PROCESS 0x400000 //4MB
#define REALLY_LARGE_NUMBER 0x10000000
#define FDA_END 8 
#define FDA_START 2
#define VIRTUAL_START 32
#define MAX_NUMBER_OF_PAGES 992
#define ELF_SIZE 4
#define FIRST_FEW_BYTES_SIZE 28
#define BUFFER_LENGTH 7
#define BUFFER_INDEX 6
#define SHELL_PID 1
#define PAGING_FLAGS 0x087

extern void flush_tlb();
extern void go_to_exec();


//function prototypes
void parseString(const uint8_t * str);
uint32_t checkIfExecutable(uint8_t * str);
void switch_task_memory();
void load_program_into_memory(const uint8_t * filename);
void create_pcb_child();
void prepare_context_switch();
int32_t execute_steps(const uint8_t* command);
#endif
