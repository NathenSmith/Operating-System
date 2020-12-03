#include "pit.h"
#include "shared_global_variables.h"
#include "linkage.h"
#include "execute.h"
#include "paging.h"

int scheduled_terminal = 0;
int visible_terminal = 0;
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
    //save ebp and esp
    save_ebp_esp(curr_pcb + ESP_LOCATION, curr_pcb + EBP_LOCATION);

    //save cursor
    curr_pcb->screen_x = get_x();
    curr_pcb->screen_y = get_y(); 

    //switch paging for video memory
    if(scheduled_terminal == visible_terminal) { 
        pageTable[VIDEO_MEMORY_IDX >> 12] = (VIDEO_MEMORY_IDX | 0x003); // 0x3 are bits needed to set present, rw, supervisor
    }
    else {
        pageTable[VIDEO_MEMORY_IDX >> 12] = VIDEO_MEMORY_IDX + ((0x1000*(scheduled_terminal + 1)) | 0x003);
    }

    //increment scheduled terminal number
    scheduled_terminal++;

    //if done with all active processes, go to start of active proceses
    if((scheduled_terminal >= 3) || (active_processes[scheduled_terminal] == NULL)) {
        scheduled_terminal = 0;
    }

    //no need to reschedule for 0 or 1 running processes
    int x = 0;
    while(active_processes[x]) {x++;}
    if(x <= 1) return; 

    //get curr_pcb for new process
    curr_pcb = active_processes[scheduled_terminal];

    //set cursor
    update_cursor(curr_pcb->screen_x, curr_pcb->screen_y);

    //switch paging for user program memory
    switch_task_memory();

    //restore ebp and esp for newly scheduled process
    restore_ebp_esp(curr_pcb->esp, curr_pcb->ebp); 

    //set TSS
    prepare_context_switch();
}

void switch_terminal(uint32_t terminal_num){
    send_eoi(0x01);

    //save cursor
    curr_pcb->screen_x = get_x();
    curr_pcb->screen_y = get_y(); 

    //save and restore video memory
    memcpy((void *) (VIDEO_MEMORY_IDX + ((0x1000*(visible_terminal + 1)))), (void *) VIDEO_MEMORY_IDX, 0x1000);
    visible_terminal = terminal_num;
    memcpy((void *)VIDEO_MEMORY_IDX, (void *) (VIDEO_MEMORY_IDX + ((0x1000*(terminal_num + 1)))), 0x1000);    

    if(active_processes[terminal_num] == NULL) { //if never opened terminal before
        //clear();
        execute((uint8_t *)"shell");
    }    

    //get curr_pcb for new process
    curr_pcb = active_processes[visible_terminal];

    //set cursor
    update_cursor(curr_pcb->screen_x, curr_pcb->screen_y);

    //switch paging for user program memory
    switch_task_memory();
}
    

    


 
