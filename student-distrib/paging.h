#ifndef PAGING_H
#define PAGING_H

#include "x86_desc.h"
#include "lib.h"

uint32_t pageTable[1024] __attribute__((aligned (4096))); // page table, aligned
uint32_t pageDirectory[1024] __attribute__((aligned (4096))); // page directory, aligned

extern void paging_init();

#endif
