#include "pit.h"
#include "shared_global_variables.h"
#include "linkage.h"
#include "execute.h"
#include "paging.h"

int scheduled_terminal = 0;
PCB_t * active_processes[3];
uint32_t visible_terminal;

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
    
    //copy video memory from previous process into corresponding video memory backup
    // switch(scheduled_terminal){
    //     case 0:
    //         memcpy((void *)BACKUP_ONE, (void *)VIDEO_MEMORY_IDX, 0x1000);
    //         break;
    //     case 1:
    //         memcpy((void *)BACKUP_TWO, (void *)VIDEO_MEMORY_IDX, 0x1000);
    //         break;
    //     case 2:
    //         memcpy((void *)BACKUP_THREE, (void *)VIDEO_MEMORY_IDX, 0x1000);
    //         break;
    //     default:
    //         break;
    // }
    
    //SCHEDULE
    if(scheduled_terminal == visible_terminal) { //scheduled terminal is the same as the visible terminal
        //set video memory
        pageTable[VIDEO_MEMORY_IDX >> 12] = (VIDEO_MEMORY_IDX | 0x003); // 0x3 are bits needed to set present, rw, supervisor
    }
    else {
        pageTable[VIDEO_MEMORY_IDX >> 12] = VIDEO_MEMORY_IDX + ((0x1000*(scheduled_terminal + 1)) | 0x003);
    }
    //save eip from the previous process to the pcb for that process
    curr_pcb->eip = save_eip;

    //increment scheduled terminal number
    scheduled_terminal++;

    //if done with all active processes, go to start of active proceses
    if((scheduled_terminal >= 3) || (active_processes[scheduled_terminal] == NULL)) {
        scheduled_terminal = 0;
    }

    int x = 0;
    while(active_processes[x]) {x++;}
    if(x <= 1) {return;} //no processes and no need to reschedule a single process

    //get curr_pcb for new process
    curr_pcb = (PCB_t *)(START_OF_KERNEL_STACKS - (active_processes[scheduled_terminal]->process_id)*SIZE_OF_KERNEL_STACK);
    //kinda unsure about this - Piyush sud 11/30/2020 8:32 CST
    switch_task_memory();
    load_program_into_memory(curr_pcb->filename);
    prepare_context_switch();
        
    return;
}

void switch_terminal(uint32_t terminal_num){
    send_eoi(0x01);
    cli();
    memcpy(VIDEO_MEMORY_IDX + ((0x1000*(visible_terminal + 1)) | 0x003), VIDEO_MEMORY_IDX, 0x1000);
    visible_terminal = terminal_num;
    memcpy(VIDEO_MEMORY_IDX, VIDEO_MEMORY_IDX + ((0x1000*(terminal_num + 1)) | 0x003), 0x1000);
    if(active_processes[visible_terminal] == NULL) { //if never opened terminal before
        clear();
        execute((uint8_t *)"shell");
    }
    else{
        //switch to user program
        curr_pcb = (PCB_t *)(START_OF_KERNEL_STACKS - (active_processes[scheduled_terminal]->process_id)*SIZE_OF_KERNEL_STACK);
        
        
        switch_task_memory();
        load_program_into_memory(curr_pcb->filename);
        prepare_context_switch();
        //push the iret context and iret to the scheduled process
        push_iret_context(curr_pcb->eip);
    }
    

    

    // if(scheduled_terminal == terminal_num) { //scheduled terminal is the same as the visible terminal
    //     //set video memory
    //     pageTable[VIDEO_MEMORY_IDX >> 12] = (VIDEO_MEMORY_IDX | 0x003); // 0x3 are bits needed to set present, rw, supervisor
    // }
    // else {
    //     //pageDirectory[0] = ((uint32_t)pageTable | 0x003); // 0x3 are bits needed to set present, rw, supervisor
    //     pageTable[VIDEO_MEMORY_IDX >> 12] = VIDEO_MEMORY_IDX + ((0x1000*(visible_terminal + 1)) | 0x003);
    //     flush_tlb();
    //     if(active_processes[visible_terminal] == NULL) { //if never opened terminal before
    //         execute((uint8_t *)"shell");
    //     }
      
    // }
    
}
 
