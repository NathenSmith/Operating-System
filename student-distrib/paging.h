// this is going to be the paging.h file
#ifndef PAGING_H
#define PAGING_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"

#define VIDEO_MEMORY_IDX 0xB8000
#define KERNEL_IDX 0x400000

#define DIRECTORY_SIZE 1024

uint32_t pageDirectory[DIRECTORY_SIZE] __attribute__((aligned (4096)));
uint32_t pageTable[DIRECTORY_SIZE] __attribute__((aligned (4096)));

extern void paging_init(void);

#endif