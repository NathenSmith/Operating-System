#include "execute.h"
#include "filesystem.h"
#include "rtc.h"
#include "terminal.h"

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



