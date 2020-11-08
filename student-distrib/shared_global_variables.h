#ifndef _SHARED_GLOBAL_VARIABLE_H
#define _SHARED_GLOBAL_VARIABLE_H

typedef struct func_ptrs
{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
}  func_ptrs_t;

typedef struct file_entry {
    func_ptrs * file_op_ptr;
    uint32_t inode_num;
    uint32_t file_pos;
    uint32_t flags;
} file_entry_t;

typedef struct PCB {
    uint32_t parentPtr;
    uint32_t process_id; //process_id is 1 for init_task
    uint32_t esp;
    uint32_t ebp;
    file_entry_t file_arr[8];
    uint8_t * currArg;
} PCB_t;

extern PCB_t * curr_pcb;
extern uint32_t curr_process_id;
#endif
