#include "pit.h"
#include "shared_global_variables.h"
#include "linkage.h"
#include "execute.h"

int i = 0;
PCB_t * active_processes[3];

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
 
