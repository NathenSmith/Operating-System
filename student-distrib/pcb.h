// typedef struct
// {
//     int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
//     int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
//     int32_t (*open)(const uint8_t* filename);
//     int32_t (*close)(int32_t fd);
// }  func_ptrs;

// typedef struct file_entry {
//     func_ptrs * file_op_ptr;
//     uint32_t inode_num;
//     uint32_t file_pos;
//     uint32_t flags;
// } file_entry_t;

// typedef struct PCB {
//     uint32_t parentPtr;
//     uint32_t process_id; //process_id is 1 for init_task
//     uint32_t esp;
//     uint32_t ebp;
//     file_entry_t file_arr[8];
//     uint8_t * currArg;
// } PCB_t;

// PCB_t * curr_pcb;
// uint32_t curr_process_id = 1;
