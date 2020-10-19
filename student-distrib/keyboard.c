#include "keyboard.h"

static 

void initialize_keyboard(){
    enable_irq(KEYBOARD_IRQ);   //enable interrupt on pin on pic
}

static char scan_codes[NUM_KEYS]= 	{'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v', 
	 'b', 'n', 'm',',', '.', '/', '\0', '*', '\0', ' ', '\0'};

}

static 
void key_board_handler(){
    send_eoi(KEYBOARD_IRQ);     //stop interrupt on pin
    cli();
    if(inb(KEYBOARD_PORT) < NUM_KEYS && inb(KEYBOARD_PORT) >= 0){   //check if scan code is in bounds
        putc(scan_codes[inb(KEYBOARD_PORT)]);
    }
    sti();
}