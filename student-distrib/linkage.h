#ifndef _LINKAGE_H
#define _LINKAGE_H

#include "lib.h"
#include "system_calls.h"

extern void divide();

extern void test_func();

extern void isr_wrapper0(); //rtc
extern void isr_wrapper1(); //kbd
extern void isr_wrapper2();
extern void isr_wrapper3();
extern void isr_wrapper4();
extern void isr_wrapper5();
extern void isr_wrapper6();
extern void isr_wrapper7();
extern void isr_wrapper8();
extern void isr_wrapper9();
extern void isr_wrapperA();
extern void isr_wrapperB();
extern void isr_wrapperC();
extern void isr_wrapperD();
extern void isr_wrapperE();
extern void isr_wrapperF();
extern void sys_call();

#endif
