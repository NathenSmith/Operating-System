#define KEYBOARD_IRQ 0x01
#define NUM_KEYS   88         //number of scan codes
#define KEYBOARD_PORT	0x60 
#define CASE_CONVERSION 32   //to convert ascii upper case and lower case  
#define NUM_STATES 4
#define BUF_SIZE  128
#define SHIFT_STATE  0
#define CAPS_STATE   1
#define CTRL_STATE   2
#define ALT_STATE    3

#include "i8259.h"
char kbd_buf[128]; //2d array having 3 buffers for each terminal
//int buf_counter;


//extern functions
extern void initialize_keyboard();
extern void key_board_handler();
extern void add_to_kdb_buf(char c);
extern void backspace_buffer();

