#define RTC_IRQ  0x8

#define RTC_PORT 0x70
#define CMOS_PORT 0x71
#define REGISTER_B 0x8B
#define REGISTER_C 0x0C
#define INT_TURN_ON 0x40  //turn on interrupt on CMOS PORT

#include "lib.h"

//extern function
extern void initialize_rtc();
extern void rtc_handler();
