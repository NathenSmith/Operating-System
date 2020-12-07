#include "paging.h"

/* paging_init
 *  DESCRIPTION: Initialize paging with page directory and page table
 *  INPUTS: None
 *  OUTPUTS: None
 *  RETURN VALUE: None
 *  SIDE EFFECTS: None
 */

void paging_init(void) {

    //0x2 are bits needed to set not present, rw, supervisor
    int i;
    for(i = 0; i < DIRECTORY_SIZE; i++) pageDirectory[i] = 0x002;
    for(i = 0; i < DIRECTORY_SIZE; i++) pageTable[i] = 0x002;

    //set video memory
    pageDirectory[0] = ((uint32_t)pageTable | 0x003); // 0x3 are bits needed to set present, rw, supervisor
    pageTable[VIDEO_MEMORY_IDX >> 12] = (VIDEO_MEMORY_IDX | 0x003); // 0x3 are bits needed to set present, rw, supervisor
    pageTable[(VIDEO_MEMORY_IDX >> 12) + 1] = (BACKUP_ONE | 0x003); // 0x3 are bits needed to set present, rw, supervisor   
    pageTable[(VIDEO_MEMORY_IDX >> 12) + 2] = (BACKUP_TWO | 0x003); // 0x3 are bits needed to set present, rw, supervisor  
    pageTable[(VIDEO_MEMORY_IDX >> 12) + 3] = (BACKUP_THREE | 0x003); // 0x3 are bits needed to set present, rw, supervisor

    // set kernel memory at 4MB
    pageDirectory[1] = (KERNEL_IDX | 0x093);

    //cr3 has addr of page directory
    //cr0 is paging enable bit
    asm volatile(
        "movl %0, %%eax \n \
        movl %%eax, %%cr3 \n \
        movl %%cr4, %%eax \n \
        orl $0x00000010, %%eax \n \
        movl %%eax, %%cr4 \n \
        movl %%cr0, %%eax \n \
        orl $0x80000000, %%eax \n \
        movl %%eax, %%cr0"
        :
        :"r"(pageDirectory)
        :"%eax"
    );
}

