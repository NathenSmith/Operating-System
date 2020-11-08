#include "keyboard.h"
#define VGA_WIDTH 80
volatile int terminal_flag;

uint32_t terminal_read (uint32_t fd, char* buf, uint32_t nbytes);
uint32_t terminal_write (uint32_t fd, const char* buf, uint32_t nbytes);
uint32_t terminal_open (const uint8_t* filename);
uint32_t terminal_close (uint32_t fd);
uint32_t bad_call () {return -1;} //should not be able to open close terminal
// void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);


