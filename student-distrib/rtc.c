#include "rtc.h"
#include "i8259.h"

volatile int rtc_interrupt_flag = 0;

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
    int32_t default_freq = 2;
    rtc_write(0, &default_freq, 4);
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
    send_eoi(RTC_IRQ);   //to stop interrupt that was recieved 
    rtc_interrupt_flag = 1;
    outb(REGISTER_C, RTC_PORT);	// select register C
    inb(CMOS_PORT);		// just throw away contents
    //test_interrupts();
}
//should only return 0: appendix b
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
    while(!rtc_interrupt_flag)
    {
        //Do nothing
    }

    rtc_interrupt_flag = 0;
    return 0;
}

int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    if(nbytes != 4 || buf == NULL) return -1;

    int32_t freq = *buf;
    int32_t i;
    uint8_t rate = 0x0F;
    if ((freq & (freq - 1)) == 0) && (freq <= FREQ_MAX){ //if freq is a power of 2, https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2
        if(freq == 0) rate = 0;
        else{
            //for(i = freq; i != 0x02; i >> 1)
            while(freq != 0x02){
                freq >> 1;
                rate--;
            }
        }
        cli(); //changing interrupt rate, from osdev
        outb(REGISTER_A, RTC_PORT);
        char prev = inb(CMOS_PORT);
        outb(REGISTER_A, RTC_PORT);
        outb(CMOS_PORT, (prev & 0xF0) | rate);
        sti();
        return 0; //success
    }
    return -1;
}

int32_t rtc_open(const uint8_t* filename){
    return 0;
}

int32_t rtc_close(int32_t fd){
    return 0;
}
