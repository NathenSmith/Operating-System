#include "paging.h"
#include "x86_desc.h"
#include "shared_global_variables.h"
#include "system_calls.h"
#include "execute.h"
#define PCB_SIZE_B4_ARG 144

PCB_t * curr_pcb;
uint32_t curr_process_id = 1;
uint8_t task_name[MAX_ARG_SIZE];
uint32_t entry_point;

/* execute_steps
 *  DESCRIPTION: Calls each of execute's 7 steps and switches the program to 
    the child process in user space.
 *  INPUTS: 
        const uint8_t* command - the file to execute and arguments if there are any.
 *  OUTPUTS: None
 *  RETURN VALUE: returns -1 if the program doesn't exist or the file isn't an executable,
    256 if the program dies because of an exception, and a value between 0 and 255 if 
    halt is called.
 *  SIDE EFFECTS: Passes control of the program to the child process, switches to 
    the child process's user space.
 */

int32_t execute_steps(const uint8_t* command) {
    //todo: make init task inherit memory from the last user process
    if(curr_process_id >= MAX_NUMBER_OF_PAGES) {
        return -1;
    }
    curr_process_id++;
    curr_pcb = (PCB_t *)(START_OF_KERNEL_STACKS - (curr_process_id - 1)*SIZE_OF_KERNEL_STACK);

    //set up stdin, stdout
    curr_pcb->file_arr[0].flags = 1;
    curr_pcb->file_arr[0].inode_num = 0;
    curr_pcb->file_arr[1].flags = 1;
    curr_pcb->file_arr[1].inode_num = 0;

    //call execute's 7 steps
    parseString(command);
    if(checkIfExecutable(task_name) == -1) return -1;
    switch_task_memory();
    load_program_into_memory(task_name);
    create_pcb_child();
    prepare_context_switch();
    push_iret_context();
        
    return 0;
}


/* parseString
 *  DESCRIPTION: Parses the task name and argument from the command string.
 *  INPUTS: 
        const uint8_t* str - the command string to parse.
 *  OUTPUTS: None
 *  RETURN VALUE: None
 *  SIDE EFFECTS: None
 */
void parseString(const uint8_t * str) {
    // printf("str: %s", str);
    // int i = 0;
    // int j = 0;
    // while(str[i] == ' '){
    //         i++;
    // }
    // while(str[i] != ' ') {
    //     task_name[i] = str[i];
    //     i++;
    // }
    // printf("TASKNAME: %s\n", task_name);
    // while(str[i] == ' '){
    //         i++;
    // } //counting remaining spaces
    // while(str[i + j] != '\0') {
    //     curr_pcb->currArg[j] = str[i + j];
    //     j++;
    // }
    strcpy((int8_t *)task_name, (int8_t *)str);
    //printf("CURR_ARG: \n");
}

/* checkIfExecutable
 *  DESCRIPTION: Checks whether or not the given file is an executable.
 *  INPUTS: 
        const uint8_t* str - the name of the file.
 *  OUTPUTS: None
 *  RETURN VALUE: Returns 0 if the file is an executable, and -1 otherwise.
 *  SIDE EFFECTS: Opens and reads the contents of the file.
 */
uint32_t checkIfExecutable(uint8_t * str) {
    uint8_t buf[ELF_SIZE];
    int fd = open(str);
    if(read(fd, buf, 4) == -1) {
        return -1;
        close(fd);
    }
    close(fd);
    if(strncmp((int8_t *)buf + 1, "ELF", 3) != 0) { //not exectuable file
        return -1;        
    }
    return 0;
}

