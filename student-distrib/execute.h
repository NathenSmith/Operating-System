#include "lib.h"
#include "filesystem.h"
#include "paging.h"

extern void flush_tlb();
extern void push_iret_context();

typedef struct PCB {
    uint8_t * currArg;
    uint32_t parentPtr;
    uint32_t process_id; //process_id is 1 for init_task
    file_entry_t file_arr[8];
} PCB_t;

typedef struct file_entry {
    uint32_t file_op_ptr;
    uint32_t inode_num;
    uint32_t file_pos;
    uint32_t flags;
} file_entry_t;

//function prototypes
void parseString(uint8_t * str);
void checkIfExecutable(uint8_t * str);
void switch_task_memory();
void load_program_into_memory(uint8_t * filename);
void create_pcb_child();