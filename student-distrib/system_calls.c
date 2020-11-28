#include "execute.h"
#include "filesystem.h"
#include "rtc.h"
#include "terminal.h"
#include "system_calls.h"
#include "paging.h"
#include "lib.h"

//must declare globally or else stack will fill up everytime open is called
static func_ptrs_t terminal_ptr = {terminal_read, terminal_write, terminal_open, terminal_close};
static func_ptrs_t rtc_ptr = {rtc_read, rtc_write, rtc_open, rtc_close};
static func_ptrs_t dir_ptr = {dir_read, dir_write, dir_open, dir_close};
static func_ptrs_t file_ptr = {file_read, file_write, file_open, file_close};

int nShellsOpen = 0;

/* halt
 *
 * Implements the halt system call.
 * Inputs: status
 * Outputs: None
 * Side Effects: None
 */

int32_t halt(uint8_t status) {
    if(EXCEPTION) return EXCEPTION_NUM;
    // if the current process is shell
    if(nShellsOpen != 1) {
        // close open files using fd
        int i;
        for(i = FDA_START; i < FDA_END; i++){
            close(i);
        }

        curr_pcb = (PCB_t *)curr_pcb->parentPtr;

        switch_task_memory();
        prepare_context_switch();
        restore_parent_data(curr_pcb->esp, curr_pcb->ebp, (uint32_t)status);
    }
    return -1;
}

/* execute
 *
 * Implements the execute system call.
 * Inputs: executes given command following steps
 * Outputs: None
 * Side Effects: None
 */

int32_t execute(const uint8_t* command) {
    EXCEPTION = 0;
    memset(task_name, '\0', MAX_ARG_SIZE);
    memset(curr_arg, '\0', MAX_ARG_SIZE);
    argSize = 0;
    parseString(command);

    if(strncmp((int8_t *)task_name, (int8_t *) "shell", 5) == 0 && nShellsOpen == 0){
        curr_pcb = (PCB_t *)(START_OF_KERNEL_STACKS - SIZE_OF_KERNEL_STACK);
        curr_pcb->process_id = 1;
        nShellsOpen++;
    } else {
        if(strncmp((int8_t *)task_name, (int8_t *) "shell", 5) == 0) {
            if(nShellsOpen == 3) {
                return -1;
            }
            nShellsOpen++;
        }
        uint32_t newProcessId = curr_pcb->process_id + 1;
        if(newProcessId >= MAX_NUMBER_OF_PAGES) return -1;
        curr_pcb = (PCB_t *)(START_OF_KERNEL_STACKS - (newProcessId)*SIZE_OF_KERNEL_STACK);
        curr_pcb->process_id = newProcessId;
    }

    //set up stdin, stdout
    curr_pcb->file_arr[0].flags = 1;
    curr_pcb->file_arr[0].inode_num = 0;
    curr_pcb->file_arr[0].file_op_ptr = &terminal_ptr;
    curr_pcb->file_arr[1].flags = 1;
    curr_pcb->file_arr[1].inode_num = 0;
    curr_pcb->file_arr[1].file_op_ptr = &terminal_ptr;

    //call execute's other 6 steps
    if(checkIfExecutable(task_name) == -1) return -1;
    switch_task_memory();
    load_program_into_memory(task_name);
    create_pcb_child();
    prepare_context_switch();
    push_iret_context(entry_point);
        
    return 0;
}

/* read
 *
 * Implements the read system call, calls filetype's respective read call
 * Inputs: fd - file desc array index, buf-buffer to read into, nbytes- num bytes to read
 * Outputs: None
 * Side Effects: None
 */

int32_t read(int32_t fd, void * buf, int32_t nbytes) {
    if(fd < 0 || fd >= FDA_END || fd == 1 || !(curr_pcb->file_arr[fd].flags)){ //not in bounds or not open
        return -1;
    }
    //file pos only to be updated in file_read
    //printf("\nFILE OP PTR: %d\n", curr_pcb->file_arr[fd].file_op_ptr);
    int retval = curr_pcb->file_arr[fd].file_op_ptr->read(fd, buf, nbytes); 
    return retval;
}

/* write
 *
 * Implements the write system call, calls filetype's respective write call
 * Inputs: fd - file desc array index, buf-buffer to write, nbytes- num bytes to write
 * Outputs: None
 * Side Effects: None
 */

int32_t write(int32_t fd, void* buf, int32_t nbytes) {
    if(fd <= 0 || fd >= FDA_END || !(curr_pcb->file_arr[fd].flags)){ //not in bounds or not open
        return -1;
    }
    int32_t retval = curr_pcb->file_arr[fd].file_op_ptr->write(fd, buf, nbytes);
    return retval;
}

/* open
 *
 * Implements the open system call for every file type.
 * Inputs: filename- checked if valid
 * Outputs: None
 * Side Effects: None
 */

int32_t open(const uint8_t* filename) {
    //printf("open");
    //iterate through pcb starting at index 2
    dentry_t file_dentry;
    if(read_dentry_by_name (filename, &file_dentry) == -1) return -1;
    curr_pcb->filename = filename;
    int i;
    for(i = FDA_START; i < FDA_END; i++){
        if(!(curr_pcb->file_arr[i].flags)){
            curr_pcb->file_arr[i].flags = 1;
            //if statements, go through each type of device
            //make fileop table point to respective table
            switch (file_dentry.filetype)
            {
            case 0: //real-time clock, filetype (0)
                curr_pcb->file_arr[i].file_op_ptr = &rtc_ptr;
                rtc_ptr.open(filename);
                break;
            case 1: //directory, filetype (1)
                curr_pcb->file_arr[i].file_op_ptr = &dir_ptr;
                dir_ptr.open(filename);
                break;
            case 2: //regular file, filetype (2)
                curr_pcb->file_arr[i].inode_num = file_dentry.inode_num; 
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
 * Inputs: fd- file descriptor array index
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
    //clear buffer
    memset(buf, '\0', nbytes);

    if(nbytes == 0 || argSize == 0){
        // printf("curr arg %s \n", curr_arg);
        // printf("arg size: %d", argSize);
        return -1; //no argument
    }
    //printf("nbytes: %d \n", nbytes);
    // printf("curr arg %s \n", curr_arg);
    // printf("arg size: %d", argSize);
    int numBytesToCopy = nbytes;
    if(argSize < nbytes) numBytesToCopy = argSize;
    strncpy((int8_t *)buf, (int8_t *)curr_arg, numBytesToCopy);
    //printf("buf: %s\n", buf);
    return 0;
}

/* vidmap
 *
 * Implements the vidmap system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t vidmap(uint8_t** screen_start) {
    // check if screen start is between 128 and 132 MB
    if(screen_start == NULL) return -1;
    uint32_t address = (uint32_t) screen_start;
    if(address < USER_PAGE_START || address >= USER_PAGE_END) return -1;

    // 33 -> 128 MB in virtual mem
    pageDirectory[33] = *videoMemTable | 0x07; //set user, r/w, present
    videoMemTable[0] = (uint32_t) VIDEO_MEMORY_IDX | 0x07;

    // flush_tlb();
    asm volatile (
        "movl %cr3, %eax;"
        "movl %eax, %cr3;"
    );

    *screen_start = (uint8_t*) USER_PAGE_START;

    return 0;

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

