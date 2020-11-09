#include "execute.h"
#include "filesystem.h"
#include "rtc.h"
#include "terminal.h"
#include "system_calls.h"
#include "paging.h"

//must declare globally or else stack will fill up everytime open is called
//static func_ptrs terminal_ptr = {terminal_read, terminal_write, terminal_open, terminal_close};
static func_ptrs_t rtc_ptr = {rtc_read, rtc_write, rtc_open, rtc_close};
static func_ptrs_t dir_ptr = {dir_read, dir_write, dir_open, dir_close};
static func_ptrs_t file_ptr = {file_read, file_write, file_open, file_close};

/* halt
 *
 * Implements the halt system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t halt(uint8_t status) {

    // if the current process is shell
    if(curr_process_id != 2){

        curr_process_id--;
        uint32_t task_memory = TASK_VIRTUAL_LOCATION; // task memory is a 4 MB page, 128MB in virtual memory

        // setting new child pcb
        curr_pcb = (PCB_t *)(START_OF_KERNEL_STACKS - (curr_process_id*SIZE_OF_KERNEL_STACK));
        pageDirectory[curr_process_id] = task_memory | 0x83; //for pid = 2(first task after init_task), page directory will be at 2*4MB = 8MB
        // flush TLB every time page directory is switched.
        flush_tlb();

        // close open files using fd
        int i;
        for(i = FDA_START; i < FDA_END; i++){
            close(i);
        }
    }
    /* Load in current process's ebp and esp and save status to eax */
    asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        "movl %2, %%eax;"
        "jmp go_to_exec;"

        :
        :"r"(curr_pcb->esp), "r"(curr_pcb->ebp), "r"((uint32_t) status)
        :"%eax"
    );

    return 0;
}

/* execute
 *
 * Implements the execute system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t execute(const uint8_t* command) {
    return execute_steps(command);
}

/* read
 *
 * Implements the read system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t read(int32_t fd, void * buf, int32_t nbytes) {
    if(fd < 0 || fd >= FDA_END || fd == 1 || !(curr_pcb->file_arr[fd].flags)){ //not in bounds or not open
        return -1;
    }
    //file pos only to be updated in file_read
    //printf("\nFILE OP PTR: %d\n", curr_pcb->file_arr[fd].file_op_ptr);
    return curr_pcb->file_arr[fd].file_op_ptr->read(fd, buf, nbytes); //page faults
}

/* write
 *
 * Implements the write system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t write(int32_t fd, void* buf, int32_t nbytes) {
    if(fd <= 0 || fd >= FDA_END || !(curr_pcb->file_arr[fd].flags)){ //not in bounds or not open
        return -1;
    }
    return curr_pcb->file_arr[fd].file_op_ptr->write(fd, buf, nbytes);
}

/* open
 *
 * Implements the open system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t open(const uint8_t* filename) {
    //printf("open");
    //iterate through pcb starting at index 2
    dentry_t file_dentry;
    if(read_dentry_by_name (filename, &file_dentry) == -1) return -1;
    int i;
    for(i = FDA_START; i < FDA_END; i++){
        if(!(curr_pcb->file_arr[i].flags)){
            curr_pcb->file_arr[i].flags = 1;
            //if statements, go through each type of device
            //make fileop table point to respective table
            switch (file_dentry.filetype)
            {
            case 0: //real-time clock
                curr_pcb->file_arr[i].inode_num = 0; //should be ignored here and directory?
                // rtc_ptrs rtc_ptr = {rtc_read, rtc_write, rtc_open, rtc_close};
                curr_pcb->file_arr[i].file_op_ptr = &rtc_ptr;
                rtc_ptr.open(filename);
                break;
            case 1: //directory
                curr_pcb->file_arr[i].inode_num = 0; //should be ignored here and directory?
                // dir_ptrs dir_ptr = {dir_read, dir_write, dir_open, dir_close};
                curr_pcb->file_arr[i].file_op_ptr = &dir_ptr;
                dir_ptr.open(filename);
                break;
            case 2: //regular file
                curr_pcb->file_arr[i].inode_num = file_dentry.inode_num; //should be ignored here and directory?
                // file_ptrs file_ptr = {file_read, file_write, file_open, file_close};
                curr_pcb->file_arr[i].file_op_ptr = &file_ptr;
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
    if(fd < FDA_START || fd >= FDA_END || !(curr_pcb->file_arr[fd].flags)){ //not in bounds or not open
        return -1;
    }
    curr_pcb->file_arr[fd].flags = 0;
    curr_pcb->file_arr[fd].inode_num = 0; 
    curr_pcb->file_arr[fd].file_pos = 0;
    curr_pcb->file_arr[fd].file_op_ptr = 0;
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
