/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xff; /* IRQs 0-7  */
uint8_t slave_mask = 0xff;  /* IRQs 8-15 */

//data ports on master and slave
uint16_t MASTER_DATA = MASTER_8259_PORT + 1;
uint16_t SLAVE_DATA = SLAVE_8259_PORT + 1;

/* Initialize the 8259 PIC */
void i8259_init(void) {   
    outb(master_mask, MASTER_DATA);
    outb(slave_mask, SLAVE_DATA);
    
    //send control words to master
    outb(ICW1, MASTER_8259_PORT);      //initialization control word
    outb(ICW2_MASTER, MASTER_DATA);  //second control word to master, mapped on 0x20
    outb(ICW3_MASTER, MASTER_DATA);   //has slave on IR2
    outb(ICW4, MASTER_DATA);

    //send control words to slave
    outb(ICW1, SLAVE_8259_PORT);      //initialization control word
    outb(ICW2_SLAVE, SLAVE_DATA);  //second control word to SLAVE, mapped on 0x20
    outb(ICW3_SLAVE, SLAVE_DATA);   //has slave on IR2
    outb(ICW4, SLAVE_DATA);  //end of interrupt

    outb(master_mask, MASTER_DATA);
    outb(slave_mask, SLAVE_DATA);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {

    if(irq_num < 8) {     //on master
        master_mask = inb(MASTER_DATA) & ~(1 << irq_num);
        outb(master_mask, MASTER_DATA);
    } else {
        irq_num -= 8;     //to get irq on slave chip
        slave_mask = inb(SLAVE_DATA) & ~(1 << irq_num);
        outb(slave_mask, SLAVE_DATA);
        outb(inb(MASTER_DATA) & ~(ICW3_SLAVE), MASTER_DATA);
    }
}
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint16_t port;
 
    if(irq_num < 8) {     //on master
        port = MASTER_DATA;
        master_mask = inb(MASTER_DATA) | (1 << irq_num);
        outb(master_mask, MASTER_DATA);
    } else {
        port = SLAVE_DATA;
        irq_num -= 8;     //to get irq on slave chip
        slave_mask = inb(SLAVE_DATA) | (1 << irq_num);
        outb(slave_mask, port);
        outb(inb(MASTER_DATA) | (ICW3_SLAVE), MASTER_DATA);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if(irq_num >= 8)             ///if inq_num is on slave
		outb(EOI | irq_num, SLAVE_8259_PORT);
    outb(EOI | irq_num, MASTER_8259_PORT);
}
