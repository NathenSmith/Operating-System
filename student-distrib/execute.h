#include "lib.h"
#include "filesystem.h"
#include "paging.h"

typedef struct PCB {
    uint8_t currArg[128]; //double check
    uint32_t n_tasks_executed;
    uint32_t parentPtr;
    uint32_t process_id;
    file_arr_t file_arr;
} PCB_t;

typedef struct file_array {
    file_entry_t file_entry[8];
} file_arr_t;

typedef struct file_entry {
    uint32_t file_op_ptr;
    uint32_t inode_ptr;
    uint32_t file_pos;
    uint32_t flags;
} file_entry_t;

//function prototypes
void parseString(uint8_t * str);
void checkIfExecutable(uint8_t * str);
void switch_task_memory();
void load_program_into_memory(uint8_t * filename);
void create_pcb_child();