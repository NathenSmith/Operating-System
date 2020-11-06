#include "lib.h"
#include "filesystem.h"
#include "paging.h"

PCB_t pcb_orig;
PCB_t * pcb = &pcb_orig;

typedef struct PCB {
    uint8_t * currArg;
    uint32_t n_tasks_executed;
    uint32_t parentPtr;
    uint32_t process_id;
    file_entry_t file_desc_array[8];
} PCB_t;

typedef struct file_entry {
    uint32_t *file_op_ptr;
    uint32_t inode_ptr;
    uint32_t file_pos;
    uint32_t flags;
} file_entry_t;

//function prototypes
void parseString(uint8_t * str);
void checkIfExecutable(uint8_t * str);
