#include "execute.h"

#define TASK_VIRTUAL_LOCATION 0x8000000
#define SIZE_OF_KERNEL_STACK 0x2000 //8 KB
#define START_OF_KERNEL_STACKS 0x800000 //8 MB
#define MAX_ARG_SIZE 128

uint32_t curr_process = 0;
PCB_t parent_pcb_orig;
PCB_t * parent_pcb = &parent_pcb_orig;
uint8_t task_name[MAX_ARG_SIZE];

void execute(uint8_t * str) {
    parseString(str);
    checkIfExecutable();
    switch_task_memory();
    load_program_into_memory();
    create_pcb_child();
    prepare_context_switch();
    push_IRET_context();
    //IRET in assembly
}

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
    uint32_t task_memory = 0x8000000; // task memory is a 4 MB page, 128MB in virtual memory
    pageDirectory[2 + parent_pcb->n_tasks_executed] = task_memory | 0x83;    
    //<FLUSH TLB HERE>
}

void load_program_into_memory(uint8_t * filename) {
    //take file data and directly put into memory location
    uint32_t task_ptr = TASK_VIRTUAL_LOCATION;
    file_open(filename);
    file_read(0, task_ptr, 1000000); //nbytes is a really large number because we want to read the whole file.
}

void create_pcb_child() {
    PCB_t child_pcb;
    child_pcb.n_tasks_executed = parent_pcb->n_tasks_executed + 1;
    child_pcb.parentPtr = START_OF_KERNEL_STACKS - SIZE_OF_KERNEL_STACK*parent_pcb->n_tasks_executed;
    child_pcb.process_id = parent_pcb->process_id + 1;
    memcpy(child_pcb.parentPtr - SIZE_OF_KERNEL_STACK, &child_pcb, sizeof(child_pcb));
}

void prepare_context_switch() {
    //set SS0 and ESP0 in TSS
}

void push_IRET_context() {
    //set EIP(bytes 24-27 of executable loaded)
    //set CS
    //set EFLAGS
    //set ESP
    //set SS
}







