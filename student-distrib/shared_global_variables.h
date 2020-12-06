#ifndef _SHARED_GLOBAL_VARIABLE_H
#define _SHARED_GLOBAL_VARIABLE_H

#define MAX_ARG_SIZE 128
#define VID_MEM_PAGE 0x1000
#define ESP2_LOCATION 16
#define EBP2_LOCATION 20
#define SCHEDULE_ENABLE 1

int EXCEPTION;
uint8_t curr_arg[MAX_ARG_SIZE];
uint8_t task_name[MAX_ARG_SIZE];
uint32_t argSize;

typedef struct func_ptrs
{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
}  func_ptrs_t;

typedef struct file_entry {
    func_ptrs_t * file_op_ptr;
    uint32_t inode_num;
    uint32_t file_pos;
    uint32_t flags;
} file_entry_t;

typedef struct PCB {
    uint32_t parentPtr;
    uint32_t process_id; //process_id is 1 for shell
    uint32_t esp;
    uint32_t ebp;
    uint32_t esp2;
    uint32_t ebp2;
    uint32_t screen_x;
    uint32_t screen_y;
    uint8_t filename[128];
    file_entry_t file_arr[8];
} PCB_t;

extern PCB_t * curr_pcb;
extern uint8_t task_name[MAX_ARG_SIZE];
extern uint32_t entry_point;
extern PCB_t * active_processes[3];
extern int visible_terminal;
extern int scheduled_terminal;
extern int nProcesses[3];
extern int total_processes;
extern int terminal_write_flag[3];
extern int paging_scheme;
extern volatile int entered_flag[3];


#endif
