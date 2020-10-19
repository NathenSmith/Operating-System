#include "keyboard.h"

/* SOURCES:  https://wiki.osdev.org/PS/2_Keyboard
    some code from Linux documentation of PS/2 Keyboard
*/

/* initialize_keyboard
 * 
 * enables interrupt on IRQ line of keyboard on the pic
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Return value: None
 */ 
void initialize_keyboard(){
    printf("kbd init");
    enable_irq(KEYBOARD_IRQ);   //enable interrupt on pin on pic
}
 
/* key_board_handler
 * 
 * handles the interrupts of the keyboard, prints to console
 * Inputs: None
 * Outputs: character on screen
 * Side Effects: None
 * Return value: None
 */ 
void key_board_handler(){
    printf("kbd handler");
    
    send_eoi(KEYBOARD_IRQ);     //stop interrupt on pin
    
    if(inb(KEYBOARD_PORT) < NUM_KEYS && inb(KEYBOARD_PORT) >= 0){   //check if scan code is in bounds of scan code array
        putc(scan_codes[inb(KEYBOARD_PORT)]);   //print character to screen
    }
    
}