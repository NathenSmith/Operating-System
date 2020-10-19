#include "rtc.h"
#include "i8259.h"

/* SOURCES:  https://wiki.osdev.org/RTC 
    some code from Linux documentation of RTC
*/

/* initialize_rtc
 * 
 * enables interrupt on IRQ line of rtc on the pic and initializes registers
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Return value: None
 */ 
void initialize_rtc(){
    cli();
    outb(REGISTER_B, RTC_PORT);		// select register B, and disable NMI
    char prev=inb(CMOS_PORT);	// read the current value of register B
    outb(REGISTER_B, RTC_PORT);		// set the index again (a read will reset the index to register D)
    outb(prev | INT_TURN_ON, CMOS_PORT);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
    sti();
    enable_irq(RTC_IRQ);
}

/* rtc_handler
 * 
 * handles interrupt of rtc
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Return value: None
 */ 
void rtc_handler(){  
   
    
    outb(REGISTER_C, RTC_PORT);	// select register C
    
    inb(CMOS_PORT);		// just throw away contents
    test_interrupts();
    send_eoi(RTC_IRQ);   //to stop interrupt that was recieved
}

