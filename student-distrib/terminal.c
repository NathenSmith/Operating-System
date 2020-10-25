#include "terminal.h"

uint32_t terminal_read (uint32_t fd, void* buf, uint32_t nbytes){
    return 0;
}
uint32_t terminal_write (uint32_t fd, const void* buf, uint32_t nbytes){
    return 0;
}
uint32_t terminal_open (const uint8_t* filename){
    return 0;
}
uint32_t terminal_close (uint32_t fd){
    return 0;
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void update_cursor(int x, int y)
{
	uint16_t pos = y * VGA_WIDTH + x;
 
	outb(0x0F, 0x3D4);
	outb((uint8_t) (pos & 0xFF), 0x3D5);
	outb(0x0E, 0x3D4);
	outb((uint8_t) ((pos >> 8) & 0xFF), 0x3D5);
}
