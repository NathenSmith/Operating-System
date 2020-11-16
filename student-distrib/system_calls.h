#ifndef _SYSTEM_CALLS_H
#define _SYSTEM_CALLS_H

#include "shared_global_variables.h"

#define SHELL_ID 2
#define PAGE_MASK 0x097
#define EXCEPTION_NUM 256
#define USER_PAGE_START 0x08000000
#define USER_PAGE_END 0x08400000

extern int32_t halt(uint8_t status); 
extern int32_t execute(const uint8_t* command);
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t write(int32_t fd, void* buf, int32_t nbytes);
extern int32_t open(const uint8_t* filename);
extern int32_t close(int32_t fd);
extern int32_t getargs(uint8_t* buf, int32_t nbytes);
extern int32_t vidmap(uint8_t** screen_start);
extern void set_handler();
extern void sigreturn();

#endif



