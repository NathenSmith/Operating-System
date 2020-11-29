#include "pit.h"
#include "shared_global_variables.h"
#include "linkage.h"
#include "execute.h"
#include "paging.h"

int i = 0;
PCB_t * active_processes[3];
uint32_t current_terminal;

void initialize_pit(){
    cli();
    outb(0x36, 0x43);		// enable mode 3
    // outb(0x00110110, 0x43);
    outb(11905 & 0x000000FF, 0x40); //low byte
    outb(11905 >> 8, 0x40); //high 8 bits
    int j;
    for(j = 0; j < 3; j++) {
        active_processes[j] = NULL;
    }
    sti();
    enable_irq(0x0); //irq is zero
}

void pit_handler() {
    //do something with curr process tracker
    schedule();
    send_eoi(0x0);
}

void schedule() {
    //save eip from the previous process to the pcb for that process
    curr_pcb->eip = save_eip;
    //copy into corresponding video memory backup
    switch(i){
        case 0:
            memcpy(BACKUP_ONE, VIDEO_MEMORY_IDX, 0x1000);
        case 1:
            memcpy(BACKUP_TWO, VIDEO_MEMORY_IDX, 0x1000);
        case 2:
            memcpy(BACKUP_THREE, VIDEO_MEMORY_IDX, 0x1000);
    }

    //increment process counter
    i++;

    //if done with all active processes, go to start of active proceses
    if(active_processes[i] != NULL) {
        i = 0;
    }

    //get curr_pcb for new process
    curr_pcb = (PCB_t *)(START_OF_KERNEL_STACKS - (active_processes[i]->process_id)*SIZE_OF_KERNEL_STACK);
     
    switch_task_memory();
    load_program_into_memory(curr_pcb->filename);
    prepare_context_switch();
        
    //push the iret context and iret to the scheduled process
    push_iret_context(curr_pcb->eip);
}

void switch_terminal(uint32_t terminal_num){
    cli();
    current_terminal = terminal_num;
    if(i == terminal_num){
        //set video memory
        pageDirectory[0] = ((uint32_t)pageTable | 0x003); // 0x3 are bits needed to set present, rw, supervisor
        pageTable[VIDEO_MEMORY_IDX >> 12] = (VIDEO_MEMORY_IDX | 0x003); // 0x3 are bits needed to set present, rw, supervisor
    }
    else{
        pageDirectory[0] = ((uint32_t)pageTable | 0x003); // 0x3 are bits needed to set present, rw, supervisor
        switch(i){
            case 0:
                pageTable[VIDEO_MEMORY_IDX >> 12] = (BACKUP_ONE | 0x003); // 0x3 are bits needed to set present, rw, supervisor 
            case 1:
                pageTable[VIDEO_MEMORY_IDX >> 12] = (BACKUP_TWO | 0x003); // 0x3 are bits needed to set present, rw, supervisor 
            case 2:
                pageTable[VIDEO_MEMORY_IDX >> 12] = (BACKUP_THREE | 0x003); // 0x3 are bits needed to set present, rw, supervisor      
        }
      
    }
    sti();
}
 
