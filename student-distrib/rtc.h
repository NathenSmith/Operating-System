#define RTC_IRQ  0x8

#define RTC_PORT 0x70
#define CMOS_PORT 0x71
#include "lib.h"


extern void initialize_rtc();
extern void rtc_handler();
