#include "keyboard.h"
#define VGA_WIDTH 80
volatile int terminal_flag;

extern uint32_t terminal_read (uint32_t fd, void* buf, uint32_t nbytes);
extern uint32_t terminal_write (uint32_t fd, const void* buf, uint32_t nbytes);
extern uint32_t terminal_open (const uint8_t* filename);
extern uint32_t terminal_close (uint32_t fd);
//extern uint32_t bad_call () {return -1;} //should not be able to open close terminal
// void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);


