#ifndef _EXECUTE_H
#define _EXECUTE_H
#include "lib.h"
#include "shared_global_variables.h"

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
#define EIP_START 24
#define EIP_SIZE 4
#define SHELL_PID 1
#define PAGING_FLAGS 0x087
#define ESP_LOCATION 8
#define EBP_LOCATION 12

extern void flush_tlb();
extern void go_to_exec();
extern void push_iret_context_test();
extern void restore_parent_data();
extern void switch_task_memory();
extern void prepare_context_switch();
extern void restore_ebp_esp();

//function prototypes
void parseString(const uint8_t * str);
uint32_t checkIfExecutable(uint8_t * str);
void load_program_into_memory(const uint8_t * filename);
void create_pcb_child();
void push_iret_context();
int32_t execute_steps(const uint8_t* command);
#endif

