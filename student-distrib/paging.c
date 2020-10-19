#include "paging.h"

#define DIRECTORY_SIZE 1024
#define PAGE_SIZE 4096
#define VIDEO_MEMORY_IDX 0xB8000

/* paging_init
 *  DESCRIPTION: Initialize paging with page directory and page table
 *  INPUTS: None
 *  OUTPUTS: None
 *  RETURN VALUE: None
 *  SIDE EFFECTS: None
 */
void paging_init() {
    int i;
    //0x2 are bits needed to set not present, rw, supervisor
    for(i = 0; i < DIRECTORY_SIZE; i++) pageDirectory[i] = 0x2;
    for(i = 0; i < DIRECTORY_SIZE; i++) pageTable[i] = 0x2; //0x2 means not present

    pageDirectory[0] = (uint32_t)pageTable | 0x3; // 0x3 are bits needed to set present, rw, supervisor

    uint32_t portion = 0x400000; // 4-8MB is a single 4 MB page
    pageDirectory[1] = portion | 0x83;

    portion = VIDEO_MEMORY_IDX;
    portion = portion | 0x3;

    uint32_t index = VIDEO_MEMORY_IDX >> 12;
    pageTable[index] = portion;
    pageTable[index + 1] = 0xB9000 | 0x3; // 0x3 are bits needed to set present, rw, supervisor
    pageTable[index + 2] = 0xBA000 | 0x3;
    pageTable[index + 3] = 0xBB000 | 0x3;

    asm volatile( // code to enable paging by setting a bit of cr0
          "movl %0, %%eax;"
          "movl %%eax, %%cr3;"
          "movl %%cr4, %%eax;"
          "orl $0x10, %%eax;"
          "movl %%eax, %%cr4;"
          "movl %%cr0, %%eax;"
          "orl $0x80000000, %%eax;" //0x8000000 is the bit of cr0 that enables paging
          "movl %%eax, %%cr0;"
          :
          :"r"(pageDirectory)
          :"%eax"
    );
}
