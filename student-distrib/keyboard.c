#include "keyboard.h"

/* SOURCES:  https://wiki.osdev.org/PS/2_Keyboard
    some code from Linux documentation of PS/2 Keyboard
*/

static uint8_t check_if_letter(char index);
static uint8_t check_if_symbol(char index);

static char scan_codes[NUM_KEYS]= 	{ //presses
    '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v', 
	 'b', 'n', 'm',',', '.', '/', '\0', '*', '\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0',
      '\0', '\0', '\0', '\0', '\0','\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
      '\0', '\0', '\0', '\0', '\0', '\0', '\0'
     }; 
static shift_state = 0;  //state to check if shift key is pressed


/* initialize_keyboard
 * 
 * enables interrupt on IRQ line of keyboard on the pic
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Return value: None
 */ 
void initialize_keyboard(){
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
    //---------shift checks---------
    if(shift_state){        
        //conversion to uppercase
        if(check_if_letter(inb(KEYBOARD_PORT))){
            putc(scan_codes[inb(KEYBOARD_PORT)] - CASE_CONVERSION); 
            send_eoi(KEYBOARD_IRQ);  
        }
        else if(check_if_symbol(inb(KEYBOARD_PORT))){
            putc(check_if_symbol(inb(KEYBOARD_PORT)));
            send_eoi(KEYBOARD_IRQ);
        }
        else if(inb(KEYBOARD_PORT) != 0xAA || inb(KEYBOARD_PORT) != 0xB6){
            shift_state = 0;
            send_eoi(KEYBOARD_IRQ); 
        }
    }
    else{
        if(inb(KEYBOARD_PORT) == 0x2A || inb(KEYBOARD_PORT) == 0x36){
            shift_state = 1;
            send_eoi(KEYBOARD_IRQ); 
        }
        //check if scan code is in bounds of scan code array
        else if(inb(KEYBOARD_PORT) < NUM_KEYS && inb(KEYBOARD_PORT) >= 0){   
            putc(scan_codes[inb(KEYBOARD_PORT)]);   //print character to screen
            send_eoi(KEYBOARD_IRQ);  //stop interrupt on pin
        }
    }
    send_eoi(KEYBOARD_IRQ);  //stop interrupt on pin    
}

/* check_if_letter
 * 
 * Description:checks if the scan code is a letter so 
 * it can be capitalized during a shift
 * Inputs: index: scan code index
 * Outputs: none
 * Side Effects: None
 * Return value: true/false (0/1)
 */ 
static uint8_t check_if_letter(char index){
    uint8_t check = 0;
    if(index >= 0x10 && index <= 0x19){
        check = 1;
    }
    else if(index >= 0x1E && index <= 0x26){
        check = 1;
    }
    else if(index >= 0x2C && index <= 0x32){
        check = 1;
    }
    return check;
}

/* check_if_symbol
 * 
 * Description:checks if the scan code is a symbol so 
 * it can be changed during a shift
 * Inputs: index: scan code index
 * Outputs: none
 * Side Effects: None
 * Return value: converted index (0 if not symbol)
 */ 
static uint8_t check_if_symbol(char index){
    switch(index){
        //scancodes (0x02-0x0B) for numbers 0-9
        case 0x02:
            return '!';
        case 0x03:
            return '@';
        case 0x04:
            return '#';
        case 0x05:
            return '$';
        case 0x06:
            return '%';
        case 0x07:
            return '^';
        case 0x08:
            return '&';
        case 0x09:
            return '*';
        case 0x0A:
            return '(';
        case 0x0B:
            return ')';
        default:
            return 0;
    }
}
