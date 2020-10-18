/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

//lock used for initializing pic
spinlock_t * i8259A_lock;

/* Initialize the 8259 PIC */
void i8259_init(void) {   
    unsigned long flags;

    spin_lock_irqsave(&i8259A_lock, flags);    //save flags and lock

    outb(0xff, MASTER_8259_PORT + 1);       //mask interrupts on master data port
    outb(0xff, SLAVE_8259_PORT + 1);       //mask interrupts on slave data port
    
    //send control words to master
    outb_p(ICW1, MASTER_8259_PORT);      //initialization control word
    outb_p(ICW2_MASTER, MASTER_8259_PORT + 1);  //second control word to master, mapped on 0x20
    outb_p(ICW3_MASTER, MASTER_8259_PORT +1);   //has slave on IR2
    outb_p(EOI, MASTER_8259_PORT + 1);

    //send control words to slave
    outb_p(ICW1, SLAVE_8259_PORT);      //initialization control word
    outb_p(ICW2_SLAVE, SLAVE_8259_PORT + 1);  //second control word to SLAVE, mapped on 0x20
    outb_p(ICW3_SLAVE, SLAVE_8259_PORT +1);   //has slave on IR2
    outb_p(EOI, SLAVE_8259_PORT + 1);

    spin_unlock_irqrestore(&i8259A_lock, flags);    //restore flags and unlock
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint16_t port;
 
    if(irq_num < 8) {     //on master
        port = MASTER_8259_PORT + 1;
        master_mask = inb(port) & ~(1 << irqnum);
        outb(master_mask, port);
    } else {
        port = SLAVE_8259_PORT + 1;
        irq_num -= 8;     //to get irq on slave chip
        slave_mask = inb(port) & ~(1 << irqnum);
        outb(slave_mask, port);
    }
}
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint16_t port;
 
    if(irq_num < 8) {     //on master
        port = MASTER_8259_PORT + 1;
        master_mask = inb(port) | (1 << irqnum);
        outb(master_mask, port);
    } else {
        port = SLAVE_8259_PORT + 1;
        irq_num -= 8;     //to get irq on slave chip
        slave_mask = inb(port) | (1 << irqnum);
        outb(slave_mask, port);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if(irq_num >= 8)             ///if inq_num is on slave
		outb(EOI, SLAVE_8259_PORT);
    outb(EOI, MASTER_8259_PORT);
}
