#include "paging.h"
#include "x86_desc.h"
#include "shared_global_variables.h"
#include "system_calls.h"
#include "execute.h"
#define PCB_SIZE_B4_ARG 144

PCB_t * curr_pcb;
uint32_t curr_process_id = 1;
uint8_t task_name[MAX_ARG_SIZE];

int32_t execute_steps(const uint8_t* command) {
    //the init_task does not take up any memory, it is a kernel thread. It instead inherits the
    //memory from the last user process.
    
    curr_process_id++;

    curr_pcb = (PCB_t *)(START_OF_KERNEL_STACKS - (curr_process_id - 1)*SIZE_OF_KERNEL_STACK);
    //curr_pcb->currArg = (uint8_t *)(curr_pcb + PCB_SIZE_B4_ARG);
    //set up stdin, stdout
    curr_pcb->file_arr[0].flags = 1;
    curr_pcb->file_arr[0].inode_num = 0;
    curr_pcb->file_arr[1].flags = 1;
    curr_pcb->file_arr[1].inode_num = 0;
   // printf("\nBEFORE CHECK");
    parseString(command);
    
    if(checkIfExecutable(task_name) == -1) return -1;
    printf("\nBEFORE SWITCHTASK");
    switch_task_memory();
    printf("\nBEFORE LOADPROGRAM");
    load_program_into_memory(task_name);
    printf("\nBEFORE CREATEPCB");
    create_pcb_child();
    printf("\nBEFORE CONTEXTSWITCH");
    prepare_context_switch();
    printf("\nBEFORE IRET");
    push_iret_context();
        
    // asm(
    // "go_to_exec:"    
    // );
    return 0;
}

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
    strcpy(task_name, str);
    printf("TASKNAME: %s\n", task_name);
    //printf("CURR_ARG: \n");
}

uint32_t checkIfExecutable(uint8_t * str) {
    uint8_t buf[4];
    int fd = open(str);
    printf("2AFTER_OPEN2\n");
    printf("FD: %d", fd);
    if(read(fd, buf, 4) == -1) {
        return -1;
    }

    if(strncmp((int8_t *)buf + 1, "ELF", 3) != 0) { //not exectuable file
        return -1;        
    }
    return 0;
    
}

//set up paging
void switch_task_memory() {
    
    uint32_t task_memory = START_OF_KERNEL_STACKS + (curr_process_id - 2) * MEMORY_SIZE_PROCESS; // task memory is a 4 MB page, 128MB in virtual memory
    pageDirectory[VIRTUAL_START] = task_memory | 0x83; //for pid = 2(first task after init_task), page directory will be at 2*4MB = 8MB   
    //Flush TLB every time page directory is switched.
    flush_tlb();
}

void load_program_into_memory(const uint8_t * filename) {
    
    //take file data and directly put into memory location
    uint8_t * task_ptr = (uint8_t *)(TASK_VIRTUAL_LOCATION + (curr_process_id - 2)*MEMORY_SIZE_PROCESS); //upper bound needed
    int32_t fd = open(filename);
    read(fd, task_ptr, REALLY_LARGE_NUMBER); //nbytes is a really large number because we want to read the whole file.
}

void create_pcb_child() {
    if(curr_process_id == 2){
        curr_pcb->parentPtr = NULL;
    } else {
        curr_pcb->parentPtr = START_OF_KERNEL_STACKS - (curr_process_id - 2)*SIZE_OF_KERNEL_STACK;
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
    //printf("before iret asm\n");
    push_iret_context_asm(eip, cs, esp, ss);
}
