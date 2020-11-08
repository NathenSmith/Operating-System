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
    for(i = 0; i < DIRECTORY_SIZE; i++) pageDirectory[i] = 0x2;
    for(i = 0; i < DIRECTORY_SIZE; i++) pageTable[i] = 0x2;

    pageDirectory[0] = ((uint32_t)pageTable | 0x3); // 0x3 are bits needed to set present, rw, supervisor

    // kernel memory starts at KERNEL = 4MB
    pageDirectory[1] = (KERNEL_IDX | 0x83);

    pageTable[VIDEO_MEMORY_IDX >> 12] = (VIDEO_MEMORY_IDX | 0x3); // 0x3 are bits needed to set present, rw, supervisor

    //cr3 has addr of page directory
    //cr0 is paging enable bit
    asm volatile(
        "movl %0, %%eax \n \
        movl %%eax, %%cr3 \n \
        movl %%cr4, %%eax \n \
        orl $0x00000010, %%eax \n \
        movl %%eax, %%cr4 \n \
        movl %%cr0, %%eax \n \
        orl $0x80000001, %%eax \n \
        movl %%eax, %%cr0"
        :
        :"r"(pageDirectory)
        :"%eax"
    );
}