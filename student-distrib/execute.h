#ifndef _EXECUTE_H
#define _EXECUTE_H
#include "lib.h"

#define TASK_VIRTUAL_LOCATION 0x8000000 //128 MB
#define SIZE_OF_KERNEL_STACK 0x2000 //8 KB
#define START_OF_KERNEL_STACKS 0x800000 //8 MB
#define MEMORY_SIZE_PROCESS 0x400000 //4MB
#define MAX_ARG_SIZE 128
#define REALLY_LARGE_NUMBER 0x10000000
#define FDA_END 8 
#define FDA_START 2

extern void flush_tlb();
extern void push_iret_context_asm();
extern void go_to_exec();

//function prototypes
void parseString(const uint8_t * str);
uint32_t checkIfExecutable(uint8_t * str);
void switch_task_memory();
void load_program_into_memory(const uint8_t * filename);
void create_pcb_child();
void prepare_context_switch();
void push_iret_context();
int32_t execute_steps(const uint8_t* command);
#endif
