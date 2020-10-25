#define KEYBOARD_IRQ 0x01
#define NUM_KEYS   88         //number of scan codes
#define KEYBOARD_PORT	0x60 
#define CASE_CONVERSION 32   //to convert ascii upper case and lower case  

//asciis for shifted characters
#define EXCL    33
#define AT      64
#define POUND   35
#define DOLLAR  36
#define PERCENT 37
#define CARROT  94
#define AMP     38
#define STAR    42
#define OP      40
#define CP      41

#include "i8259.h"

//extern functions
extern void initialize_keyboard();
extern void key_board_handler();


