#define KEYBOARD_IRQ 0x01
#define NUM_KEYS   64
#define KEYBOARD_PORT	0x60

void initialize_keyboard();
void key_board_handler();