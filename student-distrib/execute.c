#include "paging.h"
#include "x86_desc.h"
#include "system_calls.h"
#include "execute.h"
#include "filesystem.h"
#define PCB_SIZE_B4_ARG 144

PCB_t * curr_pcb;
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


/* parseString
 *  DESCRIPTION: Parses the task name and argument from the command string.
 *  INPUTS: 
        const uint8_t* str - the command string to parse.
 *  OUTPUTS: None
 *  RETURN VALUE: None
 *  SIDE EFFECTS: None
 */
void parseString(const uint8_t * str) {
    //printf("str: %s", str);
    int i = 0;
    int j = 0;
    int k = 0;
    while(str[i] == ' '){
            i++;
    }
    while(str[i] != ' ' && str[i] != '\0') {
        task_name[k] = str[i];
        i++;
        k++;
    }
    if(str[i] == '\0'){
       task_name[k] = str[i];
       return; 
    }
    //printf("TASKNAME: %s\n", task_name);
    while(str[i] == ' '){
            i++;
    } //counting remaining spaces
    while(str[i + j] != '\0') {
        curr_arg[j] = str[i + j];
        j++;
    }
    argSize = j;
    // printf("curr arg %s \n", curr_arg);
    // printf("length: %d\n", strlen(curr_arg));
    // printf("arg size: %d", curr_pcb->argSize);
    //strcpy((int8_t *)task_name, (int8_t *)str);
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
    dentry_t dentry;
    read_dentry_by_name (str, &dentry);
    uint32_t inode_number = dentry.inode_num;
    if(read_data (inode_number, 0, buf, ELF_SIZE) == -1) {
        return -1;
    }
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
    uint32_t task_memory = START_OF_KERNEL_STACKS + (curr_pcb->process_id-1) * MEMORY_SIZE_PROCESS; // task memory is a 4 MB page, 128MB in virtual memory
    pageDirectory[VIRTUAL_START] = task_memory | PAGING_FLAGS; //for pid = 1, page directory will be at 8MB   
    flush_tlb(); //Flush TLB every time page directory is switched.
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
    dentry_t dentry;
    uint32_t inode_number;

    //make note of the entry point(contained in bytes 24-27)
    read_dentry_by_name (filename, &dentry);
    inode_number = dentry.inode_num;
    read_data (inode_number, EIP_START, (uint8_t *)(&entry_point), EIP_SIZE); 

    //copy the entire file to memory starting at virtual address 0x08048000
    uint8_t * task_ptr = (uint8_t *)START_OF_USER_PROGRAM; 
    read_dentry_by_name (filename, &dentry);
    inode_number = dentry.inode_num;
    read_data (inode_number, 0, task_ptr, REALLY_LARGE_NUMBER);
}

/* create_pcb_child
 *  DESCRIPTION: Initializes the PCB for the child process.
 *  INPUTS: None
 *  OUTPUTS: None
 *  RETURN VALUE: None
 *  SIDE EFFECTS: None
 */
void create_pcb_child() {
    if(curr_pcb->process_id == SHELL_PID){
        curr_pcb->parentPtr = NULL;
    } else {
        curr_pcb->parentPtr = START_OF_KERNEL_STACKS - (curr_pcb->process_id - SHELL_PID)*SIZE_OF_KERNEL_STACK;
    }
    //initialize fda members
    int i;
    for(i = FDA_START; i < FDA_END; i++){
        curr_pcb->file_arr[i].file_op_ptr = 0;
        curr_pcb->file_arr[i].inode_num = 0;
        curr_pcb->file_arr[i].file_pos = 0;
        curr_pcb->file_arr[i].flags = 0;
    }

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
    tss.esp0 = START_OF_KERNEL_STACKS - (curr_pcb->process_id - SHELL_PID)*SIZE_OF_KERNEL_STACK - 4; 
}

/* push_iret_context
 *  DESCRIPTION: pushes the IRET context and calls IRET, transferring control to the
    child process and switching to its user space.
 *  INPUTS: None
 *  OUTPUTS: None
 *  RETURN VALUE: None
 *  SIDE EFFECTS: None
 */
void push_iret_context(uint32_t eip_val) {
    //set EIP(bytes 24-27 of executable loaded)
    uint32_t eip = eip_val;
    uint32_t cs = USER_CS;
    //set ESP for user stack to bottom of 4MB page holding executable image
    uint32_t esp = TASK_VIRTUAL_LOCATION + MEMORY_SIZE_PROCESS - 4; //should be 0x083FFFFC
    uint32_t ss = USER_DS;    

    push_iret_context_test(curr_pcb->parentPtr + ESP_LOCATION, curr_pcb->parentPtr + EBP_LOCATION, eip, cs, esp, ss);    
}

