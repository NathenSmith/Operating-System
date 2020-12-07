#include "pit.h"
#include "shared_global_variables.h"
#include "linkage.h"
#include "execute.h"
#include "paging.h"
#include "keyboard.h"
#define MODECMDREG 0x43
#define CHANNEL 0x36
#define FREQUENCY 11905
#define LOWBYTEMASK 0x000000FF
#define HIGHBYTESHIFT 8
#define MAXSHELLS 3
#define RELOADVAL 0x40
#define PITIRQ 0x0

int scheduled_terminal = 0;
int visible_terminal = 0;
int calls_to_schedule = 0;
int paging_scheme = 0;
int x = 0;
PCB_t * active_processes[3];

void initialize_pit(){
    outb(CHANNEL, MODECMDREG);		// enable mode 3
    outb(FREQUENCY & LOWBYTEMASK, 0x40); //low byte
    outb(FREQUENCY >> HIGHBYTESHIFT, RELOADVAL); //high 8 bits
    int j;
    for(j = 0; j < MAXSHELLS; j++) {
        active_processes[j] = NULL;
    }
    enable_irq(PITIRQ); //irq is zero
}

void pit_handler() {
    //do something with curr process tracker
    schedule();
    send_eoi(PITIRQ);
}

void schedule() {
    calls_to_schedule++;
    if(total_processes < 3 ) {
        if(total_processes == 0){
            clear();
            execute((uint8_t *)"shell");            
        }
        save_ebp_esp((uint32_t)curr_pcb + ESP2_LOCATION, (uint32_t)curr_pcb + EBP2_LOCATION);
        switch_terminal(total_processes, 0);
        scheduled_terminal++;
        clear();
        execute((uint8_t *)"shell");
    }
    
    save_ebp_esp((uint32_t)curr_pcb + ESP2_LOCATION, (uint32_t)curr_pcb + EBP2_LOCATION);

    if(calls_to_schedule == 4) {
        switch_terminal(0, 1);
    }
    // else {
    //     //save cursor
    //     active_processes[scheduled_terminal]->screen_x = get_x();
    //     active_processes[scheduled_terminal]->screen_y = get_y();
    // }     

    //increment scheduled terminal number
    
   
    scheduled_terminal = (scheduled_terminal + 1) % 3; 
    //if done with all active processes, go to start of active proceses
    // //switch paging for video memory
    // if(scheduled_terminal == visible_terminal) { 
    //     pageTable[VIDEO_MEMORY_IDX >> 12] = (VIDEO_MEMORY_IDX | 0x003); // 0x3 are bits needed to set present, rw, supervisor
    //     //paging_scheme = 0;
    // }
    // else {
    //     //for backups
    //     pageTable[VIDEO_MEMORY_IDX >> 12] = ((VIDEO_MEMORY_IDX + (0x1000*(scheduled_terminal + 1))) | 0x003);
    //     //paging_scheme = scheduled_terminal + 1;
    // }
    // flush_tlb();

    //get curr_pcb for new process
    curr_pcb = active_processes[scheduled_terminal];
    // printf("curr_pcb: %d\n", curr_pcb);    

    //switch paging for user program memory
    switch_task_memory();

    //set TSS
    prepare_context_switch();
    //set cursor 
    if(calls_to_schedule != 4) {
       //update_cursor(active_processes[visible_terminal]->screen_x, active_processes[visible_terminal]->screen_y);
    }

    //restore ebp and esp for newly scheduled process
    restore_ebp_esp(curr_pcb->esp2, curr_pcb->ebp2); 
}

void switch_terminal(uint32_t terminal_num, int state){
    //save cursor
    // active_processes[visible_terminal]->screen_x = get_x();
    // active_processes[visible_terminal]->screen_y = get_y(); 

    //save and restore video memory
    memcpy((void *) (VIDEO_MEMORY_IDX + ((0x1000*(visible_terminal + 1)))), (void *) VIDEO_MEMORY_IDX, 0x1000);
    visible_terminal = terminal_num;
    memcpy((void *)VIDEO_MEMORY_IDX, (void *) (VIDEO_MEMORY_IDX + ((0x1000*(terminal_num + 1)))), 0x1000);  

    //pageTable[VIDEO_MEMORY_IDX >> 12] = ((VIDEO_MEMORY_IDX + (0x1000*(terminal_num + 1))) | 0x003);
    
    //set cursor
    if(state == 0) {
        //for first 3 shells
        update_cursor(0, 0, 0);
    }
    else {
        update_cursor(get_x(visible_terminal), get_y(visible_terminal), 0);
    }
    // restore_original_paging();
    send_eoi(0x01);
}
    
//writing to terminal: 
    //when the user types something, the program should write to virtual video memory using putc, which should point to physical video memory.
    //when a program writes to memory using terminal write, the virtual video memory should point to either the backup or the physical video memory.
    

 
