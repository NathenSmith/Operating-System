#include "lib.h"
#include "filesystem.h"
#include "paging.h"
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

typedef struct PCB {
    uint8_t * currArg;
    uint32_t parentPtr;
    uint32_t process_id; //process_id is 1 for init_task
    file_entry_t file_arr[8];
} PCB_t;

typedef struct file_entry {
    uint32_t * file_op_ptr;
    uint32_t inode_num;
    uint32_t file_pos;
    uint32_t flags;
} file_entry_t;

uint32_t curr_process_id = 1;
PCB_t * parent_pcb; 
PCB_t * child_pcb; //current_pcb
uint8_t task_name[MAX_ARG_SIZE];

//function prototypes
void parseString(uint8_t * str);
void checkIfExecutable(uint8_t * str);
void switch_task_memory();
void load_program_into_memory(uint8_t * filename);
void create_pcb_child();
