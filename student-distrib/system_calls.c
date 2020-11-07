#include "system_calls.h"
#include "lib.h"
#include "execute.h"
#include "filesystem.h"
#include "rtc.h"

typedef struct
{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const int32_t* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
}  rtc_ptrs;

typedef struct
{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const int32_t* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
} file_ptrs;

typedef struct
{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const int32_t* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
} dir_ptrs;

typedef struct
{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const int32_t* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
} terminal_ptrs;

/* halt
 *
 * Implements the halt system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t halt(uint8_t status) {

    // if the current process is shell
    if(child_ptr->process_id == 2) return -1;

    uint32_t task_memory = TASK_VIRTUAL_LOCATION; // task memory is a 4 MB page, 128MB in virtual memory

    // setting new child and parent pcbs
    child_pcb = (PCB_t *)(START_OF_KERNEL_STACKS - (parent_pcb->process_id)*SIZE_OF_KERNEL_STACK);
    parent_pcb = (PCB_t *)(START_OF_KERNEL_STACKS - (parent_pcb->process_id - 1)*SIZE_OF_KERNEL_STACK);

    curr_process_id = child_pcb -> process_id;

    pageDirectory[parent_pcb->process_id] = task_memory | 0x83; //for pid = 2(first task after init_task), page directory will be at 2*4MB = 8MB
    // flush TLB every time page directory is switched.
    flush_tlb();

    // close open files using fd
    int i;
    for(i = 2; i <=7; i++){
        close(i);
    }

    uint_32 child_esp = START_OF_KERNEL_STACKS - (child_pcb->process_id)*SIZE_OF_KERNEL_STACK;
    uint_32 child_ebp = START_OF_KERNEL_STACKS - (child_pcb->process_id - 1)*SIZE_OF_KERNEL_STACK;

    /* Load in parent process's ebp and esp and save status to eax */
    asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        "movl %2, %%eax;"
        "jmp go_to_exec;"

        :
        :"r"(child_esp), "r"(child_ebp), "r"((uint32_t) status)
        :"%eax"
    );

    return 0;

    // printf("halt");
    // while(1) {}
}

/* execute
 *
 * Implements the execute system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t execute(const uint8_t* command) {
    //the init_task does not take up any memory, it is a kernel thread. It instead inherits the
    //memory from the last user process.

    //initialize parent pcb location

    parent_pcb = (PCB_t *)(START_OF_KERNEL_STACKS - (curr_process_id - 1)*SIZE_OF_KERNEL_STACK);
    parent_pcb->currArg = parent_pcb; //initializes currArg array to be at start of pcb.
    parent_pcb->process_id = curr_process_id;

    parseString(command);
    checkIfExecutable(parent_pcb->currArg);
    switch_task_memory();
    load_program_into_memory(task_name);
    create_pcb_child();
    prepare_context_switch();
    push_iret_context();
    // printf("execute");
    // while(1) {}

    asm volatile("go_to_exec:");
    return 0;
}

/* read
 *
 * Implements the read system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t read(int32_t fd,void* buf, int32_t nbytes) {
    if(fd < 0 || fd >= 8 || fd == 1 || !(file_desc_array[fd].flag)){ //not in bounds or not open
        return -1;
    }
    return pcb->file_desc_array[fd].file_op_ptr->read(fd, buf, nbytes);
}

/* write
 *
 * Implements the write system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t write(int32_t fd, void* buf, int32_t nbytes) {
    if(fd <= 0 || fd >= 8 || !(file_desc_array[fd].flag)){ //not in bounds or not open
        return -1;
    }
    return pcb->file_desc_array[fd].file_op_ptr->write(fd, buf, nbytes);
}

/* open
 *
 * Implements the open system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t open(const uint8_t* filename) {
    printf("open");
    //iterate through pcb starting at index 2
    dentry_t file_dentry;
    if(read_dentry_by_name (filename, &file_dentry) == -1) return -1;
    int i;

    if(strcmp(filename, (uint8_t* )"stdin") == 0){
        pcb->file_desc_array[0].flag = 1;
        pcb->file_desc_array.inode_num = 0;
        terminal_ptrs terminal_ptr = {terminal_read, terminal_write, terminal_open, terminal_close};
        terminal_ptr.open(filename);
    }else if(strcmp(filename, (uint8_t* )"stdout") == 0){
        pcb->file_desc_array[1].flag = 1;
        pcb->file_desc_array.inode_num = 0;
        terminal_ptrs terminal_ptr = {terminal_read, terminal_write, terminal_open, terminal_close};
        terminal_ptr.open(filename);
    }

    for(i=2; i< 8; i++){
        if(!(pcb->file_desc_array[i].flag)){
            pcb->file_desc_array[i].flag = 1;
            //if statements, go through each type of device
            //make fileop table point to respective table
            switch (file_dentry.filetype)
            {
            case 0: //real-time clock
                pcb->file_desc_array[i].inode_num = 0; //should be ignored here and directory?
                //file pos?
                pcb->file_desc_array[i].file_pos = n_bytes_read_so_far[i];
                rtc_ptrs rtc_ptr = {rtc_read, rtc_write, rtc_open, rtc_close};
                pcb->file_desc_array[i].file_op_ptr = &rtc_ptr;
                rtc_ptr.open(filename);
                break;
            case 1: //directory
                /* code */
                pcb->file_desc_array[i].inode_num = 0; //should be ignored here and directory?
                //file pos?
                pcb->file_desc_array[i].file_pos = n_bytes_read_so_far[i];
                dir_ptrs dir_ptr = {dir_read, dir_write, dir_open, dir_close};
                pcb->file_desc_array[i].file_op_ptr = &dir_ptr;
                dir_ptr.open(filename);
                break;
            case 2: //regular file
                /* code */
                pcb->file_desc_array[i].inode_num = file_dentry.inode_num; //should be ignored here and directory?
                //file pos?
                pcb->file_desc_array[i].file_pos = n_bytes_read_so_far[i];
                file_ptrs file_ptr = {file_read, file_write, file_open, file_close};
                pcb->file_desc_array[i].file_op_ptr = &file_ptr;
                file_ptr.open(filename);
                break;
            default: //should never reach here
                i = -1;
                break;
            }
            return i;
        }

    }
    return -1;
}

/* close
 *
 * Implements the close system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t close(int32_t fd) {
    //error check fd
    if(fd < 2 || fd >= 8 || !(file_desc_array[fd].flag)){ //not in bounds or not open
        return -1;
    }
    pcb->file_desc_array[fd].flag = 0;
    pcb->file_desc_array[fd].inode_num = 0; //should be ignored here and directory?
    //file pos?
    pcb->file_desc_array[fd].file_pos = 0;
    pcb->file_desc_array[i].file_op_ptr = 0;
    return 0;
}

/* getargs
 *
 * Implements the getargs system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t getargs(uint8_t* buf, int32_t nbytes) {
    printf("getargs");
    while(1) {}
}

/* vidmap
 *
 * Implements the vidmap system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t vidmap(uint8_t** screen_start) {
    printf("vidmap");
    while(1) {}
}

/* set_handler
 *
 * Implements the set_handler system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void set_handler() {
    printf("set_handler");
    while(1) {}
}

/* sigreturn
 *
 * Implements the sigreturn system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void sigreturn() {
    printf("sigreturn");
    while(1) {}
}
