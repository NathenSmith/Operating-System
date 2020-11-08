#include "execute.h"
#include "x86_desc.h"



// void execute(uint8_t * str) {
//     //the init_task does not take up any memory, it is a kernel thread. It instead inherits the
//     //memory from the last user process.

//     //initialize parent pcb location
//     parent_pcb = (PCB_t *)(START_OF_KERNEL_STACKS - (curr_process_id - 1)*SIZE_OF_KERNEL_STACK);
//     parent_pcb->currArg = parent_pcb; //initializes currArg array to be at start of pcb.
//     parent_pcb->process_id = curr_process_id;

//     parseString(str);
//     checkIfExecutable(parent_pcb->currArg);
//     switch_task_memory();
//     load_program_into_memory(task_name);
//     create_pcb_child();
//     prepare_context_switch();
//     push_iret_context();
// }

void parseString(uint8_t * str) {
    int i = 0;
    int j = 0;
    while(str[i] == ' '){
            i++;
    }
    while(str[i] != ' ') {
        task_name[i] = str[i];
        i++;
    }
    while(str[i] == ' '){
            i++;
    }
    while(str[i + j] != '\0') {
        parent_pcb->currArg[j] = str[i + j];
        j++;
    }
}

void checkIfExecutable(uint8_t * str) {
    uint8_t buf[4];
    file_open(str);
    if(file_read(0, buf, 4) == -1) {
        return -1;
    }
    if(strncmp(buf[1], "ELF", 3) != 0) { //not exectuable file
        return -1;        
    }
    else {
        return 0;
    }
}

//set up paging
void switch_task_memory() {
    uint32_t task_memory = TASK_VIRTUAL_LOCATION; // task memory is a 4 MB page, 128MB in virtual memory
    pageDirectory[parent_pcb->process_id + 1] = task_memory | 0x83; //for pid = 2(first task after init_task), page directory will be at 2*4MB = 8MB   
    //Flush TLB every time page directory is switched.
    flush_tlb();
}

void load_program_into_memory(uint8_t * filename) {
    //take file data and directly put into memory location
    uint32_t task_ptr = TASK_VIRTUAL_LOCATION + (parent_pcb->process_id - 2)*MEMORY_SIZE_PROCESS;
    file_open(filename);
    file_read(0, task_ptr, REALLY_LARGE_NUMBER); //nbytes is a really large number because we want to read the whole file.
}

void create_pcb_child() {
    child_pcb = (PCB_t *)(START_OF_KERNEL_STACKS - (parent_pcb->process_id)*SIZE_OF_KERNEL_STACK);
    child_pcb->currArg = child_pcb;
    child_pcb->parentPtr = (PCB_t *)(START_OF_KERNEL_STACKS - (parent_pcb->process_id - 1)*SIZE_OF_KERNEL_STACK);
    child_pcb->process_id = parent_pcb->process_id + 1;
    //initialize fda members
    int i;
    for(i = 0; i < 8; i++){
        child_pcb->file_arr[i].file_op_ptr = 0;
        child_pcb->file_arr[i].inode_num = 0;
        child_pcb->file_arr[i].file_pos = 0;
        child_pcb->file_arr[i].flags = 0;
    }
}

void prepare_context_switch() {
    //set SS0 and ESP0 in TSS 
    tss.ss0 = KERNEL_DS;
    tss.esp0 = START_OF_KERNEL_STACKS - (parent_pcb->process_id)*SIZE_OF_KERNEL_STACK; 
    curr_process_id++; //increment current process_id
}

void push_iret_context() {
    //set EIP(bytes 24-27 of executable loaded)
    uint32_t * eip_addr = (uint32_t *)(TASK_VIRTUAL_LOCATION + (parent_pcb->process_id - 2)*MEMORY_SIZE_PROCESS + 24);
    uint32_t eip = *eip_addr;
    uint32_t cs = USER_CS;
    //set ESP for user stack to bottom of 4MB page holding executable image
    uint32_t esp = (uint32_t *)(TASK_VIRTUAL_LOCATION + (parent_pcb->process_id - 1)*MEMORY_SIZE_PROCESS);
    uint32_t ss = USER_DS;
    push_IRET_context(eip, cs, esp, ss);
}