/* switch_task_memory
 *  DESCRIPTION: Switches the paging scheme so that the virtual memory at 
    128 MB points to the location for the corresponding user program.
 *  INPUTS: None
 *  OUTPUTS: None
 *  RETURN VALUE: None
 *  SIDE EFFECTS: Flushes the TLB.
 */
 void switch_task_memory() {
    //Flush TLB every time page directory is switched.
    flush_tlb();
    uint32_t task_memory = START_OF_KERNEL_STACKS + (curr_process_id - 2) * MEMORY_SIZE_PROCESS; // task memory is a 4 MB page, 128MB in virtual memory
    pageDirectory[VIRTUAL_START] = task_memory | PAGING_FLAGS; //for pid = 2(first task after init_task), page directory will be at 2*4MB = 8MB   
}

/* load_program_into_memory
 *  DESCRIPTION: Loads the specified executable at the correct offset in virtual memory.
 *  INPUTS: 
        const uint8_t * filename - the name of the executable to load into memory.
 *  OUTPUTS: None
 *  RETURN VALUE: None
 *  SIDE EFFECTS: None
 */
void load_program_into_memory(const uint8_t * filename) {
    //make note of the entry point(contained in bytes 24-27)
    uint32_t buf[BUFFER_LENGTH];
    int fd = open(filename);
    read(fd, buf, FIRST_FEW_BYTES_SIZE);
    entry_point = buf[BUFFER_INDEX];
    close(fd);

    //copy the entire file to memory starting at virtual address 0x08048000
    uint8_t * task_ptr = (uint8_t *)START_OF_USER_PROGRAM; 
    fd = open(filename); //assuming initial position is reset to 0
    read(fd, task_ptr, REALLY_LARGE_NUMBER); //nbytes is a really large number because we want to read the whole file.
    close(fd);
}

/* create_pcb_child
 *  DESCRIPTION: Initializes the PCB for the child process.
 *  INPUTS: None
 *  OUTPUTS: None
 *  RETURN VALUE: None
 *  SIDE EFFECTS: None
 */
void create_pcb_child() {
    if(curr_process_id == SHELL_PID){
        curr_pcb->parentPtr = NULL;
    } else {
        curr_pcb->parentPtr = START_OF_KERNEL_STACKS - (curr_process_id - SHELL_PID)*SIZE_OF_KERNEL_STACK;
    }
    curr_pcb->process_id = curr_process_id;
    //initialize fda members
    int i;
    for(i = FDA_START; i < FDA_END; i++){
        curr_pcb->file_arr[i].file_op_ptr = 0;
        curr_pcb->file_arr[i].inode_num = 0;
        curr_pcb->file_arr[i].file_pos = 0;
        curr_pcb->file_arr[i].flags = 0;
    }

    //save esp and ebp for future use
    asm volatile (
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        :"=r"(curr_pcb->esp), "=r"(curr_pcb->ebp)
    );
}

/* prepare_context_switch
 *  DESCRIPTION: Sets the correct values in the tss, allowing for system calls.
 *  INPUTS: None
 *  OUTPUTS: None
 *  RETURN VALUE: None
 *  SIDE EFFECTS: None
 */
void prepare_context_switch() {
    //set SS0 and ESP0 in TSS 
    tss.ss0 = KERNEL_DS;
    tss.esp0 = START_OF_KERNEL_STACKS - (curr_process_id - SHELL_PID)*SIZE_OF_KERNEL_STACK - 4; 
}

/* push_iret_context
 *  DESCRIPTION: pushes the IRET context and calls IRET, transferring control to the
    child process and switching to its user space.
 *  INPUTS: None
 *  OUTPUTS: None
 *  RETURN VALUE: None
 *  SIDE EFFECTS: None
 */
void push_iret_context() {
    //set EIP(bytes 24-27 of executable loaded)
    uint32_t eip = entry_point;
    uint32_t cs = USER_CS;
    //set ESP for user stack to bottom of 4MB page holding executable image
    //+1 is for bottom of the page
    uint32_t esp = TASK_VIRTUAL_LOCATION + MEMORY_SIZE_PROCESS - 4;
    uint32_t ss = USER_DS;
    push_iret_context_asm(eip, cs, esp, ss);
}
