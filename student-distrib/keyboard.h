#define KEYBOARD_IRQ 0x01
#define NUM_KEYS   64         //number of scan codes
#define KEYBOARD_PORT	0x60   

#include "i8259.h"

//extern functions
extern void initialize_keyboard();
extern void key_board_handler();


