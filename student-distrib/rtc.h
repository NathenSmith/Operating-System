#define RTC_IRQ  0x8

#define RTC_PORT 0x70
#define CMOS_PORT 0x71
#define REGISTER_A 0x8A
#define REGISTER_B 0x8B
#define REGISTER_C 0x0C
#define INT_TURN_ON 0x40  //turn on interrupt on CMOS PORT

#define RATE_MAX 0x0F
#define RATE_MIN 0x02
#define SECOND_HEX_MASK 0xF0


#define FREQ_MAX 1024

#include "lib.h"

//extern function
extern void initialize_rtc();
extern void rtc_handler();

extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t rtc_write(int32_t fd, const int32_t* buf, int32_t nbytes);
extern int32_t rtc_open(const uint8_t* filename);
extern int32_t rtc_close(int32_t fd);
