#include "lib.h"
#include "paging.h"
#include "x86_desc.h"

#define TASK_VIRTUAL_LOCATION 0x8000000 //128 MB
#define SIZE_OF_KERNEL_STACK 0x2000 //8 KB
#define START_OF_KERNEL_STACKS 0x800000 //8 MB
#define MEMORY_SIZE_PROCESS 0x400000 //4MB
#define MAX_ARG_SIZE 128
#define REALLY_LARGE_NUMBER 0x10000000
#define FDA_END 8 //not food drug association
#define FDA_START 2

extern void flush_tlb();
extern void push_iret_context();

// PCB_t * parent_pcb; 
// PCB_t * child_pcb;

uint8_t task_name[MAX_ARG_SIZE];

//function prototypes
void parseString(uint8_t * str);
uint32_t checkIfExecutable(uint8_t * str);
void switch_task_memory();
void load_program_into_memory(uint8_t * filename);
void create_pcb_child();
