#include "execute.h"
#include "pcb.h"

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
        curr_pcb->currArg[j] = str[i + j];
        j++;
    }
}

uint32_t checkIfExecutable(uint8_t * str) {
    uint8_t buf[4];
    file_open(str);
    if(file_read(0, buf, 4) == -1) {
        return -1;
    }
    if(strncmp(buf[1], "ELF", 3) != 0) { //not exectuable file
        return -1;        
    }
    return 0;
    
}

//set up paging
void switch_task_memory() {
    uint32_t task_memory = TASK_VIRTUAL_LOCATION; // task memory is a 4 MB page, 128MB in virtual memory
    pageDirectory[curr_process_id] = task_memory | 0x83; //for pid = 2(first task after init_task), page directory will be at 2*4MB = 8MB   
    //Flush TLB every time page directory is switched.
    flush_tlb();
}

void load_program_into_memory(uint8_t * filename) {
    //take file data and directly put into memory location
    uint32_t task_ptr = TASK_VIRTUAL_LOCATION + (curr_process_id - 2)*MEMORY_SIZE_PROCESS;
    file_open(filename);
    file_read(0, task_ptr, REALLY_LARGE_NUMBER); //nbytes is a really large number because we want to read the whole file.
}

void create_pcb_child() {
    if(curr_process_id == 2){
        curr_pcb->parentPtr = NULL;
    }else{
        curr_pcb->parentPtr = (PCB_t *)(START_OF_KERNEL_STACKS - (curr_process_id - 2)*SIZE_OF_KERNEL_STACK);
    }
    curr_pcb->process_id = curr_process_id;
    //initialize fda members
    int i;
    for(i = 0; i < 8; i++){
        curr_pcb->file_arr[i].file_op_ptr = 0;
        curr_pcb->file_arr[i].inode_num = 0;
        curr_pcb->file_arr[i].file_pos = 0;
        curr_pcb->file_arr[i].flags = 0;
    }

    asm volatile (
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        :"=r"(curr_pcb->esp), "=r"(curr_pcb->ebp)
    );
}

void prepare_context_switch() {
    //set SS0 and ESP0 in TSS 
    tss.ss0 = KERNEL_DS;
    tss.esp0 = START_OF_KERNEL_STACKS - (curr_process_id - 2)*SIZE_OF_KERNEL_STACK; 
}

void push_iret_context() {
    //set EIP(bytes 24-27 of executable loaded)
    uint32_t * eip_addr = (uint32_t *)(TASK_VIRTUAL_LOCATION + (curr_process_id - 2)*MEMORY_SIZE_PROCESS + 24);
    uint32_t eip = *eip_addr;
    uint32_t cs = USER_CS;
    //set ESP for user stack to bottom of 4MB page holding executable image
    uint32_t esp = (uint32_t)(TASK_VIRTUAL_LOCATION + (curr_process_id - 1)*MEMORY_SIZE_PROCESS);
    uint32_t ss = USER_DS;
    push_IRET_context(eip, cs, esp, ss);
}
