#include keyboard.h

#define KEYBOARD_IRQ 0x01;

void initialize_keyboard(){
    enable_irq(KEYBOARD_IRQ);
}

void key_board_handler(){

}