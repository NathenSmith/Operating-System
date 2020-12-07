#ifndef _LINKAGE_H
#define _LINKAGE_H

#include "lib.h"
#include "system_calls.h"

extern void divide();

extern void test_func();

extern void isr_wrapper20(); //rtc
extern void isr_wrapper21(); //kbd
extern void isr_wrapper22();
extern void isr_wrapper23();
extern void isr_wrapper24();
extern void isr_wrapper25();
extern void isr_wrapper26();
extern void isr_wrapper27();
extern void isr_wrapper28();
extern void isr_wrapper29();
extern void isr_wrapper2A();
extern void isr_wrapper2B();
extern void isr_wrapper2C();
extern void isr_wrapper2D();
extern void isr_wrapper2E();
extern void isr_wrapper2F();
extern void sys_call();

extern uint32_t save_eip;

#endif
