#define KEYBOARD_IRQ 0x01
#define NUM_KEYS   64         //number of scan codes
#define KEYBOARD_PORT	0x60   

#include "i8259.h"

char scan_codes[NUM_KEYS]= 	{'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v', 
	 'b', 'n', 'm',',', '.', '/', '\0', '*', '\0', ' ', '\0'};     //scan_code array inputs from keyboard

//extern functions
extern void initialize_keyboard();
extern void key_board_handler();


