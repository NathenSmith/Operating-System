#include "keyboard.h"
#define VGA_WIDTH 80
//volatile int terminal_flag[3];

extern int32_t terminal_read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t terminal_write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t terminal_open (const uint8_t* filename);
extern int32_t terminal_close (int32_t fd);
//extern uint32_t bad_call () {return -1;} //should not be able to open close terminal
// void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);


