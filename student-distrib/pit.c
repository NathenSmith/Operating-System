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
PCB_t * active_processes[MAXSHELLS];

/* initialize_pit
 * 
 * enables interrupt on IRQ line of pit on pic
 * Inputs: None
 * Outputs: None
 * Side Effects: interrupts enabled
 * Return value: None
 */ 

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

/* pit_handler
 * 
 * schedules a process for every interrupt on the pit
 * Inputs: None
 * Outputs: None
 * Side Effects: process scheduled
 * Return value: None
 */ 

void pit_handler() {
    //do something with curr process tracker
    schedule();
    send_eoi(PITIRQ);
}

/* schedule
 * 
 * Description: round robin implementation of scheduling to 
 * cycle through the active process of every terminal
 * Inputs: None
 * Outputs: None
 * Side Effects: switching to user program of new terminal's process
 * Return value: None
 */ 

void schedule() {
    //execute first 3 shells on first 3 schedule calls
    calls_to_schedule++;
    if(total_processes < MAXSHELLS) {
        if(total_processes == 0){
            clear();
            execute((uint8_t *)"shell");            
        }

        //save ebp, esp of curr_pcb
        save_ebp_esp((uint32_t)curr_pcb + ESP2_LOCATION, (uint32_t)curr_pcb + EBP2_LOCATION);

        switch_terminal(total_processes, 0);
        scheduled_terminal++;
        clear();
        execute((uint8_t *)"shell");
    }
    
    //save ebp, esp of curr_pcb
    save_ebp_esp((uint32_t)curr_pcb + ESP2_LOCATION, (uint32_t)curr_pcb + EBP2_LOCATION);

    //start on terminal 0 on (4)th call to schedule
    if(calls_to_schedule == 4) {
        switch_terminal(0, 1);
    }

    //round robin
    scheduled_terminal = (scheduled_terminal + 1) % 3; 

    //switch paging for video memory
    if(scheduled_terminal == visible_terminal) { 
        pageTable[VIDEO_MEMORY_IDX >> 12] = (VIDEO_MEMORY_IDX | 0x003); // 0x3 are bits needed to set present, rw, supervisor
    }
    else {
        //for backups
        pageTable[VIDEO_MEMORY_IDX >> 12] = ((VIDEO_MEMORY_IDX + (0x1000*(scheduled_terminal + 1))) | 0x003);
    }
    flush_tlb();

    //get curr_pcb for new process
    curr_pcb = active_processes[scheduled_terminal];    

    //switch paging for user program memory
    switch_task_memory();

    //set TSS
    prepare_context_switch();

    //restore ebp and esp for newly scheduled process
    restore_ebp_esp(curr_pcb->esp2, curr_pcb->ebp2); 
}

/* switch_terminal
 * 
 * Description: switches video memory to point to new visible terminal
 * Inputs: None
 * Outputs: None
 * Side Effects: different video memory now visible
 * Return value: None
 */ 

void switch_terminal(uint32_t terminal_num, int state){
    //save and restore video memory
    memcpy((void *) (VIDEO_MEMORY_IDX + ((PAGE_SIZE*(visible_terminal + 1)))), (void *) VIDEO_MEMORY_IDX, PAGE_SIZE);
    visible_terminal = terminal_num;
    memcpy((void *)VIDEO_MEMORY_IDX, (void *) (VIDEO_MEMORY_IDX + ((PAGE_SIZE*(terminal_num + 1)))), PAGE_SIZE);  
    
    //set cursor
    if(state == 0) {
        //for first 3 shells
        update_cursor(0, 0, 0);
    }
    else {
        update_cursor(get_x(visible_terminal), get_y(visible_terminal), 0);
    }
    send_eoi(0x01);
}
    

 
