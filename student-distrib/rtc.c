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
    // int32_t default_freq = 2;
    // rtc_write(0, &default_freq, 4);
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
}

/* rtc_read
 * 
 * Waits on interrupt to occur.
 * Inputs: None are currently used
 * Outputs: None
 * Side Effects: Resets interrupt flag after handler receives interrupt
 * Return value: Always zero
 */ 
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
    while(!rtc_interrupt_flag)
    {
        //Do nothing
    }

    rtc_interrupt_flag = 0;
    return 0;
}

/* rtc_write
 * 
 * Sets the frequency
 * Inputs: fd -- not currently used
 *         buf -- the frequency to change to
 *         nbytes -- how many bytes being sent
 * Outputs: None
 * Side Effects: Sets frequency
 * Return value: Zero on success, -1 if an invalid byte being sent, NULL pointer being passed in, or a frequency not a power of 2
 */ 
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    if(nbytes != EXPECTED_BYTES || buf == NULL) return -1;

    int32_t freq = *((int32_t *) buf);
    uint8_t rate = RATE_MAX;
    if(((freq & (freq - 1)) == 0) && (freq <= FREQ_MAX)){  //if freq is a power of 2, https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2
        if(freq == 0) rate = 0; //freq 0 just means rate of 0
        else{
            while(freq != RATE_MIN){
                freq = freq >> 1; 
                rate--; //used to obtain the correct rtc rate as defined by https://courses.grainger.illinois.edu/ece391/fa2020/secure/references/ds12887.pdf
            }
        }
        cli(); //changing interrupt rate, from osdev link above
        outb(REGISTER_A, RTC_PORT);
        char prev = inb(CMOS_PORT); //save the previous 
        outb(REGISTER_A, RTC_PORT);
        outb((prev & SECOND_HEX_MASK) | rate , CMOS_PORT);
        sti();
        return 0; //success
    }
    return -1;
}
/* rtc_open
 * 
 * Resets the frequency to 2
 * Inputs: filename--not currently used
 * Outputs: None
 * Side Effects: Sets frequency to 2
 * Return value: Zero
 */ 
int32_t rtc_open(const uint8_t* filename){
    //reset frequency to 2 hz
    int32_t f = 2;
    int32_t *f_ptr = &f;
    rtc_write(0,f_ptr,EXPECTED_BYTES); //4 is default number of bytes
    return 0;
}
/* rtc_close
 * 
 * Inputs: fd -- not currently used
 * Outputs: None
 * Side Effects: None
 * Return value: Zero
 */ 
int32_t rtc_close(int32_t fd){
    return 0;
}
