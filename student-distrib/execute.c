#include "execute.h"

#define TASK_VIRTUAL_LOCATION 0x8000000


void parseString(uint8_t * str) {
    uint8_t task_name[128];
    uint8_t arg[128];
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
        arg[j] = str[i + j];
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
    pageDirectory[2 + pcb->n_tasks_executed] = task_memory | 0x83;    
    //<FLUSH TLB HERE>
}

void load_program_into_memory(uint8_t * filename) {
    //take file data and directly put into memory location
    uint32_t task_ptr = TASK_VIRTUAL_LOCATION;
    file_open(filename);
    file_read(0, task_ptr, 1000000); //nbytes is a really large number because we want to read the whole file.
}

//todo: a) create PCB for child process,
//      b) prepare for context switch,
//      c) push IRET context to kernel stack






