#include "rtc.h"
#include "i8259.h"

void initialize_rtc(){
    outb(0x8B, RTC_PORT);		// select register B, and disable NMI
    char prev=inb(CMOS_PORT);	// read the current value of register B
    outb(0x8B, RTC_PORT);		// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, CMOS_PORT);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
    enable_irq(RTC_IRQ);
}

void rtc_handler(){  
    send_eoi(RTC_IRQ);   //to stop interrupt that was recieved
    cli();
    outportb(0x70, 0x0C);	// select register C
    inportb(0x71);		// just throw away contents
    sti();
}

